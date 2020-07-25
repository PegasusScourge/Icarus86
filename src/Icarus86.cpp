/**************************************************************************

Icarus86.cpp

Created by Matthew Clarke

Entry point to the emulator

 **************************************************************************/

#include "Icarus86.hpp"
#include "Constexprs.hpp"
#include "COutSys.hpp"
#include "Processor/Processor_8086.hpp"

#include "SimpleIni/SimpleIni.h"

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

	// Load the ini file
	parseINI();

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
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Icarus86");

	// Processor
	m_cyclesToWait = 0;

	m_runningProcessor = true;
	m_singleStep = false;
	
	while (window.isOpen()) {
		sf::Event evt;
		while (window.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {
				window.close();
			}
		}

		m_gpu.update(m_dataBus, m_addressBus, m_mmu);

		m_processorAccumulator += m_processorClock.getElapsedTime().asMicroseconds();
		m_processorClock.restart();

		if (m_processorAccumulator >= m_microsPerClock)
			m_cyclesPerTick = 0;

		if (m_processorAccumulator >= m_microsPerClock * 2000)
			m_processorAccumulator = m_microsPerClock * 2000;

		if (m_processor->isHLT() || m_processor->isFailed())
			m_runningProcessor = false;

		while (m_processorAccumulator >= m_microsPerClock && m_runningProcessor) {
			m_processorAccumulator -= m_microsPerClock;
			
			if (m_cyclesToWait > 0) {
				m_cyclesToWait--;
			}
			else {
				if (m_singleStep) {
					m_runningProcessor = false;
				}

				m_cyclesToWait = m_processor->fetchDecode();
				if (m_processor->isFailed()) {
					i::COutSys::Println("Icarus86 detected processor failure after fetchDecode()", i::COutSys::LEVEL_ERR);
					break;
				}
				m_processor->execute();
				if (m_processor->isFailed()) {
					i::COutSys::Println("Icarus86 detected processor failure after execute()", i::COutSys::LEVEL_ERR);
					break;
				}
			}
			m_cyclesPerTick++;
		}
		
		if (m_renderClock.getElapsedTime().asMilliseconds() >= 10) {
			m_renderClock.restart();

			window.clear();

			sf::Sprite gpuSprite;
			gpuSprite.setTexture(m_gpu.getTexture().getTexture(), true);
			gpuSprite.setPosition((WINDOW_WIDTH / 2.0f) - (m_gpu.getXWidth() / 2.0f), (WINDOW_HEIGHT / 2) - (m_gpu.getYWidth() / 2.0f));
			window.draw(gpuSprite);

			if (m_displayStatistics)
				drawStatistics(window);

			window.display();
		}
	}
}

// Setters


// Getters

int i::Icarus86::getReturnValue() {
	return m_returnValue;
}

bool i::Icarus86::failure() {
	return m_hasErrored || !m_intialized;
}

