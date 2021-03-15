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

#include "Intel8086/I86Consts.hpp"
#include "ComputerGeneric/Addressable.hpp"
#include "Instruction/InstructionSet.hpp"
#include "Instruction/CurrentInstruction.hpp"
#include "Util/HexUtil.hpp"

#include "SFML/System/Clock.hpp"

#include <cstdint>
#include <ostream>

namespace i86 {

namespace intel86 {

// ****************************************************************
// Class Register16
// Properties: None
// ****************************************************************

class Register16 {
public:
    friend std::ostream &operator<<(std::ostream &output, const Register16 &r) { 
        output << i86::util::NumToHexStr(r.X);
        return output;
    }

    uint16_t X;
    uint8_t H() {
        return (X & 0xFF00) >> 8;
    }
    void sH(uint8_t v) {
        X = (v << 8) & (X & 0xFF);
    }
    uint8_t L() {
        return (X & 0xFF);
    }
    void sL(uint8_t v) {
        X = (v) & (X & 0xFF00);
    }
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

    Register16 m_registers[i86::intel86::consts::NUM_REGISTERS];

    size_t m_ticksToIdle = 0;
    size_t accumulator = 0;
    sf::Clock m_timingClock;

    size_t m_numFaults = 0;
    static constexpr size_t NUM_MAX_FAULTS = 1;
    bool m_isFaulted = false;

    // Current instruction
    i86::intel86::instruction::CurrentInstruction m_cInstr;

    /*
        \Function   fault
        \Brief      Faults the CPU
        \Details    Faults the CPU
        \Parameter  std::string reason (default "None given")
        \Returns    None
    */
    void fault(std::string reason = "None given");

    /*
        \Function   primeCurrentInstruction
        \Brief      Primes m_cInstr with data
        \Details    Primes m_cInstr with data
        \Parameter  i86::intel86::instruction::ICode& instr
        \Parameter  uint32_t baseIP
        \Parameter  uint32_t& ipInc
        \Returns    None
    */
    void primeCurrentInstruction(i86::intel86::instruction::ICode& instr, uint32_t baseIP, uint32_t& ipInc);

    /*
        \Function   getReg
        \Brief      Retrieves a register
        \Details    Retrieves a register
        \Parameter  consts::R register
        \Returns    Register& reg
    */
    Register16& getReg(consts::R reg);

    /*
        \Function   rToI
        \Brief      Converts enum to index
        \Details    Converts enum to index
        \Parameter  consts::R register
        \Returns    unsigned int index
    */
    unsigned int rToI(consts::R reg);

    /*
        \Function   getSegOffsetPair
        \Brief      Returns the full segment:offset combination
        \Details    Returns the full segment:offset combination
        \Parameter  consts::R segment
        \Parameter  unsigned int offset
        \Returns    None
    */
    uint32_t getSegOffsetPair(consts::R segmentReg, unsigned int offsetVal);
    uint32_t getSegOffsetPair(consts::R segmentReg, consts::R offsetReg);

    /*
        \Function   execute
        \Brief      Executes an instruction
        \Details    Executes instruction and increments ticksToIdle
        \Parameter  None
        \Returns    None
    */
    void execute();

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

    /*
        \Function   isFaulted
        \Brief      Returns true if the CPU has faulted enough times to fail
        \Details    Takes into account any clock costs
        \Parameter  None
        \Returns    bool isFaulted
    */
    bool isFaulted();

}; // end class Intel86

} // end namespace intel86

} // end namespace i86

#endif