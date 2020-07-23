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
	unsigned int increment = 1;
	uint16_t ipVal = m_registers[(int)REGISTERS::R_IP].read();
	m_addressBus.putData(ipVal);
	if (!m_mmu.tryReadByte(m_dataBus, m_addressBus)) {
		// Failed to read a byte
		m_failed = true;
		return 0;
	}

	ipi::ICode& instr = m_iSet[(uint8_t)m_dataBus.readData()];

	while (instr.isValid() && instr.isPrefix()) {
		m_addressBus.putData(ipVal + (++increment));
		if (!m_mmu.tryReadByte(m_dataBus, m_addressBus)) {
			// Failed to read a byte
			icarus::COutSys::Println("Processor8086 failed to read byte", icarus::COutSys::LEVEL_ERR);
			m_failed = true;
			return 0;
		}

		instr = instr[(uint8_t)m_dataBus.readData()];
	}

	if (!instr.isValid()) {
		// Failed to get a valid instruction
		icarus::COutSys::Println("Processor8086 failed to get valid instr", icarus::COutSys::LEVEL_ERR);
		m_failed = true;
		return 0;
	}

	// We have a valid instruction, now lets get the microcode
	cInstrMicrocode = instr.getMicrocode();
	if (cInstrMicrocode.size() == 0) {
		// No microcode!
		icarus::COutSys::Println("Processor8086 found no microcode in instr", icarus::COutSys::LEVEL_WARN);
		return 0;
	}

	// Update the instruction pointer
	m_registers[(int)REGISTERS::R_IP].put(ipVal + increment);

	return cyclesToWait;
}

void ip::Processor_8086::execute() {
	// We now have to execute the microcode instructions
	uint16_t srcCache[2];
	uint16_t* srcPtr = srcCache;

	for (auto& mcode : cInstrMicrocode) {
		namespace m = icarus::processor::instruction;
		switch (mcode.getType()) {

		default:
			// We assume NOP and do nothing
			break;

		}
	}
}

std::vector<uint64_t> ip::Processor_8086::getRegisterValues() {
	std::vector<uint64_t> regs;
	for (auto& r : m_registers) {
		regs.push_back(r.read());
	}
	return regs;
}

std::vector<std::string> ip::Processor_8086::getRegisterValuesAsStr() {
	std::vector<std::string> regs;
	for (auto& r : m_registers) {
		regs.push_back(COutSys::ToHexStr(r.read(), true));
	}
	return regs;
}

std::string* ip::Processor_8086::getRegisterNames() {
	return REGISTER_NAMES;
}

/***********************************/
// class Processor_8086 : private
/***********************************/

uint32_t ip::Processor_8086::resolveAddress(uint16_t segment, uint16_t offset) {
	return (segment * 0x10) + offset;
}

void ip::Processor_8086::onErrorDumpToConsole() {

}
