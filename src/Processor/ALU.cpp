/**************************************************************************

ALU.cpp

Created by Matthew Clarke

Arithmetic and Logic Unit implementation

 **************************************************************************/

#include "ALU.hpp"

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
