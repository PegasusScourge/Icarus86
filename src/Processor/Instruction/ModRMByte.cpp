/**************************************************************************

ModRMByte.cpp

Created by Matthew Clarke

ModRMByte handling

 **************************************************************************/

#include "ModRMByte.hpp"

#include "../../COutSys.hpp"

namespace ipi = icarus::processor::instruction;

ipi::ModRMByte::ModRMByte() {
	byte = 0;
}

ipi::ModRMByte::ModRMByte(uint8_t b) {
	byte = b;

	if (RM != (b & 0x7)) {
		// ERROR
		icarus::COutSys::Print("ERROR: MOD/RM BYTE FAILURE: RM != b & 0x7!! ", icarus::COutSys::LEVEL_ERR);
		icarus::COutSys::Println("(MOD=" + icarus::COutSys::ToHexStr(MOD) + ", REGOP=" +
			icarus::COutSys::ToHexStr(REGOP) + ", RM=" + icarus::COutSys::ToHexStr(RM) + ", BYTE=" + icarus::COutSys::ToHexStr(byte) + ")");
	}
	else {
		// icarus::COutSys::Print("MOD/RM decode correct ", icarus::COutSys::LEVEL_INFO);
		// icarus::COutSys::Println("(MOD=" + icarus::COutSys::ToHexStr(MOD) + ", REGOP=" +
		// icarus::COutSys::ToHexStr(REGOP) + ", RM=" + icarus::COutSys::ToHexStr(RM) + ", BYTE=" + icarus::COutSys::ToHexStr(byte) + ")");
	}
}
