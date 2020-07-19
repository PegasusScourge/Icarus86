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
		icarus::Bus16 m_dataBus;
		icarus::Bus32 m_addressBus{ 20 }; // This is a 32 bit bus, but it is limited to 20 bits

		// Processor information
		std::unique_ptr<icarus::Processor> m_processor;
		enum class ProcessorRequestType { PTypeNONE, PType8086 } m_requestedProcessorType;
		bool m_createdProcessor = false;

		sf::Clock processorClock;
		sf::Int64 processorAccumulator = 0;
		sf::Int64 microsPerClock = 0;
		sf::Clock renderClock;

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