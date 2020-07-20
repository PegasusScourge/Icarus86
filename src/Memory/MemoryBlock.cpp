/**************************************************************************

MemoryBlock.cpp

Created by Matthew Clarke

Controls a block of memory that the CPU can access

 **************************************************************************/

#include "MemoryBlock.hpp"

#include "../COutSys.hpp"

namespace im = icarus::memory;

im::MemoryBlock::MemoryBlock(size_t startAddress, size_t size) : m_data(size) {
	m_size = size;
	m_startAddress = startAddress;
}

im::MemoryBlock::~MemoryBlock() {

}

bool im::MemoryBlock::isInRange(size_t address) {
	return address >= m_startAddress && address < (m_startAddress + m_size);
}

void im::MemoryBlock::tryByteWrite(size_t address, uint8_t value) {
	if (isInRange(address))
		m_data[address - m_startAddress] = value;
}

bool im::MemoryBlock::tryByteRead(size_t address, uint8_t* dest) {
	if (!isInRange(address) || dest == nullptr)
		return false;

	*dest = m_data[address - m_startAddress];
	return true;
}

size_t im::MemoryBlock::getSize() {
	return m_size;
}

size_t im::MemoryBlock::getStartAddress() {
	return m_startAddress;
}