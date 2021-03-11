#ifndef _modrm_hpp
#define _modrm_hpp

/*
    **> \File           ModRMByte.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-11
    **> \Brief          Header for ModRMByte.cpp
    **> \Details        None
    **> \Notes          None
*/


#include <cstdint>

namespace i86 {

namespace intel86 {

namespace instruction {

class ModRMByte {
public:
	ModRMByte(uint8_t b);
	ModRMByte();

	uint8_t byte();
	uint8_t MOD();
	uint8_t REGOP();
	uint8_t RM();

	void setByte(uint8_t b);

private:
	uint8_t m_byte;
	uint8_t m_MOD : 2;
	uint8_t m_REGOP : 3;
	uint8_t m_RM : 3;
};

}

}

}

#endif