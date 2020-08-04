/**************************************************************************

P8086_MCsrc.cpp

Created by Matthew Clarke

Microcode execution functions definitions: microcode SRC instructions in this file

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
// SRC MICROCODE
/***********************************/

Processor_8086::CurrentInstruction::MicrocodeInformation::Cache& Processor_8086::mcode_getNextSrc() {
	auto& src = (m_cInstr.mCodeI.srcAUsed ? m_cInstr.mCodeI.srcB : m_cInstr.mCodeI.srcA);
	MCODE_DEBUG("SrcSelect, srcAUsed=" + std::to_string(m_cInstr.mCodeI.srcAUsed));
	if (!m_cInstr.mCodeI.srcAUsed)
		m_cInstr.mCodeI.srcAUsed = true;
	return src;
}

void Processor_8086::mcode_toSrcFromReg(Processor_8086::CurrentInstruction::MicrocodeInformation::Cache& src, uint8_t sval) {
	MCODE_DEBUG("SVAL = " + std::to_string(sval));

	if (m_cInstr.mCodeI.bitMode8Bit) {
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

void Processor_8086::mcode_toSrcFromMem00(Processor_8086::CurrentInstruction::MicrocodeInformation::Cache& src, uint8_t sval) {
	src.bytes = m_cInstr.mCodeI.bitMode8Bit ? 1 : 2;

	switch (sval) {
	case 0b000: // [BX + SI]
		MCODE_DEBUG("SRC = MEM [BX + SI]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_BX].read() + m_registers[REGISTERS::R_SI].read()));
		break;

	case 0b001: // [BX + DI]
		MCODE_DEBUG("SRC = MEM [BX + DI]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_BX].read() + m_registers[REGISTERS::R_DI].read()));
		break;

	case 0b010: // [BP + SI]
		MCODE_DEBUG("SRC = MEM [BP + SI]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_BP].read() + m_registers[REGISTERS::R_SI].read()));
		break;

	case 0b011: // [BP + DI]
		MCODE_DEBUG("SRC = MEM [BP + DI]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_BP].read() + m_registers[REGISTERS::R_DI].read()));
		break;

	case 0b100: // [SI]
		MCODE_DEBUG("SRC = MEM [SI]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_SI].read()));
		break;

	case 0b101: // [DI]
		MCODE_DEBUG("SRC = MEM [DI]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_DI].read()));
		break;

	case 0b110: // [sword]
		MCODE_DEBUG("SRC = MEM [sword], where sword=" + icarus::util::ToHexStr(m_cInstr.displacement));
		// We need to read memory at the position of the immediate byte, and then put that in the src
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA,m_cInstr.displacement));
		break;

	case 0b111: // [BX]
		MCODE_DEBUG("SRC = MEM [BX]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA,m_registers[REGISTERS::R_BX].read()));
		break;

	default:
		// ERROR
		MCODE_DEBUG_ERR("SRC = MEMORY_DECODE_ERROR");
		break;
	}

	MCODE_DEBUG("READ from " + icarus::util::ToHexStr(m_addressBus.readData()) + " of " + std::to_string(m_cInstr.mCodeI.dst.bytes) + " bytes");
	if (src.bytes == 1) {
		m_mmu.readByte(m_dataBus, m_addressBus);
	}
	else {
		m_mmu.fillBus(m_dataBus, m_addressBus, icarus::memory::MMU::ReadType::LittleEndian);
	}
	src.v = m_dataBus.readData();
}

void Processor_8086::mcode_toSrcFromMem10(Processor_8086::CurrentInstruction::MicrocodeInformation::Cache& src, uint8_t sval) {
	src.bytes = m_cInstr.mCodeI.bitMode8Bit ? 1 : 2;

	switch (sval) {
	case 0b000: // [BX + SI + sword]
		MCODE_DEBUG("SRC = MEM [BX + SI + sword]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_BX].read() + m_registers[REGISTERS::R_SI].read() + m_cInstr.displacement));
		break;

	case 0b001: // [BX + DI + sword]
		MCODE_DEBUG("SRC = MEM [BX + DI + sword]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA,m_registers[REGISTERS::R_BX].read() + m_registers[REGISTERS::R_DI].read() + m_cInstr.displacement));
		break;

	case 0b010: // [BP + SI + sword]
		MCODE_DEBUG("SRC = MEM [BP + SI + sword]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_BP].read() + m_registers[REGISTERS::R_SI].read() + m_cInstr.displacement));
		break;

	case 0b011: // [BP + DI + sword]
		MCODE_DEBUG("SRC = MEM [BP + DI + sword]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_BP].read() + m_registers[REGISTERS::R_DI].read() + m_cInstr.displacement));
		break;

	case 0b100: // [SI + sword]
		MCODE_DEBUG("SRC = MEM [SI + sword]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_SI].read() + m_cInstr.displacement));
		break;

	case 0b101: // [DI + sword]
		MCODE_DEBUG("SRC = MEM [DI + sword]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA,m_registers[REGISTERS::R_DI].read() + m_cInstr.displacement));
		break;

	case 0b110: // [BP + sword]
		MCODE_DEBUG("SRC = MEM [BP + sword]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA, m_registers[REGISTERS::R_BP].read() + m_cInstr.displacement));
		break;

	case 0b111: // [BX + sword]
		MCODE_DEBUG("SRC = MEM [BX + sword]");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_DATA,m_registers[REGISTERS::R_BX].read() + m_cInstr.displacement));
		break;

	default:
		// ERROR
		MCODE_DEBUG_ERR("SRC = MEMORY_DECODE_ERROR");
		break;
	}

	MCODE_DEBUG("READ from " + icarus::util::ToHexStr(m_addressBus.readData()) + " of " + std::to_string(m_cInstr.mCodeI.dst.bytes) + " bytes");
	if (src.bytes == 1) {
		m_mmu.readByte(m_dataBus, m_addressBus);
	}
	else {
		m_mmu.fillBus(m_dataBus, m_addressBus, icarus::memory::MMU::ReadType::LittleEndian);
	}
	src.v = m_dataBus.readData();
}

void Processor_8086::mcode_getSrcImm() {
	// We get the source from the immediate value.
	// We simply copy
	auto& src = mcode_getNextSrc();

	src.bytes = m_cInstr.numImmeditateBytes;
	src.v = m_cInstr.immediate;
}

void Processor_8086::mcode_getSrcDisp() {
	// We get the source from the displacement value.
	// We simply copy
	auto& src = mcode_getNextSrc();

	src.bytes = m_cInstr.numDisplacementBytes;
	src.v = m_cInstr.displacement;
}

void Processor_8086::mcode_getSrcModRM() {
	// We get the source from the ModRM values.
	// This could be a memory address (mod 00 - 10 inclusive) or a register (mod 11)(16 bit or 8 bit register, see m_cInstr.mCodeI.regMode8Bit)
	// If srcA is in use, we put in srcB
	auto& src = mcode_getNextSrc();

	MCODE_DEBUG("MOD=" + std::to_string(m_cInstr.modRMByte.MOD()));

	switch (m_cInstr.modRMByte.MOD()) {
	case 0b00: // Memory addressing
		MCODE_DEBUG("SRC = M");
		mcode_toSrcFromMem00(src, m_cInstr.modRMByte.RM()); // No displacement
		break;
	case 0b01: // Memory addressing + displacement8
		MCODE_DEBUG("SRC = M+disp8");
		mcode_toSrcFromMem10(src, m_cInstr.modRMByte.RM()); // Disp8
		break;
	case 0b10: // Memory addressing + displacement16
		MCODE_DEBUG("SRC = M+disp16");
		mcode_toSrcFromMem10(src, m_cInstr.modRMByte.RM()); // Disp16
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
	auto& src = mcode_getNextSrc();

	mcode_toSrcFromReg(src, m_cInstr.modRMByte.REGOP());
}