#ifndef _currentinstr_hpp
#define _currentinstr_hpp

/*
    **> \File           CurrentInstruction.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          CurrentInstruction struct
    **> \Details        None
    **> \Notes          None
*/

#include "Intel8086/Instruction/Microcode.hpp"
#include "Intel8086/Instruction/ModRMByte.hpp"
#include "Intel8086/I86Consts.hpp"

#include <vector>

namespace i86 {

namespace intel86 {

namespace instruction {

class CurrentInstruction {
public:
    // List of microcode instructions to execute
    std::vector<i86::intel86::instruction::Microcode> microcode;
    // Current ModRMByte
    i86::intel86::instruction::ModRMByte modRMByte;
    // The number of immediate bytes the instruction has
    uint8_t numImmeditateBytes;
    // The number of displacement bytes the instruction has
    uint8_t numDisplacementBytes;
    // Current displacement value
    uint16_t displacement;
    // Current immediate value
    uint16_t immediate;
    // Hex code of the instruction
    uint8_t code;
    // Segment register information
    bool hasSegOverride;
    bool usedSegOverride;
    i86::intel86::consts::R segOverride; // Segement override enum

    // Information for the current microcode execution
    struct MicrocodeInformation {
        // If true, interpret registers/memory locations as 8 bit. False = 16 bit
        bool bitMode8Bit;
        // If true srcA is in use and new src selection should use srcB
        bool srcAUsed;
        // If true destination writes from dst are allowed
        bool dstEnabled;
        // Src and dst holdings
        struct Cache {
            uint8_t bytes;
            uint16_t v;
        } srcA, srcB, dst;
    } mCodeI;
}; // end class CurrentInstruction

} // end namespace instruction

} // end namespace intel86

} // end namespace i86

#endif