#pragma once

/**************************************************************************

Icarus86.hpp

Created by Matthew Clarke

Entry point to the emulator

 **************************************************************************/

#include <vector>
#include <string>
#include <memory>

#include "Bus/Bus.hpp"
#include "Processor/Processor.hpp"
#include "Memory/MMU.hpp"

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"

namespace icarus {

	class Icarus86 {
	private:
		// Icarus status information
		bool m_intialized = false;
		bool m_hasErrored = false;

		int m_returnValue = 0;

		// Buses
		icarus::bus::Bus16 m_dataBus;
		icarus::bus::Bus32 m_addressBus{ 20 }; // This is a 32 bit bus, but it is limited to 20 bits

		// Processor information
		std::unique_ptr<icarus::processor::Processor> m_processor;
		enum class ProcessorRequestType { PTypeNONE, PType8086 } m_requestedProcessorType;
		bool m_createdProcessor = false;

		sf::Clock m_processorClock;
		sf::Int64 m_processorAccumulator = 0;
		sf::Int64 m_microsPerClock = 0;
		sf::Clock m_renderClock;

		unsigned int m_cyclesPerTick = 0;

		// Memory Management Unit
		icarus::memory::MMU m_mmu;

		// Graphics display information
		bool m_displayStatistics = true;

		sf::Font m_font;

		/*
		void parseINI()
		Parses the INI file
		*/
		void parseINI();

		/*
		bool createProcessor()
		Creates the processor
		*/
		bool createProcessor();

		/*
		bool memoryTest(size_t startAddress, size_t size)
		Tests memory in the range
		*/
		bool memoryTest(size_t startAddress, size_t size);

		/*
		void drawStatistics()
		Displays the statistics on the screen
		*/
		void drawStatistics(sf::RenderWindow& window);

	public:
		Icarus86();

		/*
		void run()
		Runs the emulator
		*/
		void run();
		
		// Setters


		// Getters

		/*
		int getReturnValue()
		Returns the return value for the exit status of the program
		*/
		int getReturnValue();

		/*
		bool failure()
		Returns true if there has been an internal failure
		*/
		bool failure();

	};

}