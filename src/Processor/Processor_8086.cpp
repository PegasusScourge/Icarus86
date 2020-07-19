/**************************************************************************

Processor_8086.cpp

Created by Matthew Clarke

Implementation of the 8086 processor

 **************************************************************************/

#include "Processor_8086.hpp"

namespace i = icarus;

i::Processor_8086::Processor_8086(icarus::Bus16& dataBus, icarus::Bus32& addressBus) : m_dataBus(dataBus), m_addressBus(addressBus) {
	setName("Intel 8086");
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