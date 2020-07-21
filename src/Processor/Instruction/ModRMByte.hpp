#pragma once

/**************************************************************************

ModRMByte.hpp

Created by Matthew Clarke

ModRMByte handling

 **************************************************************************/

#include "../../Type.hpp"

#include <vector>

namespace icarus {

	namespace processor {

		namespace instruction {

			typedef union ModRMByte {
				union {
					uint8_t	MOD : 2;
					uint8_t REGOP : 3;
					uint8_t RM : 3;
				};
				uint8_t w;
			} ModRMByte;

			ModRMByte DecodeModRMByte(uint8_t byte);

		}

	}

}