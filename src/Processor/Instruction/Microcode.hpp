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
				};

				static MicrocodeType GetTypeFromString(std::string s);

				Microcode(std::string tStr, MicrocodeType type);

				MicrocodeType getType();
				std::string getTStr();
				uint8_t getSrcByteSize();
				uint8_t getSrc();
				uint8_t getDstByteSize();
				uint8_t getDst();

				void setSrcByteSize(uint8_t bs);
				void setSrc(uint8_t s);
				void setDstByteSize(uint8_t bs);
				void setDst(uint8_t d);

			private:
				MicrocodeType m_type = MicrocodeType::NOP;
				std::string m_tStr;

				uint8_t m_srcByteSize = 0;
				uint8_t m_src = 0;

				uint8_t m_dstByteSize = 0;
				uint8_t m_dst = 0;
			};

		}

	}

}