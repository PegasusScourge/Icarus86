/*
    **> \File           Microcode.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-11
    **> \Brief          Microcode
    **> \Details        None
    **> \Notes          None
*/


#include "Intel8086/Instruction/Microcode.hpp"

using namespace i86::intel86::instruction;

 /***********************************/
 // class Microcode
 /***********************************/

Microcode::MicrocodeType Microcode::GetTypeFromString(std::string s) {
	if (s.compare("NOP") == 0) { return Microcode::MicrocodeType::NOP; }
	else if (s.compare("HLT") == 0) { return Microcode::MicrocodeType::HLT; }

	else if (s.compare("RMB_8") == 0) { return Microcode::MicrocodeType::RMB_8; }
	else if (s.compare("RMB_16") == 0) { return Microcode::MicrocodeType::RMB_16; }

	else if (s.compare("SE_SRC_B") == 0) { return Microcode::MicrocodeType::SE_SRC_B; }

	else if (s.compare("SEG_OVERRIDE_ES") == 0) { return Microcode::MicrocodeType::SEG_OVERRIDE_ES; }

	else if (s.compare("SRC_REGOP") == 0) { return Microcode::MicrocodeType::SRC_REGOP; }
	else if (s.compare("SRC_MODRM") == 0) { return Microcode::MicrocodeType::SRC_MODRM; }
	else if (s.compare("SRC_IMM") == 0) { return Microcode::MicrocodeType::SRC_IMM; }
	else if (s.compare("SRC_DISP") == 0) { return Microcode::MicrocodeType::SRC_DISP; }

	else if (s.compare("SRC_STACK_POP") == 0) { return Microcode::MicrocodeType::SRC_STACK_POP; }

	else if (s.compare("SRC_R_AL") == 0) { return Microcode::MicrocodeType::SRC_R_AL; }
	else if (s.compare("SRC_R_AX") == 0) { return Microcode::MicrocodeType::SRC_R_AX; }
	else if (s.compare("SRC_R_BX") == 0) { return Microcode::MicrocodeType::SRC_R_BX; }
	else if (s.compare("SRC_R_CX") == 0) { return Microcode::MicrocodeType::SRC_R_CX; }
	else if (s.compare("SRC_R_DX") == 0) { return Microcode::MicrocodeType::SRC_R_DX; }
	else if (s.compare("SRC_R_BP") == 0) { return Microcode::MicrocodeType::SRC_R_BP; }
	else if (s.compare("SRC_R_DI") == 0) { return Microcode::MicrocodeType::SRC_R_DI; }
	else if (s.compare("SRC_R_DS") == 0) { return Microcode::MicrocodeType::SRC_R_DS; }
	else if (s.compare("SRC_R_CS") == 0) { return Microcode::MicrocodeType::SRC_R_CS; }
	else if (s.compare("SRC_R_ES") == 0) { return Microcode::MicrocodeType::SRC_R_ES; }
	else if (s.compare("SRC_R_SS") == 0) { return Microcode::MicrocodeType::SRC_R_SS; }

	else if (s.compare("DST_REGOP") == 0) { return Microcode::MicrocodeType::DST_REGOP; }
	else if (s.compare("DST_REGOP_SREG") == 0) { return Microcode::MicrocodeType::DST_REGOP_SREG; }
	else if (s.compare("DST_MODRM") == 0) { return Microcode::MicrocodeType::DST_MODRM; }

	else if (s.compare("DST_STACK_PUSH") == 0) { return Microcode::MicrocodeType::DST_STACK_PUSH; }

	else if (s.compare("DST_R_AL") == 0) { return Microcode::MicrocodeType::DST_R_AL; }
	else if (s.compare("DST_R_BL") == 0) { return Microcode::MicrocodeType::DST_R_BL; }
	else if (s.compare("DST_R_CL") == 0) { return Microcode::MicrocodeType::DST_R_CL; }
	else if (s.compare("DST_R_AX") == 0) { return Microcode::MicrocodeType::DST_R_AX; }
	else if (s.compare("DST_R_BX") == 0) { return Microcode::MicrocodeType::DST_R_BX; }
	else if (s.compare("DST_R_CX") == 0) { return Microcode::MicrocodeType::DST_R_CX; }
	else if (s.compare("DST_R_DX") == 0) { return Microcode::MicrocodeType::DST_R_DX; }
	else if (s.compare("DST_R_SP") == 0) { return Microcode::MicrocodeType::DST_R_SP; }
	else if (s.compare("DST_R_BP") == 0) { return Microcode::MicrocodeType::DST_R_BP; }
	else if (s.compare("DST_R_DI") == 0) { return Microcode::MicrocodeType::DST_R_DI; }
	else if (s.compare("DST_R_DS") == 0) { return Microcode::MicrocodeType::DST_R_DS; }
	else if (s.compare("DST_R_CS") == 0) { return Microcode::MicrocodeType::DST_R_CS; }
	else if (s.compare("DST_R_ES") == 0) { return Microcode::MicrocodeType::DST_R_ES; }
	else if (s.compare("DST_R_SS") == 0) { return Microcode::MicrocodeType::DST_R_SS; }

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

	else if (s.compare("FN_CLC") == 0) { return Microcode::MicrocodeType::FN_CLC; }
	else if (s.compare("FN_STC") == 0) { return Microcode::MicrocodeType::FN_STC; }
	else if (s.compare("FN_CLI") == 0) { return Microcode::MicrocodeType::FN_CLI; }
	else if (s.compare("FN_STI") == 0) { return Microcode::MicrocodeType::FN_STI; }

	else if (s.compare("FN_MOV") == 0) { return Microcode::MicrocodeType::FN_MOV; }
	else if (s.compare("FN_XCHG") == 0) { return Microcode::MicrocodeType::FN_XCHG; }

	else if (s.compare("FN_CALL_REL") == 0) { return Microcode::MicrocodeType::FN_CALL_REL; }

	else if (s.compare("FN_RETN_NEAR") == 0) { return Microcode::MicrocodeType::FN_RETN_NEAR; }

	else if (s.compare("FN_INT") == 0) { return Microcode::MicrocodeType::FN_INT; }
	else if (s.compare("FN_IRET") == 0) { return Microcode::MicrocodeType::FN_IRET; }

	else if (s.compare("FN_JMP") == 0) { return Microcode::MicrocodeType::FN_JMP; }
	else if (s.compare("FN_JMPF") == 0) { return Microcode::MicrocodeType::FN_JMPF; }
	else if (s.compare("FN_JZ") == 0) { return Microcode::MicrocodeType::FN_JZ; }
	else if (s.compare("FN_JNZ") == 0) { return Microcode::MicrocodeType::FN_JNZ; }
	else if (s.compare("FN_JC") == 0) { return Microcode::MicrocodeType::FN_JC; }
	else if (s.compare("FN_JNC") == 0) { return Microcode::MicrocodeType::FN_JNC; }
	else if (s.compare("FN_JBE") == 0) { return Microcode::MicrocodeType::FN_JBE; }
	else if (s.compare("FN_JNBE") == 0) { return Microcode::MicrocodeType::FN_JNBE; }
	else if (s.compare("FN_JS") == 0) { return Microcode::MicrocodeType::FN_JS; }
	else if (s.compare("FN_JNS") == 0) { return Microcode::MicrocodeType::FN_JNS; }

	else if (s.compare("FN_APASS") == 0) { return Microcode::MicrocodeType::FN_APASS; }
	else if (s.compare("FN_BPASS") == 0) { return Microcode::MicrocodeType::FN_BPASS; }

	else if (s.compare("FN_REGOP_8X") == 0) { return Microcode::MicrocodeType::FN_REGOP_8X; }
	else if (s.compare("FN_REGOP_FE") == 0) { return Microcode::MicrocodeType::FN_REGOP_FE; }
	return Microcode::MicrocodeType::UDEF;
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
