/**************************************************************************

Processor_8086_MCODEEXEC.cpp

Created by Matthew Clarke

Microcode execution functions definitions

 **************************************************************************/

#include "Processor_8086.hpp"

#include "../COutSys.hpp"
#include "../Constexprs.hpp"

using namespace icarus::processor::instruction;
using namespace icarus::processor;

#ifdef MCODE_DEBUG_PRINT

#define MCODE_DEBUG(s) icarus::COutSys::Println(std::string(__FUNCTION__) +  ":\t" + s, icarus::COutSys::LEVEL_INFO)
#define MCODE_DEBUG_ERR(s) icarus::COutSys::Println(std::string(__FUNCTION__) +  ":\t" + s, icarus::COutSys::LEVEL_ERR)

#else

#define MCODE_DEBUG(s)
#define MCODE_DEBUG_ERR(s)

#endif

void Processor_8086::mcode_toSrcFromReg(Processor_8086::CurrentInstruction::MicrocodeInformation::Source& src, uint8_t sval) {
	MCODE_DEBUG("SVAL = " + std::to_string(sval));
	
	if (m_cInstr.mCodeI.regMode8Bit) {
		src.bytes = 1;
		switch (sval) {
		case 0: // Get register AL
			MCODE_DEBUG("SRC = REGISTER_AL");
			src.v = m_registers[(uint8_t)REGISTERS::R_AX].readLower();
			break;
		case 1: // Get register CL
			MCODE_DEBUG("SRC = REGISTER_CL");
			src.v = m_registers[(uint8_t)REGISTERS::R_CX].readLower();
			break;
		case 2: // Get register DL
			MCODE_DEBUG("SRC = REGISTER_DL");
			src.v = m_registers[(uint8_t)REGISTERS::R_DX].readLower();
			break;
		case 3: // Get register BL
			MCODE_DEBUG("SRC = REGISTER_BL");
			src.v = m_registers[(uint8_t)REGISTERS::R_BX].readLower();
			break;
		case 4: // Get register AH
			MCODE_DEBUG("SRC = REGISTER_AH");
			src.v = m_registers[(uint8_t)REGISTERS::R_AX].readUpper();
			break;
		case 5: // Get register CH
			MCODE_DEBUG("SRC = REGISTER_CH");
			src.v = m_registers[(uint8_t)REGISTERS::R_CX].readUpper();
			break;
		case 6: // Get register DH
			MCODE_DEBUG("SRC = REGISTER_DH");
			src.v = m_registers[(uint8_t)REGISTERS::R_DX].readUpper();
			break;
		case 7: // Get register BH
			MCODE_DEBUG("SRC = REGISTER_BH");
			src.v = m_registers[(uint8_t)REGISTERS::R_BX].readUpper();
			break;
		default:
			// ERROR
			MCODE_DEBUG_ERR("SRC = REGISTER_ERROR");
			break;
		}
	}
	else {
		src.bytes = 2;
		REGISTERS reg = REGISTERS::R_AX;
		switch (sval) {
		case 0: // Get register AX
			MCODE_DEBUG("SRC = REGISTER_AX");
			reg = REGISTERS::R_AX;
			break;
		case 1: // Get register CX
			MCODE_DEBUG("SRC = REGISTER_CX");
			reg = REGISTERS::R_CX;
			break;
		case 2: // Get register DX
			MCODE_DEBUG("SRC = REGISTER_DX");
			reg = REGISTERS::R_DX;
			break;
		case 3: // Get register BX
			MCODE_DEBUG("SRC = REGISTER_BX");
			reg = REGISTERS::R_BX;
			break;
		case 4: // Get register SP
			MCODE_DEBUG("SRC = REGISTER_SP");
			reg = REGISTERS::R_SP;
			break;
		case 5: // Get register BP
			MCODE_DEBUG("SRC = REGISTER_BP");
			reg = REGISTERS::R_BP;
			break;
		case 6: // Get register SI
			MCODE_DEBUG("SRC = REGISTER_SI");
			reg = REGISTERS::R_SI;
			break;
		case 7: // Get register DI
			MCODE_DEBUG("SRC = REGISTER_DI");
			reg = REGISTERS::R_DI;
			break;
		default:
			// ERROR
			MCODE_DEBUG_ERR("SRC = REGISTER_ERROR");
			break;
		}
		// Read into v
		src.v = m_registers[(uint8_t)reg].read();
	}
}

