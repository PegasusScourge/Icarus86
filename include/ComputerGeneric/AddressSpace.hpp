#ifndef _AddressSpace_hpp
#define _AddressSpace_hpp

/*
    **> \File           Addressable.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Header for AddressSpace.cpp
    **> \Details        AddressSpace class interface for memory/IO access
    **> \Notes          None
*/

#include "Device/GenericDevice.hpp"
#include "AddressRange.hpp"

#include <cstdint>
#include <cstddef>
#include <mutex>
#include <map>
#include <memory>

namespace i86 {

namespace compGeneric {

// ****************************************************************
// Class AddressSpace
// Properties: Thread safe (lock_guard)
// ****************************************************************

class AddressSpace {

private:
    i86::compGeneric::AddressRange m_addrRange;

    std::mutex m_mutex;

    std::map<size_t, std::shared_ptr<i86::device::GenericDevice>> m_devices; // addr, Device

    /*
        \Function   performWrite
        \Brief      performWrite
        \Details    Performs a write to an address
        \Parameter  size_t addr
        \Parameter  uint8_t byte
        \Returns    None
    */
    void performWrite(size_t addr, uint8_t byte);

    /*
        \Function   performWrite
        \Brief      performRead
        \Details    Performs a read from an address
        \Parameter  size_t addr
        \Returns    uint8_t byte
    */
    uint8_t performRead(size_t addr);

public:
    /*
        \Function   AddressSpace
        \Brief      Constructor
        \Details    Constructor
        \Parameter  i86::compGeneric::AddressRange range
        \Returns    None
    */
    AddressSpace(i86::compGeneric::AddressRange range);

    /*
        \Function   registerDevice
        \Brief      Registers a device
        \Details    Registers a device
        \Parameter  std::shared_ptr<i86::device::GenericDevice> device
        \Parameter  std::vector<i86::compGeneric::AddressRange>& ranges
        \Returns    None
    */
    void registerDevice(std::shared_ptr<i86::device::GenericDevice> device, std::vector<i86::compGeneric::AddressRange>& ranges);

    /*
        \Function   writeByte
        \Brief      Writes a byte to the address if valid
        \Details    Writes a byte to the address if valid
        \Parameter  size_t address
        \Parameter  uint8_t byte
        \Returns    bool writeSuccess
    */
    bool writeByte(size_t address, uint8_t byte);

    /*
        \Function   readByte
        \Brief      Reads a byte from the address if valid
        \Details    Reads a byte from the address if valid
        \Parameter  size_t address
        \Parameter  uint8_t& outbyte
        \Returns    bool readSuccess
    */
    bool readByte(size_t address, uint8_t& outbyte);

    /*
        \Function   getSizeBytes
        \Brief      getSizeBytes
        \Details    getSizeBytes
        \Parameter  None
        \Returns    size_t sizeBytes
    */
    size_t getSizeBytes();

    /*
        \Function   getStartAddress
        \Brief      getStartAddress
        \Details    getStartAddress
        \Parameter  None
        \Returns    size_t startAddress
    */
    size_t getStartAddress();

}; // end class AddressSpace

} // end namespace compGeneric

} // end namespace i86

#endif