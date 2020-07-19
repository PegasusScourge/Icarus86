#pragma once

/**************************************************************************

Icarus86.hpp

Created by Matthew Clarke

Entry point to the emulator

 **************************************************************************/

#include <vector>
#include <string>

#include "Bus/Bus.hpp"

namespace icarus {

	class Icarus86 {
	private:
		bool m_intialized = false;
		bool m_hasErrored = false;

		int m_returnValue = 0;

		icarus::Bus16 m_dataBus;
		icarus::Bus32 m_addressBus{ 20 }; // This is a 32 bit bus, but it is limited to 20 bits

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