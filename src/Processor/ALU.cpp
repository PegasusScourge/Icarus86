/**************************************************************************

ALU.cpp

Created by Matthew Clarke

Arithmetic and Logic Unit implementation

 **************************************************************************/

#include "ALU.hpp"

#include "../COutSys.hpp"

#include <string>


namespace ip = icarus::processor;

void ip::ALU::setCarryBit(bool c) {
	m_carryBit = c;
}

bool ip::ALU::carryBit() {
	return m_carryBit;
}
bool ip::ALU::zeroFlag() {
	return m_zeroFlag;
}
bool ip::ALU::negativeFlag() {
	return m_negativeFlag;
}
bool ip::ALU::overflowFlag() {
	return m_overflowFlag;
}
bool ip::ALU::parityFlag() {
	return m_parityFlag;
}

bool ip::ALU::test() {
	icarus::COutSys::Println("ALU TEST ROUTINE: START", icarus::COutSys::LEVEL_INFO);

	bool failed = false;
	uint8_t aluTest;

	aluTest = add<uint8_t>(255, 1);
	if (aluTest != 0 || !carryBit()) {
		icarus::COutSys::Println("ALU failed test 1: " + std::to_string(aluTest) + "," + std::to_string(carryBit()), icarus::COutSys::LEVEL_ERR);
		failed = true;
	}
	else {
		icarus::COutSys::Println("ALU passed test 1", icarus::COutSys::LEVEL_INFO);
	}
	
	aluTest = add<uint8_t>(255, 1, true);
	if (aluTest != 1 || !carryBit()) {
		icarus::COutSys::Println("ALU failed test 2: " + std::to_string(aluTest) + "," + std::to_string(carryBit()), icarus::COutSys::LEVEL_ERR);
		failed = true;
	}
	else {
		icarus::COutSys::Println("ALU passed test 2", icarus::COutSys::LEVEL_INFO);
	}

	aluTest = add<uint8_t>(127, 127, false);
	if (aluTest != 254 || !overflowFlag()) {
		icarus::COutSys::Println("ALU failed test 3: " + std::to_string(aluTest) + "," + std::to_string(overflowFlag()), icarus::COutSys::LEVEL_ERR);
		failed = true;
	}
	else {
		icarus::COutSys::Println("ALU passed test 3", icarus::COutSys::LEVEL_INFO);
	}
		
	aluTest = subtract<uint8_t>(127, 127, false);
	if (aluTest != 0 || !zeroFlag()) {
		icarus::COutSys::Println("ALU failed test 4: " + std::to_string(aluTest) + "," + std::to_string(zeroFlag()), icarus::COutSys::LEVEL_ERR);
		failed = true;
	}
	else {
		icarus::COutSys::Println("ALU passed test 4", icarus::COutSys::LEVEL_INFO);
	}
		
	aluTest = rotateLeft<uint8_t>(2);
	if (aluTest != 4) {
		icarus::COutSys::Println("ALU failed test 5: " + std::to_string(aluTest), icarus::COutSys::LEVEL_ERR);
		failed = true;
	}
	else {
		icarus::COutSys::Println("ALU passed test 5", icarus::COutSys::LEVEL_INFO);
	}
		
	aluTest = rotateRight<uint8_t>(2);
	if (aluTest != 1) {
		icarus::COutSys::Println("ALU failed test 6: " + std::to_string(aluTest), icarus::COutSys::LEVEL_ERR);
		failed = true;
	}
	else {
		icarus::COutSys::Println("ALU passed test 6", icarus::COutSys::LEVEL_INFO);
	}
	
	std::string res = (failed ? "[FAIL]" : "[PASS]");
	icarus::COutSys::Println("ALU TEST ROUTINE: END. RESULT: " + res, icarus::COutSys::LEVEL_INFO);

	return !failed;
}
