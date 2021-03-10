#ifndef _intel8086_hpp
#define _intel8086_hpp

/*
    **> \File           Intel86.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Header for Intel86.cpp
    **> \Details        None
    **> \Notes          None
*/

#include "ComputerGeneric/Addressable.hpp"

namespace i86 {

namespace intel86 {

// ****************************************************************
// Class Intel86
// Properties: None
// ****************************************************************

class Intel86 {

private:
    i86::compGeneric::Addressable& m_memorySpace;
    i86::compGeneric::Addressable& m_ioSpace;

public:
    /*
        \Function   Intel86
        \Brief      Constructs Intel86
        \Details    Constructs Intel86
        \Parameter  i86::compGeneric::Addressable& memorySpace
        \Parameter  i86::compGeneric::Addressable& ioSpace
        \Returns    None
    */
    Intel86(i86::compGeneric::Addressable& memorySpace, i86::compGeneric::Addressable& ioSpace);

}; // end class Intel86

} // end namespace intel86

} // end namespace i86

#endif