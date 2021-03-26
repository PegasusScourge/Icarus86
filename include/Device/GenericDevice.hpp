#ifndef _genericdevice_hpp
#define _genericdevice_hpp

/*
    **> \File           GenericDevice.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-23
    **> \Brief          Header for GenericDevice.cpp
    **> \Details        None
    **> \Notes          None
*/

#include "nlohmann/json.hpp"
#include "ComputerGeneric/AddressRange.hpp"
#include "DeviceType.hpp"

#include <vector>
#include <map>
#include <string>
#include <cstdint>

namespace i86 {

namespace device {

// ****************************************************************
// Class GenericDevice
// Properties: None
// ****************************************************************

class GenericDevice {

private:

    i86::device::DeviceType m_type;

    std::vector<i86::compGeneric::AddressRange> m_rangesMemory;
    std::vector<i86::compGeneric::AddressRange> m_rangesIO;

    static size_t NextDeviceID;

    std::string m_humanName;
    size_t m_id;
    std::string m_serial;

    /*
        \Function   handleReadAddress
        \Brief      Handles reading an address. Override this
        \Details    Handles reading an address. Override this
        \Parameter  size_t addr
        \Returns    uint8_t value
    */
    uint8_t handleReadAddress(size_t addr) { return 0; }

    /*
        \Function   handleWriteAddress
        \Brief      Handles writing an address. Override this
        \Details    Handles writing an address. Override this
        \Parameter  size_t addr
        \Parameter  uint8_t byte
        \Returns    uint8_t value
    */
    void handleWriteAddress(size_t addr, uint8_t byte) {}


public:
    static size_t GetNextDeviceID();

    /*
        \Function   GenericDevice
        \Brief      Constructor
        \Details    Constructor
        \Parameter  size_t id
        \Parameter  nlohmann::json& deviceConfig
        \Returns    None
    */
    GenericDevice(size_t id, nlohmann::json& deviceConfig);

    /*
        \Function   getRangesForRegistration
        \Brief      Returns a vector of ranges for registration in an AddressSpace
        \Details    Returns a vector of ranges for registration in an AddressSpace
        \Parameter  None
        \Returns    std::vector<i86::compGeneric::AddressRange>& ranges
    */
    std::vector<i86::compGeneric::AddressRange>& getRangesForRegistration_mem();
    std::vector<i86::compGeneric::AddressRange>& getRangesForRegistration_io();

    /*
        \Function   getHumanName
        \Brief      Returns the human name of the device
        \Details    Returns the human name of the device
        \Parameter  None
        \Returns    std::string humanName
    */
    std::string getHumanName();

    /*
        \Function   getID
        \Brief      Returns the ID of the device
        \Details    Returns the ID of the device
        \Parameter  None
        \Returns    size_t id
    */
    size_t getID();

    /*
        \Function   getType
        \Brief      Returns the type of device
        \Details    Returns the type of device
        \Parameter  None
        \Returns    DeviceType t
    */
    i86::device::DeviceType getType();

    /*
        \Function   readAddress
        \Brief      Handles reading an address.
        \Details    Handles reading an address.
        \Parameter  size_t addr
        \Returns    uint8_t value
    */
    uint8_t readAddress(size_t addr);

    /*
        \Function   writeAddress
        \Brief      Handles writing an address.
        \Details    Handles writing an address.
        \Parameter  size_t addr
        \Parameter  uint8_t byte
        \Returns    uint8_t value
    */
    void writeAddress(size_t addr, uint8_t byte);

}; // end class GenericDevice

} // end namespace device

} // end namespace i86

#endif