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
#include "Instruction/InstructionSet.hpp"

#include <cstdint>

namespace i86 {

namespace intel86 {

// ****************************************************************
// Class Register16
// Properties: None
// ****************************************************************

class Register16 {
public:
    static constexpr unsigned int H = 0;
    static constexpr unsigned int L = 1;

    union {
        uint16_t X;
        uint8_t x[2];
    };
};

// ****************************************************************
// Class Intel86
// Properties: None
// ****************************************************************

class Intel86 {

private:
    i86::compGeneric::Addressable& m_memorySpace;
    i86::compGeneric::Addressable& m_ioSpace;
    i86::intel86::instruction::InstructionSet& m_instructions;

    size_t m_ticksToIdle = 0;

public:
    /*
        \Function   Intel86
        \Brief      Constructs Intel86
        \Details    Constructs Intel86
        \Parameter  i86::compGeneric::Addressable& memorySpace
        \Parameter  i86::compGeneric::Addressable& ioSpace
        \Parameter  i86::intel86::instruction::InstructionSet& iSet
        \Returns    None
    */
    Intel86(i86::compGeneric::Addressable& memorySpace, i86::compGeneric::Addressable& ioSpace, i86::intel86::instruction::InstructionSet& iSet);

    /*
        \Function   reset
        \Brief      Resets the 8086 to it's default values
        \Details    Resets the 8086 to it's default values
        \Parameter  None
        \Returns    None
    */
    void reset();

    /*
        \Function   tick
        \Brief      Attempts to execute the next instruction on the 8086
        \Details    Takes into account any clock costs
        \Parameter  None
        \Returns    None
    */
    void tick();

}; // end class Intel86

} // end namespace intel86

} // end namespace i86

#endif