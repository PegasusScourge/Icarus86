/**************************************************************************

Icarus86.cpp

Created by Matthew Clarke

Entry point to the emulator

 **************************************************************************/

#include "Icarus86.hpp"
#include "Constexprs.hpp"
#include "COutSys.hpp"
#include "Processor/P8086.hpp"

#include "TGUI/TGUI.hpp"

#include <iostream>
#include <iterator>
#include <fstream>

using std::cout;
using icarus::endl; // Use our endl so we don't do constant flushes to cout. This line also simplifies changing to the flushing varient if necessary.

namespace i = icarus;

constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {

	// Compile the command line arguments into strings to pass to the emulator
	std::vector<std::string> cmdArgs;
	for (int i = 0; i < argc; i++) {
		cmdArgs.push_back(std::string(argv[i]));
	}

	// For now we do raw cout writes. We will start the console output system soon enough
	cout << "Icarus86 emulator start up. Cmd args:" << endl;
	for (auto& s : cmdArgs) {
		cout << " - " << s << endl;
	}
	cout << endl << "Starting emulation:" << endl;

	// Initialize the console output system
	i::COutSys::Initialize(true);
	i::COutSys::Println("COutSys initialized", i::COutSys::LEVEL_INFO);

	// Start the emulator
	i::Icarus86 icarus;

	// Check for initilization failures
	if (icarus.failure())
		return icarus.getReturnValue();

	icarus.run();
	return icarus.getReturnValue();
}

/***********************************/
// class Icarus
/***********************************/

i::Icarus86::Icarus86() {
	m_requestedProcessorType = ProcessorRequestType::PTypeNONE;

	// Load the cfg file
	parseCFG();

	if (!m_createdProcessor) {
		i::COutSys::Println("Icarus86 failed to create processor", i::COutSys::LEVEL_INFO);
		return;
	}

	i::COutSys::Println("Processor created: name = '" + m_processor->getName() + "', clock = " + std::to_string(m_processor->getClockRateMHz()) + " MHz", i::COutSys::LEVEL_INFO);
	i::COutSys::Println("Data bus width: " + std::to_string(m_dataBus.getBitWidth()), i::COutSys::LEVEL_INFO);
	i::COutSys::Println("Address bus width: " + std::to_string(m_addressBus.getBitWidth()), i::COutSys::LEVEL_INFO);

	// Done!
	m_intialized = true;
	i::COutSys::Println("Icarus86 initialized", i::COutSys::LEVEL_INFO);
}

