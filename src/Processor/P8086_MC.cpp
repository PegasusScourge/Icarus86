/**************************************************************************

P8086_MC.cpp

Created by Matthew Clarke

Microcode execution functions definitions: containts JMP, MISC and the mcode_execCode function

 **************************************************************************/

#include "P8086.hpp"

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

	case Microcode::MicrocodeType::RMB_8: // Switch to interpreting registers as the 8 bit registers
		m_cInstr.mCodeI.bitMode8Bit = true;
		break;

	case Microcode::MicrocodeType::RMB_16: // Switch to interpreting registers as the 16 bit registers
		m_cInstr.mCodeI.bitMode8Bit = false;
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

	case Microcode::MicrocodeType::SRC_STACK_POP: 
	{
		auto& src = mcode_getNextSrc();
		src.bytes = m_cInstr.mCodeI.bitMode8Bit ? 1 : 2;
		mcode_stackPop(src);
	}
		break;

	case Microcode::MicrocodeType::SRC_IMM:
		mcode_getSrcImm();
		break;

	case Microcode::MicrocodeType::SRC_R_AL:
	{
		auto& src = mcode_getNextSrc();
		src.v = m_registers[REGISTERS::R_AX].readLower();
		src.bytes = 1;
	}
	break;

	case Microcode::MicrocodeType::SRC_R_AX:
	{
		auto& src = mcode_getNextSrc();
		src.v = m_registers[REGISTERS::R_AX].read();
		src.bytes = 2;
	}
	break;

	case Microcode::MicrocodeType::SRC_R_BX:
	{
		auto& src = mcode_getNextSrc();
		src.v = m_registers[REGISTERS::R_BX].read();
		src.bytes = 2;
	}
		break;

	case Microcode::MicrocodeType::SRC_R_CX:
	{
		auto& src = mcode_getNextSrc();
		src.v = m_registers[REGISTERS::R_CX].read();
		src.bytes = 2;
	}
		break;

	case Microcode::MicrocodeType::SRC_R_DX:
	{
		auto& src = mcode_getNextSrc();
		src.v = m_registers[REGISTERS::R_DX].read();
		src.bytes = 2;
	}
	break;

	case Microcode::MicrocodeType::SRC_R_DI:
	{
		auto& src = mcode_getNextSrc();
		src.v = m_registers[REGISTERS::R_DI].read();
		src.bytes = 2;
	}
		break;

		/*
		DST
		*/

	case Microcode::MicrocodeType::DST_MODRM:
		mcode_dstModRM();
		break;

	case Microcode::MicrocodeType::DST_REGOP:
		mcode_toDstFromReg(m_cInstr.modRMByte.REGOP());
		break;

	case Microcode::MicrocodeType::DST_STACK_PUSH:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			mcode_stackPush(m_cInstr.mCodeI.dst);
		break;

	case Microcode::MicrocodeType::DST_R_AL:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_AX].putLower(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_BL:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_BX].putLower(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_CL:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_CX].putLower(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_AX:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_AX].put(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_BX:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_BX].put(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_CX:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_CX].put(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_DX:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_DX].put(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_SP:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_SP].put(m_cInstr.mCodeI.dst.v);
		break;

	case Microcode::MicrocodeType::DST_R_DI:
		if (!m_cInstr.mCodeI.dstEnabled)
			MCODE_DEBUG("!dstEnabled: not writing to destination");
		else
			m_registers[REGISTERS::R_DI].put(m_cInstr.mCodeI.dst.v);
		break;

		/*
		FN
		*/

	case Microcode::MicrocodeType::FN_CALL_REL:
		mcode_fnCallRel();
		break;

	case Microcode::MicrocodeType::FN_RETN_NEAR:
		mcode_fnRetnNear();
		break;

	case Microcode::MicrocodeType::FN_JMP: // Hijack the conditional jump and force
		mcode_jmpCondRelativeShort(true);
		break;

	case Microcode::MicrocodeType::FN_JZ:
		mcode_jmpCondRelativeShort(m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_ZF));
		break;

	case Microcode::MicrocodeType::FN_JNZ:
		mcode_jmpCondRelativeShort(!m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_ZF));
		break;

	case Microcode::MicrocodeType::FN_JC:
		mcode_jmpCondRelativeShort(m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_CF));
		break;

	case Microcode::MicrocodeType::FN_JNC:
		mcode_jmpCondRelativeShort(!m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_CF));
		break;

	case Microcode::MicrocodeType::FN_JBE:
		mcode_jmpCondRelativeShort(m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_CF) || m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_ZF));
		break;

	case Microcode::MicrocodeType::FN_JNBE:
		mcode_jmpCondRelativeShort(!m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_CF) && !m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_ZF));
		break;

	case Microcode::MicrocodeType::FN_JS:
		mcode_jmpCondRelativeShort(m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_SF));
		break;

	case Microcode::MicrocodeType::FN_JNS:
		mcode_jmpCondRelativeShort(!m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_SF));
		break;

	case Microcode::MicrocodeType::FN_REGOP_8X:
		mcode_fnRegop8X();
		break;

	case Microcode::MicrocodeType::FN_REGOP_FE:
		mcode_fnRegopFE();
		break;
		
	case Microcode::MicrocodeType::FN_ADD:
		mcode_fnAdd(false);
		break;

	case Microcode::MicrocodeType::FN_ADC:
		mcode_fnAdd(true);
		break;

	case Microcode::MicrocodeType::FN_SUB:
		mcode_fnSub(false);
		break;

	case Microcode::MicrocodeType::FN_SBB:
		mcode_fnSub(true);
		break;

	case Microcode::MicrocodeType::FN_CMP:
		mcode_fnCmp();
		break;

	case Microcode::MicrocodeType::FN_INC:
		mcode_fnInc();
		break;

	case Microcode::MicrocodeType::FN_DEC:
		mcode_fnDec();
		break;

	case Microcode::MicrocodeType::FN_XOR:
		mcode_fnXOR();
		break;

	case Microcode::MicrocodeType::FN_OR:
		mcode_fnOR();
		break;

	case Microcode::MicrocodeType::FN_AND:
		mcode_fnAND();
		break;

	case Microcode::MicrocodeType::FN_CLC:
		m_registers[REGISTERS::R_FLAGS].clearBit(FLAGS_CF);
		break;

	case Microcode::MicrocodeType::FN_STC:
		m_registers[REGISTERS::R_FLAGS].setBit(FLAGS_CF);
		break;

	case Microcode::MicrocodeType::FN_APASS:
		m_cInstr.mCodeI.dst = m_cInstr.mCodeI.srcA; m_cInstr.mCodeI.dstEnabled = true;
		break;

	case Microcode::MicrocodeType::FN_BPASS:
		m_cInstr.mCodeI.dst = m_cInstr.mCodeI.srcB; m_cInstr.mCodeI.dstEnabled = true;
		break;

		/*
		MISC
		*/

	case Microcode::MicrocodeType::SE_SRC_B:
		mcode_seSrcB();
		break;

	case Microcode::MicrocodeType::HLT:
		MCODE_DEBUG("HALT");
		setHLT(true);
		break;

	case Microcode::MicrocodeType::NOP:
		MCODE_DEBUG("NOP");
		break;
	default:
		MCODE_DEBUG("UDEF");
		icarus::COutSys::Println("Undefined Microcode type detected, type=" + std::to_string((uint8_t)mcode.getType()), icarus::COutSys::LEVEL_ERR);
		triggerError();
		break;

	}

	MCODE_DEBUG("    --- <mcode state> ---");
	MCODE_DEBUG("    srcA.bytes  = " + std::to_string(m_cInstr.mCodeI.srcA.bytes) + " srcA.v = " + icarus::COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v));
	MCODE_DEBUG("    srcB.bytes  = " + std::to_string(m_cInstr.mCodeI.srcB.bytes) + " srcB.v = " + icarus::COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("    dst.bytes   = " + std::to_string(m_cInstr.mCodeI.dst.bytes) + " dst.v = " + icarus::COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));
	MCODE_DEBUG("    dst.enabled = " + std::to_string(m_cInstr.mCodeI.dstEnabled));
	MCODE_DEBUG("    -- </mcode state> ---");
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