void i::Icarus86::parseINI() {
	i::COutSys::Println("[INI] Parsing INI file", i::COutSys::LEVEL_INFO);

	CSimpleIniA ini;
	ini.SetUnicode();
	SI_Error e = ini.LoadFile("Icarus.ini");
	if (e != SI_OK) {
		// Failed to read iniFile
		i::COutSys::Println("[INI] Failed to read ini file 'Icarus.ini'", i::COutSys::LEVEL_ERR);
		return;
	}
	i::COutSys::Println("[INI] File loaded", i::COutSys::LEVEL_INFO);
	
	const char* value;

	// Get the processor specification
	value = ini.GetValue("processor", "type");
	if (value) {
		std::string v{ value };
		if (v.compare("8086") == 0) {
			i::COutSys::Println("[INI] Requested type of 8086 processor", i::COutSys::LEVEL_INFO);
			m_requestedProcessorType = ProcessorRequestType::PType8086;
		}
	}
	if (!createProcessor())
		return;

	value = ini.GetValue("processor", "clockMHz");
	if (value) {
		m_processor->setClockRateMHz(std::stof(value));
		i::COutSys::Println("[INI] Requested clockMHz of " + std::string(value), i::COutSys::LEVEL_INFO);
	}

	value = ini.GetValue("processor", "force_ip");
	if (value) {
		m_processor->forceIP(std::stoul(value));
		i::COutSys::Println("[INI] Requested forcedIP of " + std::string(value), i::COutSys::LEVEL_INFO);
	}

	value = ini.GetValue("processor", "force_sp");
	if (value) {
		m_processor->forceSP(std::stoul(value));
		i::COutSys::Println("[INI] Requested forcedSP of " + std::string(value), i::COutSys::LEVEL_INFO);
	}

	// Get memory specification
	value = ini.GetValue("memory", "blocks");
	if (value) {
		int numberOfMemoryBlocks = std::stoi(value);
		if (numberOfMemoryBlocks > 0) {
			// We have memory blocks to decode
			int currentMemoryBlock = 0;
			std::string blockBase = "memory_block_";
			const char* startAddress;
			for (; currentMemoryBlock < numberOfMemoryBlocks; currentMemoryBlock++) {
				std::string bName = blockBase + std::to_string(currentMemoryBlock);
				value = ini.GetValue(bName.c_str(), "size"); startAddress = ini.GetValue(bName.c_str(), "startAddress");
				if (value && startAddress) {
					m_mmu.addMemoryBlock(std::stoul(startAddress), std::stoul(value));
					i::COutSys::Println("[INI] Memory block '" + bName + "' created with address=" + std::string{ startAddress } + ", size=" + std::string{ value }, i::COutSys::LEVEL_INFO);
				
					if (!memoryTest(std::stoul(startAddress), std::stoul(value))) {
						i::COutSys::Println("[INI] Memory block '" + bName + "'failed memory test!", i::COutSys::LEVEL_ERR);
					}
				}
				else {
					i::COutSys::Println("[INI] Memory block '" + bName + "' couldn't be found!", i::COutSys::LEVEL_WARN);
				}
			}
		}
	}

	// Check if we need to force load something into memory
	value = ini.GetValue("force_load", "path");
	if (value) {
		std::string path{ value };
		value = ini.GetValue("force_load", "address");
		if (value) {
			size_t address = std::stoul(value);
			i::COutSys::Println("[INI] Requested force load of file '" + path + "'", i::COutSys::LEVEL_INFO);

			std::ifstream is(path, std::ios::binary);
			std::istream_iterator<uint8_t> start(is), end;
			std::vector<uint8_t> binaryContent(start, end);
			i::COutSys::Println("[INI] Got content of file: size=" + i::COutSys::ToHexStr(binaryContent.size(), true), i::COutSys::LEVEL_INFO);
			i::COutSys::Print("[INI] Content dump:", i::COutSys::LEVEL_INFO);
			for (size_t i = 0; i < binaryContent.size(); i++) {
				if (i % 32 == 0) {
					cout << endl << "[" << i::COutSys::ToHexStr(i) << "] ";
				}
				cout << i::COutSys::ToHexStr(binaryContent[i]) << " ";
			}
			cout << endl;
			// Load the file
			i::COutSys::Println("[INI] Loading file into memory ", i::COutSys::LEVEL_INFO);
			m_addressBus.putData(0);
			for (auto& v : binaryContent) {
				m_dataBus.putData(v);
				m_mmu.writeByte(m_dataBus, m_addressBus);
				m_addressBus.putData(m_addressBus.readData() + 1);
			}
			i::COutSys::Println("[INI] Loading complete", i::COutSys::LEVEL_INFO);
		}
	}

	i::COutSys::Println("[INI] Parsing INI file complete", i::COutSys::LEVEL_INFO);
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

	y += 20;

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

	y += 20;

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
	for (int i = 0; i < pState.lastIPs.size(); i++, y += 12) {
		text.setString("[" + std::to_string(i) + "]: " + COutSys::ToHexStr(pState.lastIPs[i]));
		text.setPosition(x, y);
		window.draw(text);
	}

	y += 20; float tempY = y;

	// Draw the lastICodes
	text.setFillColor(sf::Color::Cyan);
	text.setString("LastICodes:");
	text.setPosition(x, y); y += 14;
	window.draw(text);

	text.setFillColor(sf::Color::White);
	for (int i = 0; i < pState.lastICodes.size(); i++, y += 12) {
		auto& iCode = pState.lastICodes[i];
		text.setString("[" + std::to_string(i) + "]: " + COutSys::ToHexStr(iCode.getCode()) + ", valid=" + std::to_string(iCode.isValid()) + 
		", prefix=" + COutSys::ToHexStr(iCode.getPrefix()));
		text.setPosition(x, y);
		window.draw(text);
	}

	y = tempY; x += 300;

	// Draw the lastDisplacements
	text.setFillColor(sf::Color::Cyan);
	text.setString("LastDisplacements:");
	text.setPosition(x, y); y += 14;
	window.draw(text);

	text.setFillColor(sf::Color::White);
	for (int i = 0; i < pState.lastDisplacements.size(); i++, y += 12) {
		auto& lDisps = pState.lastDisplacements[i];
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
	for (int i = 0; i < pState.lastImmediates.size(); i++, y += 12) {
		auto& lImm = pState.lastImmediates[i];
		text.setString("[" + std::to_string(i) + "]: " + COutSys::ToHexStr(lImm));
		text.setPosition(x, y);
		window.draw(text);
	}
}