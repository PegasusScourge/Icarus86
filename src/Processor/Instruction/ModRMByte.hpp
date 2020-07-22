#pragma once

/**************************************************************************

ModRMByte.hpp

Created by Matthew Clarke

ModRMByte handling

 **************************************************************************/

#include "../../Type.hpp"

namespace icarus {

	namespace processor {

		namespace instruction {

			class ModRMByte {
			public:
				ModRMByte(uint8_t b);

				union {
					union {
						uint8_t MOD : 2;
						uint8_t REGOP : 3;
						uint8_t RM : 3;
					};
					uint8_t byte;
				};
			};

		}

	}

}