#pragma once

/**************************************************************************

Microcode.hpp

Created by Matthew Clarke

A set of microcode instructions to complete

 **************************************************************************/

#include "../../Type.hpp"

namespace icarus {

	namespace processor {

		namespace instruction {

			class Microcode {
				enum class MicrocodeType {
					NONE = 0,

					SRC_REG,
					SRC_MEM,
					SRC_MEM_BY_REG,
					SRC_IMM,

					DST_REG,
					DST_MEM,
					DST_MEM_BY_REG,

					FN_ADD,
				};

				enum class MicrocodeSrcSlot {
					SRC_A = 0,
					SRC_B,
					SRC_C,
					SRC_D
				};

				Microcode(MicrocodeType type, MicrocodeSrcSlot srcSlot = MicrocodeSrcSlot::SRC_A);

				MicrocodeType getType();
				MicrocodeSrcSlot getSrcSlot();
				uint8_t getSrcByteSize();
				uint8_t getSrc();
				uint8_t getDstByteSize();
				uint8_t getDst();

				void setSrcByteSize(uint8_t bs);
				void setSrc(uint8_t s);
				void setDstByteSize(uint8_t bs);
				void setDst(uint8_t d);

			private:
				MicrocodeType m_type = MicrocodeType::NONE;
				MicrocodeSrcSlot m_srcSlot = MicrocodeSrcSlot::SRC_A;

				uint8_t m_srcByteSize = 0;
				uint8_t m_src = 0;

				uint8_t m_dstByteSize = 0;
				uint8_t m_dst = 0;
			};

		}

	}

}