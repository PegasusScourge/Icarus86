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

					SRC_REG,
					SRC_MEM,
					SRC_MEM_BY_REG,
					SRC_IMM,

					DST_REG,
					DST_MEM,
					DST_MEM_BY_REG,

					FN_ADD,
				};

				static MicrocodeType GetTypeFromString(std::string s);

				Microcode(MicrocodeType type);

				MicrocodeType getType();
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

				uint8_t m_srcByteSize = 0;
				uint8_t m_src = 0;

				uint8_t m_dstByteSize = 0;
				uint8_t m_dst = 0;
			};

		}

	}

}