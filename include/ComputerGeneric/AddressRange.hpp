#ifndef _addressrange_hpp
#define _addressrange_hpp

/*
    **> \File           AddressRange.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Header for AddressRange.cpp
    **> \Details        AddressRange
    **> \Notes          None
*/

#include "Device/GenericDevice.hpp"

#include <cstdint>
#include <cstddef>
#include <string>

namespace i86 {

namespace compGeneric {

// ****************************************************************
// Class AddressRange
// Properties: Thread safe (lock_guard)
// ****************************************************************

class AddressRange {

public:
    size_t start;
    size_t range;
    bool readEnable;
    bool writeEnable;
    std::string name;

    inline AddressRange(std::string n, size_t s = 0, size_t r = 0) {
        start = s;
        range = r;
        readEnable = false;
        writeEnable = false;
        name = n;
    }

    inline bool isInThisRange(i86::compGeneric::AddressRange r2) {
        bool res = false;
        if (r2.start >= start && r2.start < start + range) {
            if (r2.range < (start + range) - r2.start) {
                res = true;
            }
        }
        return res;
    }

    inline bool isInThisRange(size_t v) {
        if (v >= start && v < (start + range)) {
            return true;
        }
        return false;
    }

}; // end class AddressRange

} // end namespace compGeneric

} // end namespace i86

#endif