/**************************************************************************

P8086_MCdst.cpp

Created by Matthew Clarke

Microcode execution functions definitions: microcode DST instructions in this file

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
// DST MICROCODE
/***********************************/

void Processor_8086::mcode_toDstFromReg(uint8_t sval) {
	if (!m_cInstr.mCodeI.dstEnabled) {
		// Not allowed to write to dst
		MCODE_DEBUG("!dstEnabled: not writing to destination");
		return;
	}

	MCODE_DEBUG("SVAL = " + std::to_string(sval));

	if (m_cInstr.mCodeI.bitMode8Bit) {
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

void Processor_8086::mcode_toDstFromMem00(uint8_t sval) {
	if (!m_cInstr.mCodeI.dstEnabled) {
		// Not allowed to write to dst
		MCODE_DEBUG("!dstEnabled: not writing to destination");
		return;
	}

	uint32_t address = 0;

	switch (sval) {
	case 0b000: // [BX + SI]
		MCODE_DEBUG("DST = MEM [BX + SI]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b001: // [BX + DI]
		address = m_registers[REGISTERS::R_BX].read() + m_registers[REGISTERS::R_DI].read();
		MCODE_DEBUG("DST = MEM [BX + DI], where BX + DI = " + COutSys::ToHexStr(address));
		m_addressBus.putData(address);
		break;

	case 0b010: // [BP + SI]
		MCODE_DEBUG("DST = MEM [BP + SI]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b011: // [BP + DI]
		MCODE_DEBUG("DST = MEM [BP + DI]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b100: // [SI]
		MCODE_DEBUG("DST = MEM [SI]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b101: // [DI]
		MCODE_DEBUG("DST = MEM [DI]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b110: // [sword]
		MCODE_DEBUG("DST = MEM [sword], where sword = " + COutSys::ToHexStr(m_cInstr.displacement));
		// We need to write memory at the position of the displacement byte
		m_addressBus.putData(m_cInstr.displacement);
		break;

	case 0b111: // [BX]
		MCODE_DEBUG("DST = MEM [BX]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	default:
		// ERROR
		MCODE_DEBUG_ERR("DST = MEMORY_DECODE_ERROR");
		break;
	}

	MCODE_DEBUG("WRITE to " + icarus::COutSys::ToHexStr(m_addressBus.readData()) + " of " + std::to_string(m_cInstr.mCodeI.dst.bytes) + " bytes");
	m_dataBus.putData(m_cInstr.mCodeI.dst.v);
	if (m_cInstr.mCodeI.dst.bytes == 2) {
		m_mmu.writeBus(m_dataBus, m_addressBus, icarus::memory::MMU::ReadType::LittleEndian);
	}
	else {
		m_mmu.writeByte(m_dataBus, m_addressBus);
	}
}

void Processor_8086::mcode_toDstFromMem10(uint8_t sval) {
	if (!m_cInstr.mCodeI.dstEnabled) {
		// Not allowed to write to dst
		MCODE_DEBUG("!dstEnabled: not writing to destination");
		return;
	}

	uint32_t address = 0;

	switch (sval) {
	case 0b000: // [BX + SI + sword]
		MCODE_DEBUG("DST = MEM [BX + SI + sword]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b001: // [BX + DI + sword]
		address = m_registers[REGISTERS::R_BX].read() + m_registers[REGISTERS::R_DI].read() + m_cInstr.displacement;
		MCODE_DEBUG("DST = MEM [BX + DI + sword], where BX + DI + sword = " + COutSys::ToHexStr(address));
		m_addressBus.putData(address);
		break;

	case 0b010: // [BP + SI + sword]
		MCODE_DEBUG("DST = MEM [BP + SI + sword]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b011: // [BP + DI + sword]
		MCODE_DEBUG("DST = MEM [BP + DI + sword]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b100: // [SI + sword]
		MCODE_DEBUG("DST = MEM [SI + sword]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b101: // [DI + sword]
		address = m_registers[REGISTERS::R_DI].read() + m_cInstr.displacement;
		MCODE_DEBUG("DST = MEM [DI + sword], where DI + sword = " + COutSys::ToHexStr(address));
		m_addressBus.putData(address);
		break;

	case 0b110: // [BP + sword]
		MCODE_DEBUG("DST = MEM [BP + sword]");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;

	case 0b111: // [BX + sword]
		address = m_registers[REGISTERS::R_BX].read() + m_cInstr.displacement;
		MCODE_DEBUG("DST = MEM [BX + sword], where BX + sword = " + COutSys::ToHexStr(address));
		m_addressBus.putData(address);
		break;

	default:
		// ERROR
		MCODE_DEBUG_ERR("DST = MEMORY_DECODE_ERROR");
		break;
	}
	MCODE_DEBUG("WRITE to " + icarus::COutSys::ToHexStr(m_addressBus.readData()) + " of " + std::to_string(m_cInstr.mCodeI.dst.bytes) + " bytes");
	m_dataBus.putData(m_cInstr.mCodeI.dst.v);
	if (m_cInstr.mCodeI.dst.bytes == 2) {
		m_mmu.writeBus(m_dataBus, m_addressBus, icarus::memory::MMU::ReadType::LittleEndian);
	}
	else {
		m_mmu.writeByte(m_dataBus, m_addressBus);
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
		mcode_toDstFromMem00(m_cInstr.modRMByte.RM());
		break;
	case 0b01: // Memory addressing + displacement8
		MCODE_DEBUG("DST = M+disp8");
		MCODE_DEBUG_ERR("Not implemented!");
		triggerError();
		break;
	case 0b10: // Memory addressing + displacement16
		MCODE_DEBUG("DST = M+disp16");
		mcode_toDstFromMem10(m_cInstr.modRMByte.RM());
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