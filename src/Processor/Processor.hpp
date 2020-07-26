#pragma once

/**************************************************************************

Processor.hpp

Created by Matthew Clarke

Abract implementation of a Processor allowing multiple types to be accessed in a simple way

 **************************************************************************/

#include "../Type.hpp"
#include "../Bus/Bus.hpp"
#include "../CircularBuffer.hpp"
#include "ALU.hpp"
#include "Instruction/InstructionSet.hpp"

#include <string>
#include <vector>
#include <memory>

namespace icarus {

	namespace processor {

		class ProcessorState {
		public:
			std::vector<uint64_t> registerValues_num;
			std::vector<std::string> registerValues_str;
			std::string* registerValues_names;
			icarus::CircularBuffer<uint64_t> lastIPs;
			icarus::CircularBuffer<icarus::processor::instruction::ICode> lastICodes;
			icarus::CircularBuffer<uint32_t> lastDisplacements;
			icarus::CircularBuffer<uint32_t> lastImmediates;
			std::string flagsRegBin;
			std::string flagsNames;
		};

		class Processor {
		private:
			float m_clockRateMHz = 0.1f; // default clock rate

			// Failure flag
			bool m_failed = false;
			// HLT flag
			bool m_isHLT = false;

			virtual void onError() = 0;

		protected:
			// ALU
			icarus::processor::ALU m_alu;
			// Name
			std::string m_name = "Default Processor";
			// Instruction set
			icarus::processor::instruction::InstructionSet m_iSet;
			// Processor state
			ProcessorState m_state;

			void triggerError() {
				m_failed = true;
				onError();
			}

			/*
			virtual void onGetProcessorState()
			Called when getProcessorState() is called
			*/
			virtual void onGetProcessorState() = 0;

		public:
			void setClockRateMHz(float clockRateMHz) { if (clockRateMHz <= 0) { return; } m_clockRateMHz = clockRateMHz; }
			float getClockRateMHz() { return m_clockRateMHz; }

			std::string getName() { return m_name; }

			bool isFailed() { return m_failed; }

			void setHLT(bool h) { m_isHLT = h; }
			bool isHLT() { return m_isHLT; }

			virtual void forceIP(uint64_t ip) = 0;
			virtual void forceSP(uint64_t sp) = 0;

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
			// virtual std::vector<uint64_t> getRegisterValues() = 0;
			// virtual std::vector<std::string> getRegisterValuesAsStr() = 0;

			/*
			virtual std::string* getRegisterNames()
			Returns a string array of the register names
			*/
			// virtual std::string* getRegisterNames() = 0;

			/*
			ProcessorState& getProcessorState()
			Returns the processor state. Calls onGetProcessorState() which can be overriden to provide functionality if needed
			*/
			ProcessorState& getProcessorState() {
				onGetProcessorState();
				return m_state;
			}

		};

	}

}