#ifndef _memorycontroller_hpp
#define _memorycontroller_hpp

/*
    **> \File           MemoryController.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Header for MemoryController.cpp
    **> \Details        Basic memory controller
    **> \Notes          None
*/

#include "Addressable.hpp"

#include <memory>

namespace i86 {

namespace compGeneric {

// ****************************************************************
// Class MemoryController
// Properties: Thread safe (lock_guard)
// ****************************************************************

class MemoryController : public Addressable {

private:
    void performWrite(size_t localAddr, uint8_t byte);
    uint8_t performRead(size_t localAddr);

    std::unique_ptr<uint8_t[]> m_mem;

public:
    /*
        \Function   MemoryController
        \Brief      Constructor
        \Details    Constructor
        \Parameter  size_t spaceSizeBytes
        \Parameter  size_t spaceStartAddress
        \Returns    None
    */
    MemoryController(size_t spaceStartAddress, size_t spaceSizeBytes);

}; // end class MemoryController

} // end namespace compGeneric

} // end namespace i86

#endif