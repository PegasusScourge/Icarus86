/*
    **> \File           Icarus86.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Main Entry Point
    **> \Details        None
    **> \Notes          None
*/

#include "Icarus86.hpp"
#include "Util/LogFile.hpp"
#include "ComputerGeneric/MemoryController.hpp"
#include "Intel8086/Intel86.hpp"
#include "Intel8086/I86Consts.hpp"

#include "SFML/System.hpp"

#include <iostream>
#include <cstddef>

int main(int argc, char* argv[]) {
    using namespace i86;

    Icarus86 icarus(argc, argv);

    icarus.go();
    return 0;
}

// ****************************************************************
// Class Icarus86
// ****************************************************************

i86::Icarus86::Icarus86(int argc, char* argv[]) {
    for (int i = 0; i < argc; i ++) {
        m_cmdArgs.push_back(argv[i]);
    }
}

void i86::Icarus86::go() {
    sf::Clock clockTest;

    i86::util::LogFile logger("icarus.log", true);

    i86::intel86::instruction::InstructionSet iSet("bin/8086.json");

    if (!iSet.isValid()) {
        logger.log_str("Invalid instruction set, terminating");
        return;
    }

    i86::compGeneric::MemoryController memory(0, i86::intel86::ADDRESS_SPACE_LEN);
    i86::compGeneric::MemoryController tmpIO(0, i86::intel86::ADDRESS_SPACE_LEN);

    i86::intel86::Intel86 cpu(memory, tmpIO, iSet);
}