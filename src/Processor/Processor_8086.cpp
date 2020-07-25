/**************************************************************************

Processor_8086.cpp

Created by Matthew Clarke

Implementation of the 8086 processor

 **************************************************************************/

#include "Processor_8086.hpp"

#include "../COutSys.hpp"
#include "../Constexprs.hpp"
#include "Instruction/ModRMByte.hpp"

namespace ip = icarus::processor;
namespace ipi = ip::instruction;

/***********************************/
// class Processor_8086 : public
/***********************************/

std::string ip::Processor_8086::REGISTER_NAMES[14] = { "AX", "BX", "CX", "DX", "SI", "DI", "BP", "SP", "IP", "FS", "CS", "DS", "ES", "SS" };

ip::Processor_8086::Processor_8086(icarus::memory::MMU& mmu, icarus::bus::Bus16& dataBus, icarus::bus::Bus32& addressBus) : m_mmu(mmu), m_dataBus(dataBus), m_addressBus(addressBus) {
	m_name = "Intel 8086";

	// Configure our state
	m_state.registerValues_names = REGISTER_NAMES;

	// Create the registers
	for(int i = 0; i < 4; i++)
		m_registers.push_back(Register16{ true }); // Create registers AX through DX

	for (int i = 0; i < 5; i++)
		m_registers.push_back(Register16{ false }); // Create registers SI through IP

	m_registers.push_back(Register16{ true }); // Create register FLAGS

	for (int i = 0; i < 4; i++)
		m_registers.push_back(Register16{ false }); // Create registers CS through SS

	// Get the instrction set
	m_iSet = icarus::processor::instruction::InstructionSet(icarus::processor::instruction::InstructionPath + "8086.json");

	m_alu.test();
}

unsigned int ip::Processor_8086::fetchDecode() {
	unsigned int cyclesToWait = 1;

	// Load the address bus with the address
	unsigned int increment = 0;
	uint16_t ipVal = m_registers[(int)REGISTERS::R_IP].read();
	m_addressBus.putData(ipVal);
	if (!m_mmu.tryReadByte(m_dataBus, m_addressBus)) {
		// Failed to read a byte
		triggerError();
		return 0;
	}

	ipi::ICode& instr = m_iSet[(uint8_t)m_dataBus.readData()];

	while (instr.isValid() && instr.isPrefix()) {
		m_addressBus.putData(ipVal + (++increment));
		if (!m_mmu.tryReadByte(m_dataBus, m_addressBus)) {
			// Failed to read a byte
			icarus::COutSys::Println("Processor8086 failed to read byte", icarus::COutSys::LEVEL_ERR);
			triggerError();
			return 0;
		}

		instr = instr[(uint8_t)m_dataBus.readData()];
	}

	// Add to the last ICodes
	m_state.lastICodes.push(instr);

#ifdef DECODE8086_DEBUG_PRINT
	icarus::COutSys::Println("Processor8086 DECODE information:", icarus::COutSys::LEVEL_INFO);
	icarus::COutSys::Println("Pushed instr code=" + icarus::COutSys::ToHexStr(instr.getCode()));
#endif

	if (!instr.isValid()) {
		// Failed to get a valid instruction
		icarus::COutSys::Println("Processor8086 failed to get valid instr", icarus::COutSys::LEVEL_ERR);
		triggerError();
		return 0;
	}
#ifdef DECODE8086_DEBUG_PRINT
	icarus::COutSys::Println("Instr valid");
#endif

	m_cInstr.code = instr.getCode();

	// We have a valid instruction, now lets get the microcode
	m_cInstr.microcode = instr.getMicrocode();
	if (m_cInstr.microcode.size() == 0) {
		// No microcode!
		icarus::COutSys::Println("Processor8086 found no microcode in instr", icarus::COutSys::LEVEL_WARN);
		return 0;
	}
#ifdef DECODE8086_DEBUG_PRINT
	icarus::COutSys::Println("Has microcode");
#endif

	// Get a ModRMByte if needed
	if (instr.hasModRM()) {
#ifdef DECODE8086_DEBUG_PRINT
		icarus::COutSys::Println("Has modrm");
#endif
		m_addressBus.putData(ipVal + (++increment));
		if (!m_mmu.tryReadByte(m_dataBus, m_addressBus)) {
			// Failed to read a byte
			icarus::COutSys::Println("Processor8086 failed to read byte (for ModRM)", icarus::COutSys::LEVEL_ERR);
			triggerError();
			return 0;
		}
		m_cInstr.modRMByte.setByte((uint8_t)m_dataBus.readData());
#ifdef DECODE8086_DEBUG_PRINT
		icarus::COutSys::Println("MODRM = " + icarus::COutSys::ToHexStr(m_dataBus.readData()));
#endif
	}

	// Get displacment bytes if needed
	if (instr.numDisplacementBytes() > 0) {
#ifdef DECODE8086_DEBUG_PRINT
		icarus::COutSys::Println("Has displacement");
#endif
		m_addressBus.putData(ipVal + (++increment));
		m_cInstr.displacement = 0;
		switch (instr.numDisplacementBytes()) {
		case 2:
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.displacement |= (m_dataBus.readData() << 8);
			m_addressBus.putData(ipVal + (++increment));
		case 1:
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.displacement |= m_dataBus.readData();
			break;

		default:
			// Error getting displacement bytes
			icarus::COutSys::Println("Processor8086 failed to get displacement bytes (num=" + 
				std::to_string(instr.numDisplacementBytes()) + ")", icarus::COutSys::LEVEL_ERR);
			triggerError();
			return 0;
		}
	}

	// Get immediate bytes if needed
	if (instr.numImmediateBytes() > 0) {
#ifdef DECODE8086_DEBUG_PRINT
		icarus::COutSys::Println("Has immediate");
#endif
		m_addressBus.putData(ipVal + (++increment));
		m_cInstr.immediate = 0;
		switch (instr.numImmediateBytes()) {
		case 2:
#ifdef DECODE8086_DEBUG_PRINT
			icarus::COutSys::Println("2 Byte");
#endif
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.immediate |= m_dataBus.readData();
			m_addressBus.putData(ipVal + (++increment));
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.immediate |= (m_dataBus.readData() << 8);
			break;
		case 1:
#ifdef DECODE8086_DEBUG_PRINT
			icarus::COutSys::Println("1 Byte");
#endif
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.immediate |= m_dataBus.readData();
			break;

		default:
			// Error getting displacement bytes
			icarus::COutSys::Println("Processor8086 failed to get immediate bytes (num=" +
				std::to_string(instr.numImmediateBytes()) + ")", icarus::COutSys::LEVEL_ERR);
			triggerError();
			return 0;
		}
	}

	// Update the instruction pointer
	m_state.lastIPs.push(ipVal);
	m_state.lastDisplacements.push(m_cInstr.displacement);
	m_state.lastImmediates.push(m_cInstr.immediate);
	m_registers[(int)REGISTERS::R_IP].put(ipVal + increment + 1);

	return cyclesToWait;
}

