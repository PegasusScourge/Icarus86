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

namespace icarus {

	class Icarus86 {
	private:
		bool m_intialized = false;
		bool m_hasErrored = false;

		int m_returnValue = 0;

		icarus::Bus16 m_dataBus;
		icarus::Bus32 m_addressBus{ 20 }; // This is a 32 bit bus, but it is limited to 20 bits

		std::unique_ptr<icarus::Processor> processor;
		enum class ProcessorRequestType { PTypeNONE, PType8086 } m_requestedProcessorType;
		bool createdProcessor = false;

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