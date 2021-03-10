/*
    **> \File           MemoryController.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Basic memory controller
    **> \Details        Basic memory controller
    **> \Notes          None
*/

#include "ComputerGeneric/MemoryController.hpp"

#include <memory>

// ****************************************************************
// Class MemoryController
// ****************************************************************

i86::compGeneric::MemoryController::MemoryController(size_t spaceStartAddress, size_t spaceSizeBytes) : Addressable(spaceStartAddress, spaceSizeBytes) {
    m_mem = std::make_unique<uint8_t[]>(spaceSizeBytes);
}

void i86::compGeneric::MemoryController::performWrite(size_t localAddr, uint8_t byte) {
    m_mem[localAddr] = byte;
}

uint8_t i86::compGeneric::MemoryController::performRead(size_t localAddr) {
    return m_mem[localAddr];
}