void i::Icarus86::run() {
	// Load the Font
	icarus::graphics::GPU::InitializeFont();
	if (!icarus::graphics::GPU::FontLoaded) {
		return;
	}

	i::COutSys::Println("Icarus86 running", i::COutSys::LEVEL_INFO);

	// Timing
	m_microsPerClock = (sf::Int64)(1000000.0f / (m_processor->getClockRateMHz() * 1000000.0f));

	// Display
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Icarus86", sf::Style::Titlebar); // Don't give a close or resize option
	tgui::Gui gui{ window };

	tgui::Theme blackTheme{ "Theme.txt" };

	tgui::Button::Ptr exitButton = tgui::Button::create();
	exitButton->setRenderer(blackTheme.getRenderer("Button"));
	exitButton->getRenderer()->setFont(icarus::graphics::GPU::Font);
	exitButton->setText("Exit");
	exitButton->setPosition("90%", 0);
	exitButton->setSize("10%", 20);
	exitButton->connect(tgui::Signals::Button::Clicked, [&]() { window.close(); });
	gui.add(exitButton, "ExitButton");

	tgui::CheckBox::Ptr statsBox = tgui::CheckBox::create();
	statsBox->setRenderer(blackTheme.getRenderer("CheckBox"));
	statsBox->getRenderer()->setFont(icarus::graphics::GPU::Font);
	statsBox->setText("Show Statistics");
	statsBox->setPosition("ExitButton.left - (ExitButton.width - width)", "ExitButton.bottom");
	statsBox->connect(tgui::Signals::CheckBox::Unchecked, [&]() { m_displayStatistics = false; });
	statsBox->connect(tgui::Signals::CheckBox::Checked, [&]() { m_displayStatistics = true; });
	statsBox->setChecked(true);
	gui.add(statsBox, "StatsBox");

	// Processor
	m_cyclesToWait = 0;

	m_runningProcessor = false;
	m_singleStep = false;
	
	while (window.isOpen()) {
		sf::Event evt;
		while (window.pollEvent(evt)) {
			gui.handleEvent(evt);

			if (evt.type == sf::Event::KeyPressed) {
				// Key press events
				switch (evt.key.code) {
				case sf::Keyboard::P:
					// Pause the running
					m_runningProcessor = false;
					break;

				case sf::Keyboard::R:
					// Start the processor
					m_runningProcessor = true;
					break;

				case sf::Keyboard::S:
					// Do a single step
					if (!m_singleStepDebounce) {
						m_runningProcessor = false;
						m_singleStep = true;
						m_singleStepDebounce = true;
					}
					break;

				default:
					break;
				}
			}
			else if (evt.type == sf::Event::KeyReleased) {
				// Key release events
				switch (evt.key.code) {
				case sf::Keyboard::S:
					m_singleStepDebounce = false;
					break;

				default:
					break;
				}
			}
		}

		m_gpu.update(m_dataBus, m_addressBus, m_mmu);

		// We only increment the accumulator if we are running the processor
		if (m_runningProcessor)
			m_processorAccumulator += m_processorClock.getElapsedTime().asMicroseconds();

		// Restart the clock
		m_processorClock.restart();

		// Prep for executing ticks, so we set the # this cycle to 0 to count
		if (m_processorAccumulator >= m_microsPerClock)
			m_cyclesPerTick = 0;

		// Limit the ammount of cycles this tick
		constexpr int maxCyclesPerTick = 20;
		if (m_processorAccumulator >= m_microsPerClock * maxCyclesPerTick)
			m_processorAccumulator = m_microsPerClock * maxCyclesPerTick;

		// Stop the processor if we fail or HLT
		if (m_processor->isHLT() || m_processor->isFailed())
			m_runningProcessor = false;

		// Single step, else run down the accumulator
		if (m_singleStep && !m_processor->isFailed()) {
			m_singleStep = false;
			processorSingleStep();
		}
		else {
			while (m_processorAccumulator >= m_microsPerClock && m_runningProcessor) {
				m_processorAccumulator -= m_microsPerClock;

				if (m_cyclesToWait > 0) {
					m_cyclesToWait--;
				}
				else {
					if (!processorSingleStep())
						break;
					if (m_processor->checkBreakpoint()) {
						// Breakpoint triggered
						i::COutSys::Println("BREAKPOINT TRIGGERED!", i::COutSys::LEVEL_INFO);
						m_runningProcessor = false;
					}
				}
				m_cyclesPerTick++;

				if (m_processor->isHLT() || m_processor->isFailed())
					m_runningProcessor = false;
			}
		}
		
		// Do rendering
		if (m_renderClock.getElapsedTime().asMilliseconds() >= 10) {
			m_renderClock.restart();

			window.clear();

			sf::Sprite gpuSprite;
			gpuSprite.setTexture(m_gpu.getTexture().getTexture(), true);
			gpuSprite.setPosition((WINDOW_WIDTH / 2.0f) - (m_gpu.getXWidth() / 2.0f), (WINDOW_HEIGHT / 2) - (m_gpu.getYWidth() / 2.0f));
			window.draw(gpuSprite);

			gui.draw();

			if (m_displayStatistics)
				drawStatistics(window);

			window.display();
		}
	}
}

