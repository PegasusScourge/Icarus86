#pragma once

/**************************************************************************

Microcode.hpp

Created by Matthew Clarke

A set of microcode instructions to complete

 **************************************************************************/

#include "../../Type.hpp"

#include <string>

namespace icarus {

	namespace processor {

		namespace instruction {

			class Microcode {
			public:
				enum class MicrocodeType {
					UDEF = 0,
					NOP,
					HLT,

					REG_8, // Switches to registers being interpreted as the 8 bit registers
					REG_16, // Siwtches to registers being interpreted as the 16 bit registers

					SE_SRC_B, // Sign extends SRC_B to match the byte length of SRC_A

					SRC_REGOP, // A source is deduced from the REGOP
					SRC_MODRM,  // A source is deduced from the MODRM
					SRC_IMM, // A source is the immediate byte(s)

					SRC_STACK_POP, // Pop from the stack into a SRC

					SRC_R_BX, // Source is register BX, not deduced
					SRC_R_CX, // Source is register CX, not deduced
					SRC_R_DX, // Source is register DX, not deduced
					SRC_R_DI, // Source is register DI, not deduced

					DST_REGOP, // Destination is deduced from the REGOP
					DST_MODRM, // Destination is deduced from the MODRM

					DST_STACK_PUSH, // Destination is pushing to the stack

					DST_R_AL, // Destination is register AL, not deduced
					DST_R_AX, // Destination is register AX, not deduced
					DST_R_BX, // Destination is register BX, not deduced
					DST_R_CX, // Destination is register CX, not deduced
					DST_R_DX, // Destination is register DX, not deduced
					DST_R_SP, // Destination is register SP, not deduced
					DST_R_DI, // Destination is register DI, not deduced

					FN_ADD,
					FN_ADC,
					FN_SUB,
					FN_SBB,
					FN_OR,
					FN_XOR,
					FN_AND,
					FN_CMP,
					FN_INC,
					FN_DEC,

					FN_CLC, // Clear carry flag
					FN_STC, // Set carry flag

					FN_MOV,
					FN_XCHG,

					FN_CALL_REL, // Call relative. SRC_A should have the relative address, relative to IP after fetchDecode. Push IP to stack first

					FN_RETN_NEAR, // Return near. SRC_A should have the return IP 

					FN_JMP, // Jump
					FN_JZ, // Jump if Zero flag set, takes immediate byte as relative to PC for jump. Relative is SIGNED
					FN_JNZ, // Jump is Zero flag not set, same as above otherwise
					FN_JC, // Jump if carry flag is set
					FN_JNC, // Jump if carry flag not set
					FN_JBE, // Jump if carry or zero flag set
					FN_JNBE, // Jump if carry and zero flag not set
					FN_JS, // Jump if sign flag set
					FN_JNS, // Jump if sign flag not set

					FN_APASS, // Passes srcA to dst and sets dstEnabled
					FN_BPASS, // Passes srcB to dst and sets dstEnabled

					FN_REGOP_8X, // Opcode 81-83 regop function family as decided by the REGOP field of the ModRM byte. Decays to one of the FNs
					FN_REGOP_FE, // Opcode for FE regop fucntion family, decided by REGOP field of ModRM byte. Either 0=INC or 1=DEC
				};

				static MicrocodeType GetTypeFromString(std::string s);

				Microcode(std::string tStr, MicrocodeType type);

				MicrocodeType getType();
				std::string getTStr();

			private:
				MicrocodeType m_type = MicrocodeType::NOP;
				std::string m_tStr;
			};

		}

	}

}