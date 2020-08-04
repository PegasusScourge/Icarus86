/**************************************************************************

P8086_MCfn.cpp

Created by Matthew Clarke

Microcode execution functions definitions: microcode FN instructions in this file

 **************************************************************************/

#include "P8086.hpp"

#include "../COutSys.hpp"
#include "../Constexprs.hpp"
#include "../Util.hpp"

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

	MCODE_DEBUG("srcA = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + icarus::util::ToHexStr(m_cInstr.mCodeI.dst.v));

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

	MCODE_DEBUG("srcA = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + icarus::util::ToHexStr(m_cInstr.mCodeI.dst.v));

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

	MCODE_DEBUG("srcA = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v));
	// MCODE_DEBUG("dst = " + icarus::util::ToHexStr(m_cInstr.mCodeI.dst.v));

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

	MCODE_DEBUG("srcA = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + icarus::util::ToHexStr(m_cInstr.mCodeI.dst.v));

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

	MCODE_DEBUG("srcA = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + icarus::util::ToHexStr(m_cInstr.mCodeI.dst.v));

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

	MCODE_DEBUG("srcA = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + icarus::util::ToHexStr(m_cInstr.mCodeI.dst.v));

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

	MCODE_DEBUG("srcA = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + icarus::util::ToHexStr(m_cInstr.mCodeI.dst.v));

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

	MCODE_DEBUG("srcA = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v) + ", srcB = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v));
	MCODE_DEBUG("dst = " + icarus::util::ToHexStr(m_cInstr.mCodeI.dst.v));

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
	MCODE_DEBUG("[REL16] CALL to IP = " + icarus::util::ToHexStr((m_registers[REGISTERS::R_IP].read() + rel16)));
	m_registers[REGISTERS::R_IP].put(m_registers[REGISTERS::R_IP].read() + rel16);
}

void Processor_8086::mcode_fnCallFarAbs() {
	// Get the seg absolute offset from srcA
	uint16_t segOffset = m_cInstr.mCodeI.srcA.v;
	// Get the segment from srcB
	uint16_t seg = m_cInstr.mCodeI.srcB.v;
	// Push CS to stack
	m_cInstr.mCodeI.srcA.v = m_registers[REGISTERS::R_CS].read();
	m_cInstr.mCodeI.srcA.bytes = 2;
	mcode_stackPush(m_cInstr.mCodeI.srcA);
	// Push IP to stack
	m_cInstr.mCodeI.srcA.v = m_registers[REGISTERS::R_IP].read();
	m_cInstr.mCodeI.srcA.bytes = 2;
	mcode_stackPush(m_cInstr.mCodeI.srcA);
	// Now we set IP and CS
	MCODE_DEBUG("[FAR_ABS] CALL to CS:IP = " + icarus::util::ToHexStr(seg) + ":" + icarus::util::ToHexStr(segOffset));
	m_registers[REGISTERS::R_IP].put(segOffset);
	m_registers[REGISTERS::R_CS].put(seg);
}

void Processor_8086::mcode_fnRetnNear() {
	// SRC_A has the IP we need to jump to
	MCODE_DEBUG("RETN to = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v));
	m_registers[REGISTERS::R_IP].put(m_cInstr.mCodeI.srcA.v);
	MCODE_DEBUG("RETN gave IP = " + icarus::util::ToHexStr(m_registers[REGISTERS::R_IP].read()));
}

void Processor_8086::mcode_fnRetnFar() {
	// SRC_A has the IP we need to jump to
	m_registers[REGISTERS::R_IP].put(m_cInstr.mCodeI.srcA.v);
	// SRC_B has the CS we need to return to
	m_registers[REGISTERS::R_CS].put(m_cInstr.mCodeI.srcB.v);
	MCODE_DEBUG("RETN to CS:IP = " + icarus::util::ToHexStr(m_cInstr.mCodeI.srcB.v) + ":" + icarus::util::ToHexStr(m_cInstr.mCodeI.srcA.v));
}

void Processor_8086::mcode_fnInt() {
	// Execute an interrupt
	uint32_t intAddressBase = m_cInstr.immediate * 4; // Get the address in the IVT (segment 0) that we need to look at

	// We now need to push the flags register to the stack
	m_cInstr.mCodeI.srcA.v = m_registers[REGISTERS::R_FLAGS].read();
	m_cInstr.mCodeI.srcA.bytes = 2;
	mcode_stackPush(m_cInstr.mCodeI.srcA);

	// Now set IE to 0
	m_registers[REGISTERS::R_FLAGS].clearBit(FLAGS_IE);

	m_addressBus.putData(intAddressBase);

	// Now execute the far jump, loading srcA with intAddressBase (segOffset) and srcB with (intAddressBase + 2) (seg), both 2 byte
	m_cInstr.mCodeI.srcA.bytes = 2;
	m_mmu.fillBus(m_dataBus, m_addressBus, icarus::memory::MMU::ReadType::LittleEndian);
	m_cInstr.mCodeI.srcA.v = m_dataBus.readData();

	m_addressBus.putData(intAddressBase + 2);

	m_cInstr.mCodeI.srcB.bytes = 2;
	m_mmu.fillBus(m_dataBus, m_addressBus, icarus::memory::MMU::ReadType::LittleEndian);
	m_cInstr.mCodeI.srcB.v = m_dataBus.readData();

	mcode_fnCallFarAbs();
}

void Processor_8086::mcode_fnIRet() {
	// We need to return from an interrupt. On the stack is, in order:
	// IP
	// CS
	// FLAGS

	// RetnFar takes srcA=IP, srcB=CS
	m_cInstr.mCodeI.srcA.bytes = 2;
	m_cInstr.mCodeI.srcB.bytes = 2;
	mcode_stackPop(m_cInstr.mCodeI.srcA); // Free IP
	mcode_stackPop(m_cInstr.mCodeI.srcB); // Free CS
	// Call the retn function
	mcode_fnRetnFar();

	// Now pop FLAGS
	mcode_stackPop(m_cInstr.mCodeI.srcA); // Free FLAGS
	// Now put flags
	m_registers[REGISTERS::R_FLAGS].put(m_cInstr.mCodeI.srcA.v);
}