bool i::Icarus86::processorSingleStep() {
	m_cyclesToWait = m_processor->fetchDecode();
	if (m_processor->isFailed()) {
		i::COutSys::Println("Icarus86 detected processor failure after fetchDecode()", i::COutSys::LEVEL_ERR);
		return false;
	}
	m_processor->execute();
	if (m_processor->isFailed()) {
		i::COutSys::Println("Icarus86 detected processor failure after execute()", i::COutSys::LEVEL_ERR);
		return false;
	}
	return true;
}

int i::Icarus86::getReturnValue() {
	return m_returnValue;
}

bool i::Icarus86::failure() {
	return m_hasErrored || !m_intialized;
}

void i::Icarus86::parseCFG() {
	i::COutSys::Println("[CFG] Parsing Icarus.json cfg file", i::COutSys::LEVEL_INFO);

	using namespace nlohmann;

	json j;
	std::ifstream stream("Icarus.json");
	if (!stream.is_open()) {
		// ERROR
		icarus::COutSys::Println("[CFG] Could not open CFG!", icarus::COutSys::LEVEL_ERR);
		return;
	}
	stream >> j;

	// Check that we have the right header information
	if (!j["icarus"].is_boolean() && j["icarus"].get<bool>()) {
		// ERROR
		icarus::COutSys::Println("[CFG] Header read error", icarus::COutSys::LEVEL_ERR);
		return;
	}

	// Get the processor specification
	i::COutSys::Println("[CFG] Detecting processor spec...", i::COutSys::LEVEL_INFO);
	if (j["processor"].is_object()) {
		auto& pSpec = j["processor"];

		if (pSpec["type"].is_string()) {
			std::string v = pSpec["type"].get<std::string>();
			if (v.compare("8086") == 0) {
				i::COutSys::Println("[CFG] Requested type of 8086 processor", i::COutSys::LEVEL_INFO);
				m_requestedProcessorType = ProcessorRequestType::PType8086;
			}
		}
		if (!createProcessor())
			return;

		if (pSpec["clockMHz"].is_number()) {
			m_processor->setClockRateMHz(pSpec["clockMHz"].get<float>());
			i::COutSys::Println("[CFG] Requested clockMHz of " + std::to_string(pSpec["clockMHz"].get<float>()), i::COutSys::LEVEL_INFO);
		}

		if (pSpec["force_ip"].is_number()) {
			m_processor->forceIP(pSpec["force_ip"].get<unsigned long>());
			i::COutSys::Println("[CFG] Requested forcedIP of " + std::to_string(pSpec["force_ip"].get<unsigned long>()), i::COutSys::LEVEL_INFO);
		}

		if (pSpec["force_sp"].is_number()) {
			m_processor->forceSP(pSpec["force_sp"].get<unsigned long>());
			i::COutSys::Println("[CFG] Requested forcedSP of " + std::to_string(pSpec["force_sp"].get<unsigned long>()), i::COutSys::LEVEL_INFO);
		}

		// Check for breakpoints
		if (pSpec["breakpoints"].is_array()) {
			i::COutSys::Println("<breakpoints>", i::COutSys::LEVEL_INFO);
			for (auto& bp : pSpec["breakpoints"]) {
				icarus::processor::Breakpoint breakpoint;
				if (bp["byIP"].is_boolean() && bp["ip"].is_string()) {
					breakpoint.byAddress = bp["byIP"].get<bool>();
					std::string hex = bp["ip"].get<std::string>();
					// Convert the code to hex number
					size_t code;
					std::stringstream ss;
					ss << std::hex << hex;
					ss >> code;
					breakpoint.address = (uint64_t)code;
					if (breakpoint.byAddress)
						i::COutSys::Println("[CFG] Added breakpoint on IP = " + std::to_string(breakpoint.address), i::COutSys::LEVEL_INFO);
				}
				if (bp["byInstr"].is_boolean() && bp["instrhex"].is_string()) {
					breakpoint.byInstruction = bp["byInstr"].get<bool>();
					std::string hex = bp["instrhex"].get<std::string>();
					// Convert the code to hex number
					size_t code;
					std::stringstream ss;
					ss << std::hex << hex;
					ss >> code;
					breakpoint.instruction = (uint8_t)code;
					if (breakpoint.byInstruction)
						i::COutSys::Println("[CFG] Added breakpoint on instrhex = " + hex, i::COutSys::LEVEL_INFO);
				}
				if (breakpoint.byAddress || breakpoint.byInstruction) {
					m_processor->addBreakpoint(breakpoint);
				}
			}
			i::COutSys::Println("</breakpoints>", i::COutSys::LEVEL_INFO);
		}
	}
	else {
		i::COutSys::Println("[CFG] No processor declared!", i::COutSys::LEVEL_ERR);
	}

	i::COutSys::Println("[CFG] Detecting memory spec...", i::COutSys::LEVEL_INFO);
	if (j["memory"].is_array()) {
		auto& mSpec = j["memory"];
		for (auto& block : mSpec) {
			if (!block.is_object()) {
				i::COutSys::Println("[CFG] Memory block creation failed: not object", i::COutSys::LEVEL_ERR);
				continue;
			}

			if (!block["size"].is_number() || !block["startAddress"].is_number()) {
				i::COutSys::Println("[CFG] Memory block creation failed: missing size and address spec", i::COutSys::LEVEL_ERR);
				continue;
			}

			unsigned int size = block["size"]; 
			unsigned int startAddress = block["startAddress"];
			m_mmu.addMemoryBlock(startAddress, size);
			i::COutSys::Println("[CFG] Memory block created with address=" + std::to_string(startAddress) + ", size=" + std::to_string(size), i::COutSys::LEVEL_INFO);

			if (!memoryTest(startAddress, size)) {
				i::COutSys::Println("[CFG] Memory block failed memory test!", i::COutSys::LEVEL_ERR);
			}
		}
	}
	else {
		i::COutSys::Println("[CFG] No memory declared!", i::COutSys::LEVEL_ERR);
	}

	i::COutSys::Println("[CFG] Detecting force_load spec...", i::COutSys::LEVEL_INFO);
	// Check if we need to force load something into memory
	if (j["force_load"].is_object()) {
		auto& fSpec = j["force_load"];
		i::COutSys::Println("[CFG] Found force_load spec. Detecting components", i::COutSys::LEVEL_INFO);
		if (!fSpec["address"].is_number() || !fSpec["path"].is_string()) {
			i::COutSys::Print("[CFG] force_load configuration error", i::COutSys::LEVEL_ERR);
		}
		else {
			i::COutSys::Println("[CFG] Getting force_load address", i::COutSys::LEVEL_INFO);
			unsigned long address = fSpec["address"].get<unsigned long>();
			i::COutSys::Println("[CFG] Getting force_load path", i::COutSys::LEVEL_INFO);
			std::string path = fSpec["path"].get<std::string>();
			i::COutSys::Println("[CFG] Requested force load of file '" + path + "'", i::COutSys::LEVEL_INFO);

			std::ifstream is(path, std::ios::in | std::ios::binary);
			std::vector<char> binaryContent((std::istreambuf_iterator<char>(is)), (std::istreambuf_iterator<char>()));

			i::COutSys::Println("[CFG] Got content of file: size=" + i::COutSys::ToHexStr(binaryContent.size(), true), i::COutSys::LEVEL_INFO);
			i::COutSys::Print("[CFG] Content dump:", i::COutSys::LEVEL_INFO);
			for (size_t i = 0; i < binaryContent.size(); i++) {
				if (i % 16 == 0) {
					i::COutSys::Println("");
					i::COutSys::Print("[" + i::COutSys::ToHexStr(i) + "] ");
				}
				i::COutSys::Print(i::COutSys::ToHexStr((uint8_t)binaryContent[i]) + " ");
			}
			i::COutSys::Println("");
			// Load the file
			i::COutSys::Println("[CFG] Loading file into memory ", i::COutSys::LEVEL_INFO);
			m_addressBus.putData(0);
			for (auto& v : binaryContent) {
				m_dataBus.putData((uint8_t)v);
				m_mmu.writeByte(m_dataBus, m_addressBus);
				m_addressBus.putData(m_addressBus.readData() + 1);
			}
			i::COutSys::Println("[CFG] Loading complete", i::COutSys::LEVEL_INFO);
		}
	}

	i::COutSys::Println("[CFG] Parsing CFG file complete", i::COutSys::LEVEL_INFO);
}