void ip::Processor_8086::execute() {
#ifdef EXEC8086_DEBUG_PRINT
	// Print out the information
	icarus::COutSys::Println("Processor8086 execution information:", icarus::COutSys::LEVEL_INFO);
	icarus::COutSys::Println("code = " + icarus::COutSys::ToHexStr(m_cInstr.code));
	icarus::COutSys::Println("immediate = " + icarus::COutSys::ToHexStr(m_cInstr.immediate));
	icarus::COutSys::Println("displacement = " + icarus::COutSys::ToHexStr(m_cInstr.displacement));
	icarus::COutSys::Println("MODRM = " + icarus::COutSys::ToHexStr(m_cInstr.modRMByte.byte()));
#endif

	// We now have to execute the microcode instructions

	// Init the mCodeI struct
	m_cInstr.mCodeI.regMode8Bit = false;
	m_cInstr.mCodeI.srcAUsed = false;

	for (auto& mcode : m_cInstr.microcode) {
		mcode_execCode(mcode);
	}
}

void ip::Processor_8086::forceIP(uint64_t ip) {
	m_registers[(int)REGISTERS::R_IP].put(ip);
}

void ip::Processor_8086::forceSP(uint64_t sp) {
	m_registers[(int)REGISTERS::R_SP].put(sp);
}


/***********************************/
// class Processor_8086 : private
/***********************************/

uint32_t ip::Processor_8086::resolveAddress(uint16_t segment, uint16_t offset) {
	return (segment * 0x10) + offset;
}

void ip::Processor_8086::onError() {

}

void ip::Processor_8086::onGetProcessorState() {
	std::vector<uint64_t> regs_i;
	for (auto& r : m_registers) {
		regs_i.push_back(r.read());
	}
	m_state.registerValues_num = regs_i;
	
	std::vector<std::string> regs_s;
	for (auto& r : m_registers) {
		regs_s.push_back(COutSys::ToHexStr(r.read(), true));
	}
	m_state.registerValues_str = regs_s;
}
