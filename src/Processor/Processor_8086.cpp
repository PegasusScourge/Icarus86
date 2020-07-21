/**************************************************************************

Processor_8086.cpp

Created by Matthew Clarke

Implementation of the 8086 processor

 **************************************************************************/

#include "Processor_8086.hpp"

#include "../COutSys.hpp"

namespace ip = icarus::processor;

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

	m_alu.test();
}

void ip::Processor_8086::fetch() {

}

unsigned int ip::Processor_8086::decode() {
	return 1;
}

void ip::Processor_8086::execute() {

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
		regs.push_back(COutSys::ToHexStr(r.read(), true, true));
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
