#pragma once

/**************************************************************************

Processor.hpp

Created by Matthew Clarke

Abract implementation of a Processor allowing multiple types to be accessed in a simple way

 **************************************************************************/

#include "../Type.hpp"
#include "../Bus/Bus.hpp"

#include <string>
#include <vector>

namespace icarus {

	class Processor {
	private:
		float m_clockRateMHz = 0.1f; // default clock rate
		std::string m_name = "Default Processor";

	public:

		void setClockRateMHz(float clockRateMHz) { if (clockRateMHz < 0) { return; } m_clockRateMHz = clockRateMHz; }
		float getClockRateMHz() { return m_clockRateMHz; }

		void setName(std::string n) { m_name = n; }
		std::string getName() { return m_name; }

		/*
		virtual void fetch()
		Virtual function overridden by the fetch process of the processor. Loads the next instruction
		*/
		virtual void fetch() = 0;

		/*
		virtual int decode()
		Virtual function overriden by the decode process of the processor. Returns the number of clock cycles the execution of the instruction will take
		*/
		virtual unsigned int decode() = 0;

		/*
		virtual void execute()
		Virtual function overriden by the execute process of the processor. Will execute the next instruction
		*/
		virtual void execute() = 0;

		/*
		virtual std::vector<uint64_t> getRegisters()
		Returns the registers of the processor
		*/
		virtual std::vector<uint64_t> getRegisters() = 0;

		/*
		virtual std::string* getRegisterNames()
		Returns a string array of the register names
		*/
		virtual std::string* getRegisterNames() = 0;

	};

}