#pragma once

/**************************************************************************

P8086.hpp

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

			// Segment information
			enum SEGMENT { S_DATA, S_CODE, S_XTRA, S_STACK };

			// Current instruction
			struct CurrentInstruction {
				// List of microcode instructions to execute
				std::vector<icarus::processor::instruction::Microcode> microcode;
				// Current ModRMByte
				icarus::processor::instruction::ModRMByte modRMByte;
				// The number of immediate bytes the instruction has
				uint8_t numImmeditateBytes;
				// The number of displacement bytes the instruction has
				uint8_t numDisplacementBytes;
				// Current displacement value
				uint16_t displacement;
				// Current immediate value
				uint16_t immediate;
				// Hex code of the instruction
				uint8_t code;
				// Segment register information
				bool hasSegOverride;
				bool usedSegOverride;
				SEGMENT segOverride; // Segement override enum

				// Information for the current microcode execution
				struct MicrocodeInformation {
					// If true, interpret registers/memory locations as 8 bit. False = 16 bit
					bool bitMode8Bit;
					// If true srcA is in use and new src selection should use srcB
					bool srcAUsed;
					// If true destination writes from dst are allowed
					bool dstEnabled;
					// Src and dst holdings
					struct Cache {
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

			/*
			uint32_t getSegmentedAddress(SEGMENT defaultSeg, uint16_t offset)
			Takes into account segment overrides and converts the defaultSeg into a segment register, before using resolveAddress() to
			return the actual memory address of the location
			*/
			uint32_t getSegmentedAddress(SEGMENT defaultSeg, uint16_t offset);

			void onError() override;

			void onGetProcessorState() override;

			/*
			
			MICROCODE EXECUTION FUNCTIONS
			Definition is in Processor_8086_MC[src/fn/dst].cpp to keep code easier to read
			
			*/
			void mcode_execCode(instruction::Microcode mcode);
			/*
			SRC MICROCODE
			*/
			CurrentInstruction::MicrocodeInformation::Cache& mcode_getNextSrc();
			void mcode_toSrcFromReg(CurrentInstruction::MicrocodeInformation::Cache& src, uint8_t sval);
			void mcode_toSrcFromMem00(CurrentInstruction::MicrocodeInformation::Cache& src, uint8_t sval);
			void mcode_toSrcFromMem10(CurrentInstruction::MicrocodeInformation::Cache& src, uint8_t sval);
			void mcode_getSrcRegop();
			void mcode_getSrcImm();
			void mcode_getSrcDisp();
			void mcode_getSrcModRM();
			/*
			DST MICROCODE
			*/
			void mcode_dstModRM();
			void mcode_toDstFromReg(uint8_t sval);
			void mcode_toDstFromSReg(uint8_t sval);
			void mcode_toDstFromMem00(uint8_t sval);
			void mcode_toDstFromMem10(uint8_t sval);
			/*
			FN MICROCODE
			*/
			void mcode_fnCallRel();
			void mcode_fnRetnNear();
			void mcode_fnRegop8X();
			void mcode_fnRegopFE();
			void mcode_fnAdd(bool adc);
			void mcode_fnSub(bool sbb);
			void mcode_fnCmp();
			void mcode_fnInc();
			void mcode_fnDec();
			void mcode_fnXOR();
			void mcode_fnOR();
			void mcode_fnAND();
			/*
			JMP MICROCODE
			*/
			void mcode_jmpCondRelativeShort(bool condition);
			void mcode_jmpFar();
			/*
			MISC MICROCODE
			*/
			void mcode_stackPush(CurrentInstruction::MicrocodeInformation::Cache& c);
			void mcode_stackPop(CurrentInstruction::MicrocodeInformation::Cache& c);
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
			void forceSeg(uint64_t seg) override;
		};

	}

}