/*
    **> \File           Intel86.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          The Intel 8086 CPU
    **> \Details        None
    **> \Notes          None
*/

#include "Intel8086/Intel86.hpp"

// ****************************************************************
// Class Intel86
// ****************************************************************

i86::intel86::Intel86::Intel86(i86::compGeneric::Addressable& memorySpace, i86::compGeneric::Addressable& ioSpace) : m_memorySpace {memorySpace}, m_ioSpace {ioSpace} {
    
}