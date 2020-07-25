#pragma once

/**************************************************************************

Processor_8086.hpp

Created by Matthew Clarke

Implementation of the 8086 processor

 **************************************************************************/

#include "Processor.hpp"

#include "../Bus/Bus.hpp"
#include "../Memory/MMU.hpp"
#include "Register.hpp"

#include <vector>

namespace icarus {

	namespace processor {

		class Processor_8086 : public Processor {
		private:
			// Bus references
			icarus::bus::Bus16& m_dataBus;
			icarus::bus::Bus32& m_addressBus;

			// MMU reference
			icarus::memory::MMU& m_mmu;

			// Register definitions
			std::vector<Register16> m_registers;
			enum class REGISTERS { R_AX = 0, R_BX, R_CX, R_DX, R_SI, R_DI, R_BP, R_SP, R_IP, R_FLAGS, R_CS, R_DS, R_ES, R_SS };
			static std::string REGISTER_NAMES[14];

			// Current instruction
			struct CurrentInstruction {
				std::vector<icarus::processor::instruction::Microcode> microcode;
				icarus::processor::instruction::ModRMByte modRMByte;
				uint16_t displacement;
				uint16_t immediate;
				uint8_t code;
				struct MicrocodeInformation {
					bool regMode8Bit; // If true, interpret registers as 8 bit regs. False = 16 bit regs
					bool srcAUsed;
					struct Source {
						uint8_t bytes;
						uint16_t v;
					} srcA, srcB;
				} mCodeI;
			} m_cInstr;

			/*
			uint32_t resolveAddress(uint16_t segment, uint16_t offset)
			Returns the resolved address of a segment-offset register pair
			*/
			uint32_t resolveAddress(uint16_t segment, uint16_t offset);

			void onError() override;

			void onGetProcessorState() override;

			/*
			
			MICROCODE EXECUTION FUNCTIONS
			Definition is in Processor_8086_MCODEEXEC.cpp to keep code easier to read
			
			*/
			void mcode_execCode(instruction::Microcode mcode);
			void mcode_toSrcFromReg(CurrentInstruction::MicrocodeInformation::Source& src, uint8_t sval);
			void mcode_getSrcRegop();
			void mcode_getSrcImm();
			void mcode_getSrcModRM();

		public:
			Processor_8086(icarus::memory::MMU& mmu, icarus::bus::Bus16& dataBus, icarus::bus::Bus32& addressBus);

			/*
			Overridden main functions
			*/
			unsigned int fetchDecode() override;
			void execute() override;

			void forceIP(uint64_t ip) override;
			void forceSP(uint64_t sp) override;
		};

	}

}