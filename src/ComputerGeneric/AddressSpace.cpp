/*
    **> \File           AddressSpace.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          AddressSpace class interface for memory/IO access
    **> \Details        AddressSpace class interface for memory/IO access
    **> \Notes          None
*/

#include "Util/LogFile.hpp"
#include "ComputerGeneric/AddressSpace.hpp"

#include <vector>

// ****************************************************************
// Class Addressable
// ****************************************************************

i86::compGeneric::AddressSpace::AddressSpace(i86::compGeneric::AddressRange range) : m_addrRange{ range } {

}

bool i86::compGeneric::AddressSpace::writeByte(size_t address, uint8_t byte) {
    if (address >= m_addrRange.start && (address - m_addrRange.start) < m_addrRange.range) {
        performWrite(address, byte);
        return true;
    }
    return false;
}

bool i86::compGeneric::AddressSpace::readByte(size_t address, uint8_t& byte) {
    if (address >= m_addrRange.start && (address - m_addrRange.start) < m_addrRange.range) {
        byte = performRead(address);
        return true;
    }
    return false;
}

size_t i86::compGeneric::AddressSpace::getSizeBytes() {
    return m_addrRange.range;
}

size_t i86::compGeneric::AddressSpace::getStartAddress() {
    return m_addrRange.start;
}

void i86::compGeneric::AddressSpace::performWrite(size_t addr, uint8_t byte) {
    std::lock_guard guard(m_mutex);

    if (m_devices.count(addr)) {
        m_devices[addr]->writeAddress(addr, byte);
    }
}

uint8_t i86::compGeneric::AddressSpace::performRead(size_t addr) {
    std::lock_guard guard(m_mutex);
    uint8_t retVal = 0;

    if (m_devices.count(addr)) {
        retVal = m_devices[addr]->readAddress(addr);
    }

    return retVal;
}

void i86::compGeneric::AddressSpace::registerDevice(std::shared_ptr<i86::device::GenericDevice> device, std::vector<i86::compGeneric::AddressRange>& ranges) {
    for (auto& range : ranges) {
        if (!m_addrRange.isInThisRange(range)) {
            i86::util::LogFile::Generic.log_str("Unable to register device HUMAN_NAME = " + device->getHumanName() + " at address range " + 
                std::to_string(range.start) + " --> " + std::to_string(range.range) + " : Out of range");
            continue;
        }

        for (size_t i = range.start; i < range.range; i++) {
            m_devices[i] = device;
        }
        i86::util::LogFile::Generic.log_str("Registered device HUMAN_NAME = " + device->getHumanName() + " at address range " + 
            std::to_string(range.start) + " --> " + std::to_string(range.range));
    }
}