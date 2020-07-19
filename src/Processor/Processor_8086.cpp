/**************************************************************************

Processor_8086.cpp

Created by Matthew Clarke

Implementation of the 8086 processor

 **************************************************************************/

#include "Processor_8086.hpp"

namespace i = icarus;

i::Processor_8086::Processor_8086(icarus::Bus16& dataBus, icarus::Bus32& addressBus) : m_dataBus(dataBus), m_addressBus(addressBus) {
	setName("Intel 8086");

	// Create the registers
	for(int i = 0; i < 4; i++)
		m_registers.push_back(Register16{ true }); // Create registers AX through DX

	for (int i = 0; i < 5; i++)
		m_registers.push_back(Register16{ false }); // Create registers SI through IP

	m_registers.push_back(Register16{ true }); // Create register FLAGS

	for (int i = 0; i < 5; i++)
		m_registers.push_back(Register16{ false }); // Create registers CS through SS

}

void i::Processor_8086::fetch() {

}

unsigned int i::Processor_8086::decode() {
	return 1;
}

void i::Processor_8086::execute() {

}

uint32_t i::Processor_8086::resolveAddress(uint16_t segment, uint16_t offset) {
	return (segment * 0x10) + offset;
}

std::vector<uint64_t> i::Processor_8086::getRegisters() {
	std::vector<uint64_t> regs;
	for (auto& r : m_registers) {
		regs.push_back(r.read());
	}
	return regs;
}