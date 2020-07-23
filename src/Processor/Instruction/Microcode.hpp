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
					NOP = 0,

					SRC_REGOP,
					SRC_MOD,
					SRC_RM,
					SRC_IMM,

					DST_REGOP,
					DST_MOD,
					DST_RM,

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