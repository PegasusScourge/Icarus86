#pragma once

/**************************************************************************

Processor.hpp

Created by Matthew Clarke

Abract implementation of a Processor allowing multiple types to be accessed in a simple way

 **************************************************************************/

#include "../Type.hpp"
#include "../Bus/Bus.hpp"
#include "ALU.hpp"
#include "Instruction/InstructionSet.hpp"

#include <string>
#include <vector>
#include <memory>

namespace icarus {

	namespace processor {

		class Processor {
		private:
			float m_clockRateMHz = 0.1f; // default clock rate

			virtual void onErrorDumpToConsole() = 0;

		protected:
			// ALU
			icarus::processor::ALU m_alu;
			// Name
			std::string m_name = "Default Processor";
			// Instruction set
			icarus::processor::instruction::InstructionSet m_iSet;
			// Failure flag
			bool m_failed = false;

		public:
			void setClockRateMHz(float clockRateMHz) { if (clockRateMHz <= 0) { return; } m_clockRateMHz = clockRateMHz; }
			float getClockRateMHz() { return m_clockRateMHz; }

			std::string getName() { return m_name; }

			bool isFailed() { return m_failed; }

			/*
			virtual int fetchDecode()
			Virtual function overriden by the fetch + decode. Returns the number of clock cycles the execution of the instruction will take
			*/
			virtual unsigned int fetchDecode() = 0;

			/*
			virtual void execute()
			Virtual function overriden by the execute process of the processor. Will execute the next instruction
			*/
			virtual void execute() = 0;

			/*
			virtual std::vector<uint64_t> getRegisterValues()
			virtual std::vector<std::string> getRegisterValuesAsStr()
			Returns the registers of the processor
			*/
			virtual std::vector<uint64_t> getRegisterValues() = 0;
			virtual std::vector<std::string> getRegisterValuesAsStr() = 0;

			/*
			virtual std::string* getRegisterNames()
			Returns a string array of the register names
			*/
			virtual std::string* getRegisterNames() = 0;

		};

	}

}