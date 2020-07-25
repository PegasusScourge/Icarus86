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

					REG_8, // Switches to registers being interpreted as the 8 bit registers
					REG_16, // Siwtches to registers being interpreted as the 16 bit registers

					SRC_REGOP, // A source is deduced from the REGOP
					SRC_MODRM,  // A source is deduced from the MODRM
					SRC_IMM, // A source is the immediate byte(s)

					DST_REGOP, // Destination is deduced from the REGOP
					DST_MODRM, // Destination is deduced from the MODRM

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

					FN_MOV,
					FN_XCHG,

					FN_REGOP_8X, // Opcode 81-83 regop function family as decided by the REGOP field of the ModRM byte. Decays to one of the FNs
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