#ifndef _hexutil_hpp
#define _hexutil_hpp

/*
    **> \File           HexUtil.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-11
    **> \Brief          Header for HexUtil.cpp
    **> \Details        Utilities for hex strings and numbers
    **> \Notes          None
*/

#include <string>
#include <sstream>
#include <cstdint>
#include <cstddef>
#include <iomanip>

namespace i86 {

namespace util {

template <class T>
T HexStrToNum(std::string hexStr) {
    size_t number;
    std::stringstream s;
    s << std::hex << hexStr;
    s >> number;
    return (T)number;
}

template <class T>
std::string NumToHexStr(T num, bool prefix = false) {
    std::stringstream s;
    if (prefix)
        s << "0x";
    s << std::setfill ('0') << std::setw(sizeof(T)*2) << std::hex << std::uppercase << num;
    return s.str();
}

} // end namespace util

} // end namespace i86

#endif