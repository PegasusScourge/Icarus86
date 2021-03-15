/*
    **> \File           Intel86.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          The Intel 8086 CPU
    **> \Details        None
    **> \Notes          None
*/

#include "Intel8086/Intel86.hpp"
#include "Util/LogFile.hpp"
#include "Util/HexUtil.hpp"

namespace gc = i86::compGeneric;
namespace consts = i86::intel86::consts;

inline i86::util::LogFile Intel86Log("Intel86.log");

// ****************************************************************
// Class Intel86: Public
// ****************************************************************

i86::intel86::Intel86::Intel86(gc::Addressable& memorySpace, gc::Addressable& ioSpace, i86::intel86::instruction::InstructionSet& iSet) : 
    m_memorySpace {memorySpace}, m_ioSpace {ioSpace}, m_instructions {iSet} {
    
    reset();
}

void i86::intel86::Intel86::reset() {
    getReg(consts::R::IP).X = consts::RESET_VECTOR_IP;
    getReg(consts::R::CS).X = consts::RESET_VECTOR_CS;

    accumulator = 0;
    m_ticksToIdle = 0;

    m_isFaulted = false;
    m_numFaults = 0;

    Intel86Log.strs() << "Reset | IP: " << i86::util::NumToHexStr(getReg(consts::R::IP).X);
    Intel86Log.strs() << " CS: " << i86::util::NumToHexStr(getReg(consts::R::CS).X);
    Intel86Log.flushss();
}

void i86::intel86::Intel86::tick() {
    if (m_isFaulted) {
        fault("Already faulted MAX times!");
        return;
    }
    
    accumulator += m_timingClock.getElapsedTime().asMilliseconds();
    m_timingClock.restart();

    while (accumulator > consts::FREQ_MS_PER_CLOCK) {
        accumulator -= consts::FREQ_MS_PER_CLOCK;

        if (m_ticksToIdle > 0) {
            m_ticksToIdle--;
        }
        else {
            execute();
        }
    }
}

bool i86::intel86::Intel86::isFaulted() {
    return m_isFaulted;
}

// ****************************************************************
// Class Intel86: Private
// ****************************************************************

void i86::intel86::Intel86::fault(std::string reason) {
    m_numFaults++;

    Intel86Log.log_str("@@@@@@@@@@@@@@@@@@@@@@@@");
    Intel86Log.log_str("CPU Fault " + std::to_string(m_numFaults) + ":");
    Intel86Log.log_str(reason);
    Intel86Log.log_str("@@@@@@@@@@@@@@@@@@@@@@@@");

    if (m_numFaults >= NUM_MAX_FAULTS) {
        m_isFaulted = true;
    }
}

i86::intel86::Register16& i86::intel86::Intel86::getReg(consts::R reg) {
    unsigned int r = rToI(reg);
    if (r > consts::NUM_REGISTERS) {
        return m_registers[0];
    }
    return m_registers[r];
}

unsigned int i86::intel86::Intel86::rToI(consts::R reg) {
    unsigned int seg = 999;
    switch (reg) {
        case consts::R::AX: seg = 0; break;
        case consts::R::BX: seg = 1; break;
        case consts::R::CX: seg = 2; break;
        case consts::R::DX: seg = 3; break;
        case consts::R::SI: seg = 4; break;
        case consts::R::DI: seg = 5; break;
        case consts::R::SP: seg = 6; break;
        case consts::R::BP: seg = 7; break;
        case consts::R::IP: seg = 8; break;
        case consts::R::CS: seg = 9; break;
        case consts::R::DS: seg = 10; break;
        case consts::R::ES: seg = 11; break;
        case consts::R::SS: seg = 12; break;
        case consts::R::FG: seg = 13; break;
    }
    return seg;
}

uint32_t i86::intel86::Intel86::getSegOffsetPair(consts::R segmentReg, unsigned int offsetVal) {
    unsigned int seg = rToI(segmentReg);
    if (seg > consts::NUM_REGISTERS || offsetVal > consts::NUM_REGISTERS) {
        return 0;
    }
    uint32_t pair = (m_registers[seg].X << 4) + m_registers[offsetVal].X;
    return pair;
}

uint32_t i86::intel86::Intel86::getSegOffsetPair(consts::R segmentReg, consts::R offsetReg) {
    unsigned int seg = rToI(segmentReg);
    unsigned int off = rToI(offsetReg);
    if (seg > consts::NUM_REGISTERS || off > consts::NUM_REGISTERS) {
        return 0;
    }
    uint32_t pair = (m_registers[seg].X << 4) + m_registers[off].X;
    return pair;
}

void i86::intel86::Intel86::execute() {
    uint8_t code;

    uint32_t baseIP = getReg(consts::R::IP).X;
    uint32_t ipInc = 0;

    if (!m_memorySpace.readByte(getSegOffsetPair(consts::R::CS, baseIP + ipInc), code)) {
        fault("Unable to read byte from memory space at CS:IP (" + i86::util::NumToHexStr(getReg(consts::R::CS).X) + ":" + i86::util::NumToHexStr(getReg(consts::R::IP).X) + ")");
        return;
    }

    i86::intel86::instruction::ICode instr = m_instructions[code];

    while (instr.isValid() && instr.isPrefix()) {
        if (!m_memorySpace.readByte(getSegOffsetPair(consts::R::CS, baseIP + (++ipInc)), code)) {
            fault("Unable to read byte from memory space at CS:IP (" + i86::util::NumToHexStr(getReg(consts::R::CS).X) + ":" + i86::util::NumToHexStr(getReg(consts::R::IP).X) + ")");
            return;
        }

        instr = instr[code];
    }

    if (!instr.isValid()) {
        fault("instr invalid");
        return;
    }

    // We have a valid instruction!
    primeCurrentInstruction(instr, baseIP, ipInc);
}

void i86::intel86::Intel86::primeCurrentInstruction(i86::intel86::instruction::ICode& instr, uint32_t baseIP, uint32_t& ipInc) {

}