/***********************************/
// MISC MICROCODE
/***********************************/

void Processor_8086::mcode_seSrcB() {
	MCODE_DEBUG("SE_SRC_B BEGIN: " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	if (m_cInstr.mCodeI.srcA.bytes == 2 && m_cInstr.mCodeI.srcB.bytes == 1) {
		m_cInstr.mCodeI.srcB.bytes = 2;
		// Sign extend to 2 bytes.
		if (m_cInstr.mCodeI.srcB.v & 0x80) {
			// Sign bit is set:
			m_cInstr.mCodeI.srcB.v |= 0xFF00; // Set the top byte to be all 1s
		}
		else {
			// Sign bit is not set, do nothing
		}
		MCODE_DEBUG("SE_SRC_B RESULT: " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	}
}

void Processor_8086::mcode_stackPush(CurrentInstruction::MicrocodeInformation::Cache& src) {
	MCODE_DEBUG("Stack push of v = " + COutSys::ToHexStr(src.v) + " (bytes = " + std::to_string(src.bytes) + "), SP = " + COutSys::ToHexStr(m_registers[REGISTERS::R_SP].read()));

	// Decrement stack pointer
	m_registers[REGISTERS::R_SP].put(m_registers[REGISTERS::R_SP].read() - src.bytes);
	// Put the address bus to the value of SP
	m_addressBus.putData(m_registers[REGISTERS::R_SP].read());
	// Put the data onto the bus
	m_dataBus.putData(src.v);
	// If we have > 1 bytes, do writeBus, else do writeByte
	if (src.bytes != 1) {
		m_mmu.writeBus(m_dataBus, m_addressBus, icarus::memory::MMU::ReadType::LittleEndian);
	}
	else {
		m_mmu.writeByte(m_dataBus, m_addressBus);
	}
	MCODE_DEBUG("Push complete. SP = " + COutSys::ToHexStr(m_registers[REGISTERS::R_SP].read()));
}

void Processor_8086::mcode_stackPop(CurrentInstruction::MicrocodeInformation::Cache& src) {
	MCODE_DEBUG("Stack pop, SP = " + COutSys::ToHexStr(m_registers[REGISTERS::R_SP].read()));

	// Put the address bus to the value of SP
	m_addressBus.putData(m_registers[REGISTERS::R_SP].read());

	// If we have > 1 bytes, do fillBus, else do readByte
	if (src.bytes != 1) {
		m_mmu.fillBus(m_dataBus, m_addressBus, icarus::memory::MMU::ReadType::LittleEndian);
	}
	else {
		m_mmu.readByte(m_dataBus, m_addressBus);
	}

	src.v = m_dataBus.readData();
	// Increment stack pointer
	m_registers[REGISTERS::R_SP].put(m_registers[REGISTERS::R_SP].read() + src.bytes);

	MCODE_DEBUG("Pop complete. SP = " + COutSys::ToHexStr(m_registers[REGISTERS::R_SP].read()) + ", v = " + COutSys::ToHexStr(m_dataBus.readData()));
}