bool i::Icarus86::createProcessor() {
	// Create the processor
	switch (m_requestedProcessorType) {
	case ProcessorRequestType::PType8086:
		m_processor = std::unique_ptr<i::processor::Processor>(new i::processor::Processor_8086(m_mmu, m_dataBus, m_addressBus));
		break;

	default:
		i::COutSys::Println("Unknown processor request type", i::COutSys::LEVEL_ERR);
		return false;
		break;
	}
	m_createdProcessor = true;
	return true;
}

bool i::Icarus86::memoryTest(size_t startAddress, size_t size) {
#ifdef MMU_DEBUG_PRINT
	return true;
#endif

	size_t address = startAddress;
	bool failure = false;
	int tried = 0, failedBytes = 0;
	while (address < startAddress + size) {
		m_addressBus.putData((uint32_t)address);
		if (!m_mmu.tryReadByte(m_addressBus, m_dataBus)) {
			failure = true;
			failedBytes++;
		}
		tried++;
		address++;
	}
	i::COutSys::Println("Memory test from address=" + std::to_string(startAddress) + " size=" + std::to_string(size) + ", bytesTried=" + std::to_string(tried) + " failedBytes=" + std::to_string(failedBytes), i::COutSys::LEVEL_INFO);
	return !failure;
}

