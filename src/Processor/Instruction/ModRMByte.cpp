/**************************************************************************

ModRMByte.cpp

Created by Matthew Clarke

ModRMByte handling

 **************************************************************************/

#include "ModRMByte.hpp"

namespace ipi = icarus::processor::instruction;

ipi::ModRMByte ipi::DecodeModRMByte(uint8_t byte) {
	ModRMByte b;
	b.w = byte;
	return b;
}