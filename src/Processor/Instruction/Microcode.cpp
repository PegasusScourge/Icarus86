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
	else if (s.compare("FN_ADC") == 0) { return Microcode::MicrocodeType::FN_ADC; }
	else if (s.compare("FN_SUB") == 0) { return Microcode::MicrocodeType::FN_SUB; }
	else if (s.compare("FN_SBB") == 0) { return Microcode::MicrocodeType::FN_SBB; }
	else if (s.compare("FN_OR") == 0) { return Microcode::MicrocodeType::FN_OR; }
	else if (s.compare("FN_XOR") == 0) { return Microcode::MicrocodeType::FN_XOR; }
	else if (s.compare("FN_AND") == 0) { return Microcode::MicrocodeType::FN_AND; }
	else if (s.compare("FN_CMP") == 0) { return Microcode::MicrocodeType::FN_CMP; }
	else if (s.compare("FN_INC") == 0) { return Microcode::MicrocodeType::FN_INC; }
	else if (s.compare("FN_DEC") == 0) { return Microcode::MicrocodeType::FN_DEC; }

	else if (s.compare("FN_MOV") == 0) { return Microcode::MicrocodeType::FN_MOV; }
	else if (s.compare("FN_XCHG") == 0) { return Microcode::MicrocodeType::FN_XCHG; }
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
