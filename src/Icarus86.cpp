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

#include <iostream>

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
    for (size_t i = 0; i < argc; i ++) {
        m_cmdArgs.push_back(argv[i]);
    }
}

void i86::Icarus86::go() {
    i86::util::LogFile logger("hello.txt");

    std::cout << "Hello World!" << std::endl;

    logger.log_rawString("Hello World!");
}