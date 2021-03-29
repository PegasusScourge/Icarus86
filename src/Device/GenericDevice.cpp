/*
    **> \File           GenericDevice.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-23
    **> \Brief          A GenericDevice for an address space
    **> \Details        A GenericDevice for an address space
    **> \Notes          None
*/

#include "Device/GenericDevice.hpp"
#include "Util/LogFile.hpp"



// ****************************************************************
// Class GenericDevice
// ****************************************************************

size_t i86::device::GenericDevice::NextDeviceID = 0;

i86::device::GenericDevice::GenericDevice(size_t id, nlohmann::json& deviceConfig) {
    /*

    Required fields in JSON
    This should be verified before construction (except memoryspace and iospace array content)

    device
        - human <string>
        - serial <string>
        - type <string>
        - memoryspace []
            - startAddress <uint>
            - range <uint>
            - name <string>
            - write <bool>
            - read <bool>
        - iospace []
            - startAddress <uint>
            - range <uint>
            - name <string>
            - write <bool>
            - read <bool>

    */
    m_id = id;
    m_humanName = deviceConfig["human"].get<std::string>();
    m_type = i86::device::GetDeviceTypeFromString(deviceConfig["type"].get<std::string>());
    m_serial = deviceConfig["serial"].get<std::string>();

    // Check to see if we can declare ranges
    auto& memRanges = deviceConfig["memoryspace"];
    auto& ioRanges = deviceConfig["iospace"];

    /*
        MEMORY RANGES
    */
    for (auto& memRange : memRanges) {
        if (!memRange["startAddress"].is_number_unsigned() || !memRange["range"].is_number_unsigned() || !memRange["name"].is_string() || 
            !memRange["write"].is_boolean() || !memRange["read"].is_boolean()) {
            
            i86::util::LogFile::Generic.log_str("Warning: Device '" + m_humanName + "' has invalid memoryspace declaration");
            continue;
        }

        size_t start = memRange["startAddress"].get<size_t>();
        size_t range = memRange["range"].get<size_t>();
        std::string n = memRange["name"].get<std::string>();
        i86::compGeneric::AddressRange r = i86::compGeneric::AddressRange(n, start, range);
        r.readEnable = memRange["read"].get<bool>();
        r.writeEnable = memRange["write"].get<bool>();

        i86::util::LogFile::Generic.log_str("Device '" + m_humanName + "' memoryspace declaration configured");

        m_rangesMemory.push_back(r);
    }

    /*
        IO RANGES
    */
    for (auto& ioRange : ioRanges) {
        if (!ioRange["startAddress"].is_number_unsigned() || !ioRange["range"].is_number_unsigned() || !ioRange["name"].is_string() || 
            !ioRange["write"].is_boolean() || !ioRange["read"].is_boolean()) {
            
            i86::util::LogFile::Generic.log_str("Warning: Device '" + m_humanName + "' has invalid iospace declaration");
            continue;
        }

        size_t start = ioRange["startAddress"].get<size_t>();
        size_t range = ioRange["range"].get<size_t>();
        std::string n = ioRange["name"].get<std::string>();
        i86::compGeneric::AddressRange r = i86::compGeneric::AddressRange(n, start, range);
        r.readEnable = ioRange["read"].get<bool>();
        r.writeEnable = ioRange["write"].get<bool>();

        i86::util::LogFile::Generic.log_str("Device '" + m_humanName + "' iospace declaration configured");

        m_rangesIO.push_back(r);
    }

    // Anything else?
}

std::vector<i86::compGeneric::AddressRange>& i86::device::GenericDevice::getRangesForRegistration_mem() {
    return m_rangesMemory;
}

std::vector<i86::compGeneric::AddressRange>& i86::device::GenericDevice::getRangesForRegistration_io() {
    return m_rangesIO;
}

std::string i86::device::GenericDevice::getHumanName() {
    return m_humanName;
}

size_t i86::device::GenericDevice::getID() {
    return m_id;
}

i86::device::DeviceType i86::device::GenericDevice::getType() {
    return m_type;
}

uint8_t i86::device::GenericDevice::readAddress(bool isIO, size_t addr) {
    std::tuple<std::string, size_t> rangeInfo = getRangeAndOffset(isIO, addr);
    return handleReadAddress(isIO, std::get<0>(rangeInfo), std::get<1>(rangeInfo));
}

void i86::device::GenericDevice::writeAddress(bool isIO, size_t addr, uint8_t byte) {
    std::tuple<std::string, size_t> rangeInfo = getRangeAndOffset(isIO, addr);
    handleWriteAddress(isIO, std::get<0>(rangeInfo), std::get<1>(rangeInfo), byte);
}

std::tuple<std::string, size_t> i86::device::GenericDevice::getRangeAndOffset(bool isIO, size_t addr) {
    std::vector<i86::compGeneric::AddressRange>& searchVector = isIO ? m_rangesIO : m_rangesIO;
    i86::compGeneric::AddressRange* range = nullptr;

    for (size_t i = 0; i < searchVector.size(); i++) {
        if (searchVector[i].isInThisRange(addr)) {
            range = &searchVector[i];
            break;
        }
    }

    if (range == nullptr) {
        i86::util::LogFile::Generic.log_str("Attempted to find matching range for addr " + 
                std::to_string(addr) + (isIO ? " [ IO] " : " [!IO] ") + " but failed! (Device " + m_humanName + ")");
        return std::tuple<std::string, size_t>("__ERROR__", 0);
    }

    return std::tuple<std::string, size_t>(range->name, addr - range->start);
}

size_t i86::device::GenericDevice::GetNextDeviceID() {
    size_t id = i86::device::GenericDevice::NextDeviceID;
    i86::device::GenericDevice::NextDeviceID++;
    return id;
}