void i::Icarus86::drawStatistics(sf::RenderWindow& window) {
	float x = 10;
	float y = 10;
	float tempY = y;
	float tempX = x;

	icarus::processor::ProcessorState& pState = m_processor->getProcessorState();
	
	sf::Text text;
	text.setFont(icarus::graphics::GPU::Font);
	text.setCharacterSize(12);
	text.setOutlineThickness(2.0f);
	text.setOutlineColor(sf::Color::Black);

	// Draw the CPU information
	text.setFillColor(sf::Color::Cyan);
	text.setString("CPU name: " + m_processor->getName());
	text.setPosition(x, y); y += 12;
	window.draw(text);
	text.setString("CPU clock freq (MHz): " + std::to_string(m_processor->getClockRateMHz()));
	text.setPosition(x, y); y += 12;
	window.draw(text);
	text.setString("Microseconds/clock: " + std::to_string(m_microsPerClock));
	text.setPosition(x, y); y += 12;
	window.draw(text);
	text.setString("Cycles per tick: " + std::to_string(m_cyclesPerTick));
	text.setPosition(x, y);  y += 12;
	window.draw(text);
	text.setString("ProcessorAccumulator: " + std::to_string(m_processorAccumulator));
	text.setPosition(x, y); y += 12;
	window.draw(text);
	text.setString("Wait cycles: " + std::to_string(m_cyclesToWait));
	text.setPosition(x, y); y += 12;
	window.draw(text);

	text.setString("SingleStep:       " + (m_singleStep ? std::string("true") : std::string("false")));
	text.setPosition(x, y); y += 12;
	window.draw(text);
	text.setString("RunningProcessor: " + (m_runningProcessor ? std::string("true") : std::string("false")));
	text.setPosition(x, y); y += 12;
	window.draw(text);
	text.setString("ProcessorHLT:     " + (m_processor->isHLT() ? std::string("true") : std::string("false")));
	text.setPosition(x, y); y += 12;
	window.draw(text);
	text.setString("ProcessorFail:    " + (m_processor->isFailed() ? std::string("true") : std::string("false")));
	text.setPosition(x, y); y += 12;
	window.draw(text);

	y += 20; tempY = y;

	// Draw the registers
	text.setFillColor(sf::Color::Cyan);
	text.setString("Registers:");
	text.setPosition(x, y); y += 14;
	window.draw(text);

	std::vector<std::string>& regValues = pState.registerValues_str;
	text.setFillColor(sf::Color::White);
	for (int i = 0; i < regValues.size(); i++, y += 12) {
		text.setString("[" + pState.registerValues_names[i] + "]: " + regValues.at(i));
		text.setPosition(x, y);
		window.draw(text);
	}

	float tempY2 = y;
	y = tempY; tempX = x; x += 200;

	// Draw the flags 
	text.setFillColor(sf::Color::Cyan);
	text.setString("Flags:");
	text.setPosition(x, y); y += 14;
	window.draw(text);
	text.setFillColor(sf::Color::White);
	text.setString(pState.flagsRegBin);
	text.setPosition(x, y); y += 14;
	window.draw(text);
	text.setFillColor(sf::Color::Cyan);
	text.setString(pState.flagsNames);
	text.setPosition(x, y); y += 14;
	window.draw(text);

	y = tempY2 + 20; x = tempX;

	// Draw the bus values
	text.setFillColor(sf::Color::Cyan);
	text.setString("Data Bus Value:    " + COutSys::ToHexStr(m_dataBus.readData(), true));
	text.setPosition(x, y); y += 14;
	window.draw(text);
	text.setString("Address Bus Value: " + COutSys::ToHexStr(m_addressBus.readData(), true));
	text.setPosition(x, y); y += 14;
	window.draw(text);

	y += 20;

	// Draw the lastIPs
	text.setFillColor(sf::Color::Cyan);
	text.setString("LastIPs:");
	text.setPosition(x, y); y += 14;
	window.draw(text);

	text.setFillColor(sf::Color::White);
	for (int i = 0; i < pState.lastInstrs.size(); i++, y += 12) {
		text.setString("[" + std::to_string(i) + "]: " + COutSys::ToHexStr(pState.lastInstrs[i].ip));
		text.setPosition(x, y);
		window.draw(text);
	}

	y += 20; tempY = y; tempX = x;

	// Draw the lastICodes
	text.setFillColor(sf::Color::Cyan);
	text.setString("LastICodes:");
	text.setPosition(x, y); y += 14;
	window.draw(text);

	text.setFillColor(sf::Color::White);
	for (int i = 0; i < pState.lastInstrs.size(); i++, y += 12) {
		auto& iCode = pState.lastInstrs[i].iCode;
		text.setString("[" + std::to_string(i) + "]: " + COutSys::ToHexStr(iCode.getCode()) + ", valid=" + std::to_string(iCode.isValid()) + 
		", prefix=" + COutSys::ToHexStr(iCode.getPrefix()) + " (" + iCode.getMnemonic() + ")");
		text.setPosition(x, y);
		window.draw(text);
	}

	y = tempY; x += 500;

	// Draw the lastDisplacements
	text.setFillColor(sf::Color::Cyan);
	text.setString("LastDisplacements:");
	text.setPosition(x, y); y += 14;
	window.draw(text);

	text.setFillColor(sf::Color::White);
	for (int i = 0; i < pState.lastInstrs.size(); i++, y += 12) {
		auto& lDisps = pState.lastInstrs[i].disp;
		text.setString("[" + std::to_string(i) + "]: " + COutSys::ToHexStr(lDisps));
		text.setPosition(x, y);
		window.draw(text);
	}

	y = tempY; x += 150;

	// Draw the lastImmediates
	text.setFillColor(sf::Color::Cyan);
	text.setString("LastImmediates:");
	text.setPosition(x, y); y += 14;
	window.draw(text);

	text.setFillColor(sf::Color::White);
	for (int i = 0; i < pState.lastInstrs.size(); i++, y += 12) {
		auto& lImm = pState.lastInstrs[i].imm;
		text.setString("[" + std::to_string(i) + "]: " + COutSys::ToHexStr(lImm));
		text.setPosition(x, y);
		window.draw(text);
	}

	y += 20; x = tempX;
}