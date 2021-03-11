/*
    **> \File           Intel86.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          The Intel 8086 CPU
    **> \Details        None
    **> \Notes          None
*/

#include "Intel8086/Intel86.hpp"
#include "Intel8086/I86Consts.hpp"

namespace gc = i86::compGeneric;

// ****************************************************************
// Class Intel86
// ****************************************************************

i86::intel86::Intel86::Intel86(gc::Addressable& memorySpace, gc::Addressable& ioSpace, i86::intel86::instruction::InstructionSet& iSet) : 
    m_memorySpace {memorySpace}, m_ioSpace {ioSpace}, m_instructions {iSet} {
    
    
}