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
			// FLAGS register constexprs
			const int FLAGS_CF = 0;
			const int FLAGS_PF = 2;
			const int FLAGS_AF = 4;
			const int FLAGS_ZF = 6;
			const int FLAGS_SF = 7;
			const int FLAGS_TF = 8;
			const int FLAGS_IE = 9;
			const int FLAGS_DF = 10;
			const int FLAGS_OF = 11;

			// Bus references
			icarus::bus::Bus& m_dataBus;
			icarus::bus::Bus& m_addressBus;

			// MMU reference
			icarus::memory::MMU& m_mmu;

			// Register definitions
			std::vector<Register16> m_registers;
			enum REGISTERS { R_AX = 0, R_BX, R_CX, R_DX, R_SI, R_DI, R_BP, R_SP, R_IP, R_FLAGS, R_CS, R_DS, R_ES, R_SS };
			static std::string REGISTER_NAMES[14];

			// Current instruction
			struct CurrentInstruction {
				std::vector<icarus::processor::instruction::Microcode> microcode;
				icarus::processor::instruction::ModRMByte modRMByte;
				uint8_t numImmeditateBytes;
				uint8_t numDisplacementBytes;
				uint16_t displacement;
				uint16_t immediate;
				uint8_t code;
				struct MicrocodeInformation {
					bool regMode8Bit; // If true, interpret registers as 8 bit regs. False = 16 bit regs
					bool srcAUsed;
					bool dstEnabled;
					struct Values {
						uint8_t bytes;
						uint16_t v;
					} srcA, srcB, dst;
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
			/*
			SRC MICROCODE
			*/
			CurrentInstruction::MicrocodeInformation::Values& mcode_getNextSrc();
			void mcode_toSrcFromReg(CurrentInstruction::MicrocodeInformation::Values& src, uint8_t sval);
			void mcode_toSrcFromMem00(CurrentInstruction::MicrocodeInformation::Values& src, uint8_t sval);
			void mcode_getSrcRegop();
			void mcode_getSrcImm();
			void mcode_getSrcModRM();
			/*
			DST MICROCODE
			*/
			void mcode_dstModRM();
			void mcode_toDstFromReg(uint8_t sval);
			void mcode_toDstFromMem00(uint8_t sval);
			/*
			FN MICROCODE
			*/
			void mcode_fnCallRel();
			void mcode_fnRetnNear();
			void mcode_fnRegop8X();
			void mcode_fnCmp();
			void mcode_fnInc();
			void mcode_fnDec();
			/*
			JMP MICROCODE
			*/
			void mcode_jmpCondRelativeShort(bool condition);
			/*
			MISC MICROCODE
			*/
			void mcode_stackPush(CurrentInstruction::MicrocodeInformation::Values& src);
			void mcode_stackPop(CurrentInstruction::MicrocodeInformation::Values& src);
			void mcode_seSrcB();

		public:
			Processor_8086(icarus::memory::MMU& mmu, icarus::bus::Bus& dataBus, icarus::bus::Bus& addressBus);

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