void Processor_8086::mcode_execCode(Microcode mcode) {
	MCODE_DEBUG("MicrocodeType " + mcode.getTStr() + " exec");
	
	switch (mcode.getType()) {
	case Microcode::MicrocodeType::REG_8: // Switch to interpreting registers as the 8 bit registers
		m_cInstr.mCodeI.regMode8Bit = true;
		break;

	case Microcode::MicrocodeType::REG_16: // Switch to interpreting registers as the 16 bit registers
		m_cInstr.mCodeI.regMode8Bit = false;
		break;

	case Microcode::MicrocodeType::SRC_REGOP:
		mcode_getSrcRegop();
		break;

	case Microcode::MicrocodeType::SRC_MODRM:
		mcode_getSrcModRM();
		break;

	case Microcode::MicrocodeType::SRC_IMM:
		mcode_getSrcImm();
		break;

	default:
		MCODE_DEBUG("NOP || UDEF");
		break;

	}
}

void Processor_8086::mcode_getSrcImm() {
	// We get the source from the immediate value.
	// We simply copy
	auto& src = (m_cInstr.mCodeI.srcAUsed ? m_cInstr.mCodeI.srcB : m_cInstr.mCodeI.srcA);
	MCODE_DEBUG("SrcSelect, srcAUsed=" + std::to_string(m_cInstr.mCodeI.srcAUsed));
	if (!m_cInstr.mCodeI.srcAUsed)
		m_cInstr.mCodeI.srcAUsed = true;

	src.bytes = 2;
	src.v = m_cInstr.immediate;
}

void Processor_8086::mcode_getSrcModRM() {
	// We get the source from the ModRM values.
	// This could be a memory address (mod 00 - 10 inclusive) or a register (mod 11)(16 bit or 8 bit register, see m_cInstr.mCodeI.regMode8Bit)
	// If srcA is in use, we put in srcB
	auto& src = (m_cInstr.mCodeI.srcAUsed ? m_cInstr.mCodeI.srcB : m_cInstr.mCodeI.srcA);
	MCODE_DEBUG("SrcSelect, srcAUsed=" + std::to_string(m_cInstr.mCodeI.srcAUsed));
	if (!m_cInstr.mCodeI.srcAUsed)
		m_cInstr.mCodeI.srcAUsed = true;

	MCODE_DEBUG("MOD=" + std::to_string(m_cInstr.modRMByte.MOD()));

	switch (m_cInstr.modRMByte.MOD()) {
	case 0b00: // Memory addressing

		break;
	case 0b01: // Memory addressing + displacement8

		break;
	case 0b10: // Memory addressing + displacement16

		break;
	case 0b11: // Register addressing
		mcode_toSrcFromReg(src, m_cInstr.modRMByte.RM());
		break;
	default:
		// ERROR
		MCODE_DEBUG_ERR("SRC = MOD_ERROR");
		break;
	}
}

void Processor_8086::mcode_getSrcRegop() {
	// We get the source from the regop value. This will be a 16 bit or 8 bit register. m_cInstr.mCodeI.regMode8Bit declares it as 8 bit register
	// If srcA is in use, we put in srcB
	auto& src = (m_cInstr.mCodeI.srcAUsed ? m_cInstr.mCodeI.srcB : m_cInstr.mCodeI.srcA);
	MCODE_DEBUG("SrcSelect, srcAUsed=" + std::to_string(m_cInstr.mCodeI.srcAUsed));
	if (!m_cInstr.mCodeI.srcAUsed)
		m_cInstr.mCodeI.srcAUsed = true;

	mcode_toSrcFromReg(src, m_cInstr.modRMByte.REGOP());
}