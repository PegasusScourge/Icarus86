#ifndef _addressable_hpp
#define _addressable_hpp

/*
    **> \File           Addressable.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Header for Addressable.cpp
    **> \Details        Addressable class interface for memory/IO access
    **> \Notes          None
*/

#include <cstdint>
#include <cstddef>

namespace i86 {

namespace compGeneric {

// ****************************************************************
// Class Addressable
// ****************************************************************

class Addressable {

private:
    size_t m_sizeBytes;
    size_t m_startAddress;

    virtual void performWrite(size_t addr, uint8_t byte) = 0;
    virtual uint8_t performRead(size_t addr) = 0;

public:
    /*
        \Function   Addressable
        \Brief      Constructor
        \Details    Constructor
        \Parameter  size_t spaceSizeBytes
        \Parameter  size_t spaceStartAddress
        \Returns    None
    */
    Addressable(size_t spaceSizeBytes, size_t spaceStartAddress);

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

}; // end class Addressable

} // end namespace compGeneric

} // end namespace i86

#endif