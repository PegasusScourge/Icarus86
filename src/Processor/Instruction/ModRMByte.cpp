/**************************************************************************

ModRMByte.cpp

Created by Matthew Clarke

ModRMByte handling

 **************************************************************************/

#include "ModRMByte.hpp"

#include "../../COutSys.hpp"

namespace ipi = icarus::processor::instruction;

ipi::ModRMByte::ModRMByte() {
	setByte(0);
}

ipi::ModRMByte::ModRMByte(uint8_t b) {
	setByte(b);
}

void ipi::ModRMByte::setByte(uint8_t b) {
	m_byte = b;
	m_MOD = (m_byte >> 6) & 0x3;
	m_REGOP = (m_byte >> 3) & 0x7;
	m_RM = (m_byte) & 0x7;
}

uint8_t ipi::ModRMByte::byte() {
	return m_byte;
}

uint8_t ipi::ModRMByte::MOD() {
	return m_MOD;
}

uint8_t ipi::ModRMByte::REGOP() {
	return m_REGOP;
}

uint8_t ipi::ModRMByte::RM() {
	return m_RM;
}
