#ifndef _icarus86_hpp
#define _icarus86_hpp

/*
    **> \File           Icarus86.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Header for Icarus86.cpp
    **> \Details        None
    **> \Notes          None
*/

#include <vector>
#include <string>

namespace i86 {

// ****************************************************************
// Class Icarus86
// Properties: None
// ****************************************************************

class Icarus86 {

private:
    std::vector<std::string> m_cmdArgs;

public:
    /*
        \Function   Icarus86
        \Brief      Constructs Icarus86
        \Details    Constructs Icarus86
        \Parameter  int numArgs
        \Parameter  char* argv[]
        \Returns    None
    */
    Icarus86(int numArgs, char* argv[]);

    /*
        \Function   go
        \Brief      Runs Icarus86
        \Details    Runs Icarus86
        \Parameter  None
        \Returns    None
    */
    void go();

}; // end class Icarus86

} // end namespace i86

#endif