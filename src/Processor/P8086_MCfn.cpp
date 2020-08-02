/**************************************************************************

P8086_MCfn.cpp

Created by Matthew Clarke

Microcode execution functions definitions: microcode FN instructions in this file

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
		triggerError();
		break;
	}

	MCODE_DEBUG("Executing daughter FN");
	Microcode mcode("FN_REGOP_8X_DAUGHTER", t);
	mcode_execCode(mcode);
}

void Processor_8086::mcode_fnRegopFE() {
	// Depending on the REGOP value, we need to do a different FN execution

	Microcode::MicrocodeType t = Microcode::MicrocodeType::FN_INC;

	switch (m_cInstr.modRMByte.REGOP()) {
	case 0: // FN_ADD
		MCODE_DEBUG("REGOP FN SELECT = FN_INC");
		t = Microcode::MicrocodeType::FN_INC;
		break;
	case 1: // FN_OR
		MCODE_DEBUG("REGOP FN SELECT = FN_DEC");
		t = Microcode::MicrocodeType::FN_DEC;
		break;
	default:
		// ERROR
		MCODE_DEBUG_ERR("REGOP FN SELECT = ERROR");
		triggerError();
		break;
	}

	MCODE_DEBUG("Executing daughter FN");
	Microcode mcode("FN_REGOP_FE_DAUGHTER", t);
	mcode_execCode(mcode);
}

void Processor_8086::mcode_fnAdd(bool adc) {
	if (m_cInstr.mCodeI.srcA.bytes > 1 || m_cInstr.mCodeI.srcB.bytes > 1)
		m_cInstr.mCodeI.dst.bytes = 2;
	else
		m_cInstr.mCodeI.dst.bytes = 1;

	// If we are ADC, we need to update the ALU carry bit
	if (adc)
		m_alu.setCarryBit(m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_CF));

	m_cInstr.mCodeI.dst.v = m_alu.add(m_cInstr.mCodeI.srcA.v, m_cInstr.mCodeI.srcB.v, adc);
	m_cInstr.mCodeI.dstEnabled = true;

	MCODE_DEBUG("srcA = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_OF, m_alu.overflowFlag());
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	// R_F.putBit(FLAGS_AF, m_alu.adjustFlag()); TODO - Also Aux Carry flag
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
	R_F.putBit(FLAGS_CF, m_alu.carryBit());
}

void Processor_8086::mcode_fnSub(bool sbb) {
	if (m_cInstr.mCodeI.srcA.bytes > 1 || m_cInstr.mCodeI.srcB.bytes > 1)
		m_cInstr.mCodeI.dst.bytes = 2;
	else
		m_cInstr.mCodeI.dst.bytes = 1;

	// If we are SBB, we need to update the ALU carry bit
	if (sbb)
		m_alu.setCarryBit(m_registers[REGISTERS::R_FLAGS].getBit(FLAGS_CF));

	m_cInstr.mCodeI.dst.v = m_alu.subtract(m_cInstr.mCodeI.srcA.v, m_cInstr.mCodeI.srcB.v, sbb);
	m_cInstr.mCodeI.dstEnabled = true;

	MCODE_DEBUG("srcA = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_OF, m_alu.overflowFlag());
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	// R_F.putBit(FLAGS_AF, m_alu.adjustFlag()); TODO - Also Aux Carry flag
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
	R_F.putBit(FLAGS_CF, m_alu.carryBit());
}

void Processor_8086::mcode_fnCmp() {
	if (m_cInstr.mCodeI.srcA.bytes > 1 || m_cInstr.mCodeI.srcB.bytes > 1)
		m_cInstr.mCodeI.dst.bytes = 2;
	else
		m_cInstr.mCodeI.dst.bytes = 1;

	// We don't generate an output here as we are doing a compare. Do a subtract but lose the value
	m_alu.subtract(m_cInstr.mCodeI.srcA.v, m_cInstr.mCodeI.srcB.v);
	m_cInstr.mCodeI.dstEnabled = false; // Don't allow DST_X microcodes to output

	MCODE_DEBUG("srcA = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	// MCODE_DEBUG("dst = " + COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_OF, m_alu.overflowFlag());
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	// R_F.putBit(FLAGS_AF, m_alu.adjustFlag()); TODO - Also Aux Carry flag
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
	R_F.putBit(FLAGS_CF, m_alu.carryBit());
}

void Processor_8086::mcode_fnDec() {
	m_cInstr.mCodeI.dst.bytes = m_cInstr.mCodeI.srcA.bytes;
	m_cInstr.mCodeI.dst.v = m_alu.decrement(m_cInstr.mCodeI.srcA.v);
	m_cInstr.mCodeI.dstEnabled = true;

	MCODE_DEBUG("srcA = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_OF, m_alu.overflowFlag());
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	// R_F.putBit(FLAGS_AF, m_alu.adjustFlag()); TODO - Also Aux Carry flag
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
}

void Processor_8086::mcode_fnInc() {
	m_cInstr.mCodeI.dst.bytes = m_cInstr.mCodeI.srcA.bytes;
	m_cInstr.mCodeI.dst.v = m_alu.increment(m_cInstr.mCodeI.srcA.v);
	m_cInstr.mCodeI.dstEnabled = true;

	MCODE_DEBUG("srcA = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_OF, m_alu.overflowFlag());
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	// R_F.putBit(FLAGS_AF, m_alu.adjustFlag()); TODO - Also Aux Carry flag
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
}

void Processor_8086::mcode_fnXOR() {
	if (m_cInstr.mCodeI.srcA.bytes > 1 || m_cInstr.mCodeI.srcB.bytes > 1)
		m_cInstr.mCodeI.dst.bytes = 2;
	else
		m_cInstr.mCodeI.dst.bytes = 1;

	m_cInstr.mCodeI.dst.v = m_alu.binaryXOR(m_cInstr.mCodeI.srcA.v, m_cInstr.mCodeI.srcB.v);
	m_cInstr.mCodeI.dstEnabled = true;

	MCODE_DEBUG("srcA = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
	R_F.clearBit(FLAGS_CF);
	R_F.clearBit(FLAGS_OF);
}

void Processor_8086::mcode_fnOR() {
	if (m_cInstr.mCodeI.srcA.bytes > 1 || m_cInstr.mCodeI.srcB.bytes > 1)
		m_cInstr.mCodeI.dst.bytes = 2;
	else
		m_cInstr.mCodeI.dst.bytes = 1;

	m_cInstr.mCodeI.dst.v = m_alu.binaryOR(m_cInstr.mCodeI.srcA.v, m_cInstr.mCodeI.srcB.v);
	m_cInstr.mCodeI.dstEnabled = true;

	MCODE_DEBUG("srcA = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
	R_F.clearBit(FLAGS_CF);
	R_F.clearBit(FLAGS_OF);
}

void Processor_8086::mcode_fnAND() {
	if (m_cInstr.mCodeI.srcA.bytes > 1 || m_cInstr.mCodeI.srcB.bytes > 1)
		m_cInstr.mCodeI.dst.bytes = 2;
	else
		m_cInstr.mCodeI.dst.bytes = 1;

	m_cInstr.mCodeI.dst.v = m_alu.binaryAND(m_cInstr.mCodeI.srcA.v, m_cInstr.mCodeI.srcB.v);
	m_cInstr.mCodeI.dstEnabled = true;

	MCODE_DEBUG("srcA = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + COutSys::ToHexStr(m_cInstr.mCodeI.dst.v));

	// Update the relevant flags
	Register16& R_F = m_registers[REGISTERS::R_FLAGS];
	R_F.putBit(FLAGS_SF, m_alu.negativeFlag()); // Sign flag = negative flag
	R_F.putBit(FLAGS_ZF, m_alu.zeroFlag());
	R_F.putBit(FLAGS_PF, m_alu.parityFlag());
	R_F.clearBit(FLAGS_CF);
	R_F.clearBit(FLAGS_OF);
}


void Processor_8086::mcode_fnCallRel() {
	// Get the relative 16 offset from srcA
	int16_t rel16 = (int16_t)m_cInstr.mCodeI.srcA.v;
	// Push IP to the stack. We need to load srcB with the value first
	m_cInstr.mCodeI.srcB.v = m_registers[REGISTERS::R_IP].read();
	m_cInstr.mCodeI.srcB.bytes = 2;
	mcode_stackPush(m_cInstr.mCodeI.srcB);
	// Now add thd rel16 to IP
	MCODE_DEBUG("[REL16] CALL to IP = " + COutSys::ToHexStr((m_registers[REGISTERS::R_IP].read() + rel16)));
	m_registers[REGISTERS::R_IP].put(m_registers[REGISTERS::R_IP].read() + rel16);
}

void Processor_8086::mcode_fnRetnNear() {
	// SRC_A has the IP we need to jump to
	MCODE_DEBUG("RETN to = " + COutSys::ToHexStr(m_cInstr.mCodeI.srcA.v));
	m_registers[REGISTERS::R_IP].put(m_cInstr.mCodeI.srcA.v);
	MCODE_DEBUG("RETN gave IP = " + COutSys::ToHexStr(m_registers[REGISTERS::R_IP].read()));
}