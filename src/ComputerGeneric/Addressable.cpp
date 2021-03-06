/*
    **> \File           Addressable.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Addressable class interface for memory/IO access
    **> \Details        Addressable class interface for memory/IO access
    **> \Notes          None
*/

#include "ComputerGeneric/Addressable.hpp"

// ****************************************************************
// Class Addressable
// ****************************************************************

i86::compGeneric::Addressable::Addressable(size_t spaceStartAddress, size_t spaceSizeBytes) {
    m_sizeBytes = spaceSizeBytes;
    m_startAddress = spaceStartAddress;
}

bool i86::compGeneric::Addressable::writeByte(size_t address, uint8_t byte) {
    if (address >= m_startAddress && (address - m_startAddress) < m_sizeBytes) {
        std::lock_guard guard(m_mutex);
        performWrite(address - m_startAddress, byte);
        return true;
    }
    return false;
}

bool i86::compGeneric::Addressable::readByte(size_t address, uint8_t& byte) {
    if (address >= m_startAddress && (address - m_startAddress) < m_sizeBytes) {
        std::lock_guard guard(m_mutex);
        byte = performRead(address - m_startAddress);
        return true;
    }
    return false;
}

size_t i86::compGeneric::Addressable::getSizeBytes() {
    return m_sizeBytes;
}

size_t i86::compGeneric::Addressable::getStartAddress() {
    return m_startAddress;
}