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

void Processor_8086::mcode_execCode(Microcode mcode) {
	MCODE_DEBUG("MicrocodeType " + mcode.getTStr() + " exec");

	switch (mcode.getType()) {
		/*
		REG
		*/

	case Microcode::MicrocodeType::REG_8: // Switch to interpreting registers as the 8 bit registers
		m_cInstr.mCodeI.regMode8Bit = true;
		break;

	case Microcode::MicrocodeType::REG_16: // Switch to interpreting registers as the 16 bit registers
		m_cInstr.mCodeI.regMode8Bit = false;
		break;

		/*
		SRC
		*/

	case Microcode::MicrocodeType::SRC_REGOP:
		mcode_getSrcRegop();
		break;

	case Microcode::MicrocodeType::SRC_MODRM:
		mcode_getSrcModRM();
		break;

	case Microcode::MicrocodeType::SRC_IMM:
		mcode_getSrcImm();
		break;

		/*
		DST
		*/

	case Microcode::MicrocodeType::DST_MODRM:
		mcode_dstModRM();
		break;

	case Microcode::MicrocodeType::DST_R_AL:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_AX].putLower(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_BX:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_BX].put(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_SP:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_SP].put(m_cInstr.mCodeI.dst.v);
		break;

		/*
		FN
		*/

	case Microcode::MicrocodeType::FN_JZ:
		mcode_jmpCondRelativeShort(m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_ZF));
		break;

	case Microcode::MicrocodeType::FN_REGOP_8X:
		mcode_fnRegop8X();
		break;
		
	case Microcode::MicrocodeType::FN_CMP:
		mcode_fnCmp();
		break;

	case Microcode::MicrocodeType::FN_APASS:
		m_cInstr.mCodeI.dst = m_cInstr.mCodeI.srcA; m_cInstr.mCodeI.dstEnabled = true;
		break;

		/*
		MISC
		*/

	case Microcode::MicrocodeType::NOP:
		MCODE_DEBUG("NOP");
		break;
	default:
		MCODE_DEBUG("UDEF");
		icarus::COutSys::Println("Undefined Microcode type detected, type=" + std::to_string((uint8_t)mcode.getType()), icarus::COutSys::LEVEL_ERR);
		triggerError();
		break;

	}
}

/***********************************/
// SRC MICROCODE
/***********************************/

