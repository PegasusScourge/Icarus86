/**************************************************************************

Microcode.cpp

Created by Matthew Clarke

A set of microcode instructions to complete

 **************************************************************************/

#include "Microcode.hpp"

using namespace icarus::processor::instruction;

 /***********************************/
 // class Microcode
 /***********************************/

Microcode::MicrocodeType Microcode::GetTypeFromString(std::string s) {
	if (s.compare("NOP") == 0) { return Microcode::MicrocodeType::NOP; }

	else if (s.compare("SRC_REGOP") == 0) { return Microcode::MicrocodeType::SRC_REGOP; }
	else if (s.compare("SRC_MOD") == 0) { return Microcode::MicrocodeType::SRC_MOD; }
	else if (s.compare("SRC_RM") == 0) { return Microcode::MicrocodeType::SRC_RM; }
	else if (s.compare("SRC_IMM") == 0) { return Microcode::MicrocodeType::SRC_IMM; }

	else if (s.compare("DST_REGOP") == 0) { return Microcode::MicrocodeType::DST_REGOP; }
	else if (s.compare("DST_MOD") == 0) { return Microcode::MicrocodeType::DST_MOD; }
	else if (s.compare("DST_RM") == 0) { return Microcode::MicrocodeType::DST_RM; }

	else if (s.compare("FN_ADD") == 0) { return Microcode::MicrocodeType::FN_ADD; }
	return Microcode::MicrocodeType::NOP;
}

Microcode::Microcode(std::string tStr, MicrocodeType type) {
	m_type = type;
	m_tStr = tStr;
}

Microcode::MicrocodeType Microcode::getType() {
	return m_type;
}

std::string Microcode::getTStr() {
	return m_tStr;
}

uint8_t Microcode::getSrcByteSize() {
	return m_srcByteSize;
}

uint8_t Microcode::getSrc() {
	return m_src;
}

uint8_t Microcode::getDstByteSize() {
	return m_dstByteSize;
}

uint8_t Microcode::getDst() {
	return m_dst;
}

void Microcode::setSrcByteSize(uint8_t bs) {
	m_srcByteSize = bs;
}

void Microcode::setSrc(uint8_t s) {
	m_src = s;
}

void Microcode::setDstByteSize(uint8_t bs) {
	m_dstByteSize = bs;
}

void Microcode::setDst(uint8_t d) {
	m_dst = d;
}