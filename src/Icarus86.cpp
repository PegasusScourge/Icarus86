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

using std::cout;
using icarus::endl; // Use our endl so we don't do constant flushes to cout. This line also simplifies changing to the flushing varient if necessary.

namespace i = icarus;

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
	i::COutSys::Initialize(false);
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
	if (!m_font.loadFromFile("Consolas.ttf")) {
		i::COutSys::Println("Icarus86 failed to load Consolas.ttf font", i::COutSys::LEVEL_ERR);
		return;
	}

	i::COutSys::Println("Icarus86 running", i::COutSys::LEVEL_INFO);

	// Timing
	microsPerClock = (sf::Int64)(1000000.0f / (m_processor->getClockRateMHz() * 1000000.0f));

	// Display
	sf::RenderWindow window(sf::VideoMode(800, 600), "Icarus86");

	// Processor
	unsigned int cyclesToWait = 0;
	
	while (window.isOpen()) {
		sf::Event evt;
		while (window.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {
				window.close();
			}
		}

		processorAccumulator += processorClock.getElapsedTime().asMicroseconds();
		processorClock.restart();
		while (processorAccumulator >= microsPerClock) {
			processorAccumulator -= microsPerClock;
			if (cyclesToWait > 0) {
				cyclesToWait--;
			}
			else {
				m_processor->fetch();
				cyclesToWait = m_processor->decode();
				m_processor->execute();
			}
		}
		
		if (renderClock.getElapsedTime().asMilliseconds() >= 10) {
			renderClock.restart();

			if (m_displayStatistics)
				drawStatistics();

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

	i::COutSys::Println("[INI] Parsing INI file complete", i::COutSys::LEVEL_INFO);
}

bool i::Icarus86::createProcessor() {
	// Create the processor
	switch (m_requestedProcessorType) {
	case ProcessorRequestType::PType8086:
		m_processor = std::unique_ptr<Processor>(new i::Processor_8086(m_dataBus, m_addressBus));
		break;

	default:
		i::COutSys::Println("Unknown processor request type", i::COutSys::LEVEL_ERR);
		return false;
		break;
	}
	m_createdProcessor = true;
	return true;
}

void i::Icarus86::drawStatistics() {

}