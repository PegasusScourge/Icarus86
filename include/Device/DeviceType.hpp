#ifndef _devicetype_hpp
#define _devicetype_hpp

/*
    **> \File           DeviceType.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-24
    **> \Brief          A device type
    **> \Details        None
    **> \Notes          None
*/

#include <cstdint>
#include <string>

namespace i86 {

namespace device {

// ****************************************************************
// Enum Class DeviceType
// Properties: None
// ****************************************************************

enum class DeviceType {

    ERROR,
    GENERIC,
    ROM_CHIP,
    RAM_CHIP

}; // end enum class DeviceType

// ****************************************************************
// DeviceType conversion functions
// Properties: None
// ****************************************************************

inline DeviceType GetDeviceTypeFromString(std::string type) {
    if (type == "ROM_CHIP") {
        return DeviceType::ROM_CHIP;
    }
    if (type == "RAM_CHIP") {
        return DeviceType::RAM_CHIP;
    }
    if (type == "GENERIC") {
        return DeviceType::GENERIC;
    }
    return DeviceType::ERROR;
}

inline std::string GetDeviceTypeString(DeviceType type) {
    switch (type) {
    case DeviceType::ROM_CHIP: return "ROM_CHIP";
    case DeviceType::RAM_CHIP: return "RAM_CHIP";
    case DeviceType::GENERIC: return "GENERIC";
    case DeviceType::ERROR: return "ERROR";
    }
    return "ERROR";
}

} // end namespace device

} // end namespace i86

#endif