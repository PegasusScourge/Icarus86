#ifndef _i86consts_hpp
#define _i86consts_hpp

/*
    **> \File           I86Consts.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Constants for the Intel 8086
    **> \Details        None
    **> \Notes          None
*/

namespace i86 {

namespace intel86 {

// CPU Frequency
constexpr float FREQ_kHz = 0.1f; // Actual frequency we are running at
constexpr float FREQ_DESIGN_MIN_kHz = 5000.0f; // 5 MHz
constexpr float FREQ_DESING_MAX_kHz = 10000.0f; // 10 MHz

// CPU Registers
constexpr unsigned int NUM_REGISTERS = 14;
constexpr unsigned int R_AX = 0;
constexpr unsigned int R_BX = 1;
constexpr unsigned int R_CX = 2;
constexpr unsigned int R_DX = 3;
constexpr unsigned int R_SI = 4;
constexpr unsigned int R_DI = 5;
constexpr unsigned int R_SP = 6;
constexpr unsigned int R_BP = 7;
constexpr unsigned int R_IP = 8;
constexpr unsigned int R_CS = 9;
constexpr unsigned int R_DS = 10;
constexpr unsigned int R_ES = 11;
constexpr unsigned int R_SS = 12;
constexpr unsigned int R_FG = 13;

constexpr unsigned int FG_OF_BIT = 11;
constexpr unsigned int FG_DF_BIT = 10;
constexpr unsigned int FG_IF_BIT = 9;
constexpr unsigned int FG_TF_BIT = 8;
constexpr unsigned int FG_SF_BIT = 7;
constexpr unsigned int FG_ZF_BIT = 6;
constexpr unsigned int FG_AF_BIT = 4;
constexpr unsigned int FG_PF_BIT = 2;
constexpr unsigned int FG_CF_BIT = 0;

// Memory/IO ranges
constexpr unsigned int ADDRESS_SPACE_LEN = 0x100000;
constexpr unsigned int ADDRESS_SPACE_MAX_ADDR = 0xFFFFF;

constexpr unsigned int RESET_VECTOR_IP = 0x0;
constexpr unsigned int RESET_VECTOR_CS = 0xFFFF;

} // end namespace intel86

} // end namespace i86

#endif