void Processor_8086::mcode_toSrcFromReg(Processor_8086::CurrentInstruction::MicrocodeInformation::Values& src, uint8_t sval) {
	MCODE_DEBUG("SVAL = " + std::to_string(sval));
	
	if (m_cInstr.mCodeI.regMode8Bit) {
		src.bytes = 1;
		switch (sval) {
		case 0: // Get register AL
			MCODE_DEBUG("SRC = REGISTER_AL");
			src.v = m_registers[REGISTERS::R_AX].readLower();
			break;
		case 1: // Get register CL
			MCODE_DEBUG("SRC = REGISTER_CL");
			src.v = m_registers[REGISTERS::R_CX].readLower();
			break;
		case 2: // Get register DL
			MCODE_DEBUG("SRC = REGISTER_DL");
			src.v = m_registers[REGISTERS::R_DX].readLower();
			break;
		case 3: // Get register BL
			MCODE_DEBUG("SRC = REGISTER_BL");
			src.v = m_registers[REGISTERS::R_BX].readLower();
			break;
		case 4: // Get register AH
			MCODE_DEBUG("SRC = REGISTER_AH");
			src.v = m_registers[REGISTERS::R_AX].readUpper();
			break;
		case 5: // Get register CH
			MCODE_DEBUG("SRC = REGISTER_CH");
			src.v = m_registers[REGISTERS::R_CX].readUpper();
			break;
		case 6: // Get register DH
			MCODE_DEBUG("SRC = REGISTER_DH");
			src.v = m_registers[REGISTERS::R_DX].readUpper();
			break;
		case 7: // Get register BH
			MCODE_DEBUG("SRC = REGISTER_BH");
			src.v = m_registers[REGISTERS::R_BX].readUpper();
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
		src.v = m_registers[reg].read();
	}
}

void Processor_8086::mcode_getSrcImm() {
	// We get the source from the immediate value.
	// We simply copy
	auto& src = (m_cInstr.mCodeI.srcAUsed ? m_cInstr.mCodeI.srcB : m_cInstr.mCodeI.srcA);
	MCODE_DEBUG("SrcSelect, srcAUsed=" + std::to_string(m_cInstr.mCodeI.srcAUsed));
	if (!m_cInstr.mCodeI.srcAUsed)
		m_cInstr.mCodeI.srcAUsed = true;

	src.bytes = m_cInstr.numImmeditateBytes;
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
		MCODE_DEBUG("SRC = M");
		MCODE_DEBUG_ERR("Not implemented!");
		break;
	case 0b01: // Memory addressing + displacement8
		MCODE_DEBUG("SRC = M+disp8");
		MCODE_DEBUG_ERR("Not implemented!");
		break;
	case 0b10: // Memory addressing + displacement16
		MCODE_DEBUG("SRC = M+disp16");
		MCODE_DEBUG_ERR("Not implemented!");
		break;
	case 0b11: // Register addressing
		MCODE_DEBUG("SRC = REG (RM)");
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

/***********************************/
// DST MICROCODE
/***********************************/

void Processor_8086::mcode_toDstFromReg(uint8_t sval) {
	if (!m_cInstr.mCodeI.dstEnabled) {
		// Not allowed to write to dst
		MCODE_DEBUG("!dstEnabled: not writing to destination");
		return;
	}

	MCODE_DEBUG("SVAL = " + std::to_string(sval));

	if (m_cInstr.mCodeI.regMode8Bit) {
		switch (sval) {
		case 0: // Get register AL
			MCODE_DEBUG("DST = REGISTER_AL");
			m_registers[REGISTERS::R_AX].putLower(m_cInstr.mCodeI.dst.v);
			break;
		case 1: // Get register CL
			MCODE_DEBUG("DST = REGISTER_CL");
			m_registers[REGISTERS::R_CX].putLower(m_cInstr.mCodeI.dst.v);
			break;
		case 2: // Get register DL
			MCODE_DEBUG("DST = REGISTER_DL");
			m_registers[REGISTERS::R_DX].putLower(m_cInstr.mCodeI.dst.v);
			break;
		case 3: // Get register BL
			MCODE_DEBUG("DST = REGISTER_BL");
			m_registers[REGISTERS::R_BX].putLower(m_cInstr.mCodeI.dst.v);
			break;
		case 4: // Get register AH
			MCODE_DEBUG("DST = REGISTER_AH");
			m_registers[REGISTERS::R_AX].putLower(m_cInstr.mCodeI.dst.v);
			break;
		case 5: // Get register CH
			MCODE_DEBUG("DST = REGISTER_CH");
			m_registers[REGISTERS::R_CX].putLower(m_cInstr.mCodeI.dst.v);
			break;
		case 6: // Get register DH
			MCODE_DEBUG("DST = REGISTER_DH");
			m_registers[REGISTERS::R_DX].putLower(m_cInstr.mCodeI.dst.v);
			break;
		case 7: // Get register BH
			MCODE_DEBUG("DST = REGISTER_BH");
			m_registers[REGISTERS::R_BX].putLower(m_cInstr.mCodeI.dst.v);
			break;
		default:
			// ERROR
			MCODE_DEBUG_ERR("DST = REGISTER_ERROR");
			break;
		}
	}
	else {
		REGISTERS reg = REGISTERS::R_AX;
		switch (sval) {
		case 0: // Get register AX
			MCODE_DEBUG("DST = REGISTER_AX");
			reg = REGISTERS::R_AX;
			break;
		case 1: // Get register CX
			MCODE_DEBUG("DST = REGISTER_CX");
			reg = REGISTERS::R_CX;
			break;
		case 2: // Get register DX
			MCODE_DEBUG("DST = REGISTER_DX");
			reg = REGISTERS::R_DX;
			break;
		case 3: // Get register BX
			MCODE_DEBUG("DST = REGISTER_BX");
			reg = REGISTERS::R_BX;
			break;
		case 4: // Get register SP
			MCODE_DEBUG("DST = REGISTER_SP");
			reg = REGISTERS::R_SP;
			break;
		case 5: // Get register BP
			MCODE_DEBUG("DST = REGISTER_BP");
			reg = REGISTERS::R_BP;
			break;
		case 6: // Get register SI
			MCODE_DEBUG("DST = REGISTER_SI");
			reg = REGISTERS::R_SI;
			break;
		case 7: // Get register DI
			MCODE_DEBUG("DST = REGISTER_DI");
			reg = REGISTERS::R_DI;
			break;
		default:
			// ERROR
			MCODE_DEBUG_ERR("DST = REGISTER_ERROR");
			break;
		}
		// Put our dst.v
		m_registers[reg].put(m_cInstr.mCodeI.dst.v);
	}
}

void Processor_8086::mcode_dstModRM() {
	if (!m_cInstr.mCodeI.dstEnabled) {
		// Not allowed to write to dst
		MCODE_DEBUG("!dstEnabled: not writing to destination");
		return;
	}
	
	// We get the destination from the ModRM values.
	// This could be a memory address (mod 00 - 10 inclusive) or a register (mod 11)(16 bit or 8 bit register, see m_cInstr.mCodeI.regMode8Bit)

	MCODE_DEBUG("MOD=" + std::to_string(m_cInstr.modRMByte.MOD()));

	switch (m_cInstr.modRMByte.MOD()) {
	case 0b00: // Memory addressing
		MCODE_DEBUG("DST = M");
		MCODE_DEBUG_ERR("Not implemented!");
		break;
	case 0b01: // Memory addressing + displacement8
		MCODE_DEBUG("DST = M+disp8");
		MCODE_DEBUG_ERR("Not implemented!");
		break;
	case 0b10: // Memory addressing + displacement16
		MCODE_DEBUG("DST = M+disp16");
		MCODE_DEBUG_ERR("Not implemented!");
		break;
	case 0b11: // Register addressing
		MCODE_DEBUG("DST = REG (RM)");
		mcode_toDstFromReg(m_cInstr.modRMByte.RM());
		break;
	default:
		// ERROR
		MCODE_DEBUG_ERR("DST = MOD_ERROR");
		break;
	}
}

/***********************************/
// FN MICROCODE
/***********************************/

void Processor_8086::mcode_fnRegop8X() {
	// Depending on the REGOP value, we need to do a different FN execution
	// These are mathematical and will use the ALU. We assume that srcA and srcB are thus filled already with the relevant values
	// We decay this FN into the appropriate FN

	Microcode::MicrocodeType t = Microcode::MicrocodeType::FN_ADD;

	switch (m_cInstr.modRMByte.REGOP()) {
	case 0: // FN_ADD
		MCODE_DEBUG("REGOP FN SELECT = FN_ADD");
		t = Microcode::MicrocodeType::FN_ADD;
		break;
	case 1: // FN_OR
		MCODE_DEBUG("REGOP FN SELECT = FN_OR");
		t = Microcode::MicrocodeType::FN_OR;
		break;
	case 2: // FN_ADC
		MCODE_DEBUG("REGOP FN SELECT = FN_ADC");
		t = Microcode::MicrocodeType::FN_ADC;
		break;
	case 3: // FN_SBB
		MCODE_DEBUG("REGOP FN SELECT = FN_SBB");
		t = Microcode::MicrocodeType::FN_SBB;
		break;
	case 4: // FN_AND
		MCODE_DEBUG("REGOP FN SELECT = FN_AND");
		t = Microcode::MicrocodeType::FN_AND;
		break;
	case 5: // FN_SUB
		MCODE_DEBUG("REGOP FN SELECT = FN_SUB");
		t = Microcode::MicrocodeType::FN_SUB;
		break;
	case 6: // FN_XOR
		MCODE_DEBUG("REGOP FN SELECT = FN_XOR");
		t = Microcode::MicrocodeType::FN_XOR;
		break;
	case 7: // FN_CMP
		MCODE_DEBUG("REGOP FN SELECT = FN_CMP");
		t = Microcode::MicrocodeType::FN_CMP;
		break;
	default:
		// ERROR
		MCODE_DEBUG_ERR("REGOP FN SELECT = ERROR");
		break;
	}

	MCODE_DEBUG("Executing daughter FN");
	Microcode mcode("FN_REGOP_8X_DAUGHTER", t);
	mcode_execCode(mcode);
}

void Processor_8086::mcode_fnCmp() {
	if (m_cInstr.mCodeI.srcA.bytes > 1 || m_cInstr.mCodeI.srcB.bytes > 1)
		m_cInstr.mCodeI.dst.bytes = 2;
	else
		m_cInstr.mCodeI.dst.bytes = 1;

	// We don't generate an output here as we are doing a compare. Do a subtract but lose the value
	m_alu.subtract(m_cInstr.mCodeI.srcA.v, m_cInstr.mCodeI.srcB.v);
	m_cInstr.mCodeI.dstEnabled = false; // Don't allow DST_X microcodes to output

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_OF, m_alu.overflowFlag());
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	// R_F.putBit(FLAGS_AF, m_alu.adjustFlag()); TODO - Also Aux Carry flag
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
	R_F.putBit(FLAGS_CF, m_alu.carryBit());
}

/***********************************/
// JMP MICROCODE
/***********************************/

void Processor_8086::mcode_jmpCondRelativeShort(bool condition) {
	if (!condition) {
		MCODE_DEBUG("Condition == false, not jumping");
		return;
	}
	MCODE_DEBUG("Condition == true");
	if (m_cInstr.mCodeI.srcA.bytes == 1) {
		// Relative 8 jump
		int8_t rel8 = (int8_t)m_cInstr.mCodeI.srcA.v;
		// Execute jump by adding rel8 to IP
		MCODE_DEBUG("[REL8 ] Jumping to IP = " + COutSys::ToHexStr((m_registers[REGISTERS::R_IP].read() + rel8)));
		m_registers[REGISTERS::R_IP].put(m_registers[REGISTERS::R_IP].read() + rel8);
	}
	else if (m_cInstr.mCodeI.srcA.bytes == 2) {
		// Relative 16 jump
		int16_t rel16 = (int16_t)m_cInstr.mCodeI.srcA.v;
		// Execute jump by adding rel16 to IP
		MCODE_DEBUG("[REL16] Jumping to IP = " + COutSys::ToHexStr((m_registers[REGISTERS::R_IP].read() + rel16)));
		m_registers[REGISTERS::R_IP].put(m_registers[REGISTERS::R_IP].read() + rel16);
	}
	else {
		MCODE_DEBUG_ERR("srcA.bytes is of unknown value, unable to jump!");
	}
}