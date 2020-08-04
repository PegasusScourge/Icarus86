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

		typedef struct LastInstruction {
			uint32_t ip;
			icarus::processor::instruction::ICode iCode;
			uint32_t disp;
			uint32_t imm;
		} LastInstruction_t;

		class Breakpoint {
		public:
			bool byAddress;
			bool byInstruction;
			uint64_t address;
			uint8_t instruction;
		};

		class ProcessorState {
		public:
			std::vector<uint64_t> registerValues_num;
			std::vector<std::string> registerValues_str;
			std::string* registerValues_names;
			std::string flagsRegBin;
			std::string flagsNames;
			icarus::CircularBuffer<LastInstruction_t> lastInstrs;
		};

		class Processor {
		private:
			float m_clockRateMHz = 0.1f; // default clock rate

			// Failure flag
			bool m_failed = false;
			// HLT flag
			bool m_isHLT = false;

			// Breakpoints
			std::vector<Breakpoint> m_breakpoints;

			// Interrupt schedules
			std::vector<uint8_t> m_intCodeSchedule;

			virtual void onError() = 0;

			/*
			virtual void onGetProcessorState()
			Called when getProcessorState() is called
			*/
			virtual void onGetProcessorState() = 0;

		protected:
			// ALU
			icarus::processor::ALU m_alu;
			// Name
			std::string m_name = "Default Processor";
			// Instruction set
			icarus::processor::instruction::InstructionSet m_iSet;
			// Processor state
			ProcessorState m_state;

			// Reset
			bool m_isReset = true;

			void triggerError();

			bool hasInterrruptRequests();
			uint8_t getNextInterruptRequest();

		public:
			void setClockRateMHz(float clockRateMHz);
			float getClockRateMHz();

			std::string getName();

			bool isFailed();

			void setHLT(bool h);
			bool isHLT();

			virtual void forceIP(uint64_t ip) = 0;
			virtual void forceSP(uint64_t sp) = 0;
			virtual void forceSeg(uint64_t seg) = 0;

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
			ProcessorState& getProcessorState();

			/*
			void addBreakpoint(Breakpoint bp)
			Adds a breakpoint
			*/
			void addBreakpoint(Breakpoint bp);

			/*
			bool checkBreakpoint()
			Returns true if a breakpoint has been reached
			*/
			bool checkBreakpoint();

			/*
			void requestInterrupt(uint8_t intCode)
			Schedules an interrupt as if INT 0x(intCode) had been called
			*/
			void requestInterrupt(uint8_t intCode);

		};

	}

}