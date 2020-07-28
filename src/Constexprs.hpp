#pragma once

/**************************************************************************
Constexprs.hpp

Created by Matthew Clarke

Universal constexpr definitions that are useful throughout the program

 **************************************************************************/

// Prints the microcode debug information
#define MCODE_DEBUG_PRINT
// Prints the execution debug information
#define EXEC8086_DEBUG_PRINT
// Prints the decode debug information
#define DECODE8086_DEBUG_PRINT
// Prints the MMU debug information
#define MMU_DEBUG_PRINT

namespace icarus {

	constexpr char endl = '\n';

	namespace processor {

		namespace instruction {

			const std::string InstructionPath = "instructionsets/";

		}

	}

}
