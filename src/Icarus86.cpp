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
#include "Intel8086/Intel86.hpp"
#include "Intel8086/I86Consts.hpp"

#include <iostream>
#include <cstddef>

int main(int argc, char* argv[]) {
    using namespace i86;

    std::cout << "Icarus86 START" << std::endl;

    Icarus86 icarus(argc, argv);

    icarus.go();

    std::cout << "Icarus86 END" << std::endl;
    return 0;
}

// ****************************************************************
// Class Icarus86: Public
// ****************************************************************

std::string i86::Icarus86::ISetPath = "bin/8086.json";
std::string i86::Icarus86::DevicesCfgPath = "bin/devices.json";

i86::Icarus86::Icarus86(int argc, char* argv[]) {
    for (int i = 0; i < argc; i ++) {
        m_cmdArgs.push_back(argv[i]);
    }
}

void i86::Icarus86::go() {
    i86::util::LogFile::Generic.log_str("Constructing instruction set");
    i86::intel86::instruction::InstructionSet iSet(ISetPath);

    if (!iSet.isValid()) {
        i86::util::LogFile::Generic.log_str("Invalid instruction set, terminating");
        return;
    }

    i86::util::LogFile::Generic.log_str("Constructing address spaces");
    i86::compGeneric::AddressSpace memory(i86::compGeneric::AddressRange(0, i86::intel86::consts::ADDRESS_SPACE_LEN));
    i86::compGeneric::AddressSpace io(i86::compGeneric::AddressRange(0, i86::intel86::consts::ADDRESS_SPACE_LEN));
    
    i86::util::LogFile::Generic.log_str("Reading device configuration");

    if (!buildDeviceConfig()) {
        i86::util::LogFile::Generic.log_str("Invalid device config, terminating");
        return;
    }

    i86::util::LogFile::Generic.log_str("Constructing devices");
    std::vector<std::shared_ptr<i86::device::GenericDevice>> devices;

    if (!constructDevices(devices)) {
        i86::util::LogFile::Generic.log_str("Failed to construct devices, terminating");
        return;
    }

    i86::util::LogFile::Generic.log_str("Registering devices");

    // Register


    i86::util::LogFile::Generic.log_str("Constructing CPU");

    i86::intel86::Intel86 cpu(memory, io, iSet);

    // Initiate computation
}

bool i86::Icarus86::buildDeviceConfig() {
    using namespace nlohmann;

	std::ifstream stream(DevicesCfgPath);
	if (!stream.is_open()) {
		// ERROR
        i86::util::LogFile::Generic.log_str("Device config file '" + DevicesCfgPath + "' not found");
		return false;
	}

    m_deviceConfig = json();
    stream >> m_deviceConfig;

    // Load complete
    // Validate the json
    if (!m_deviceConfig["icarus"].is_boolean() && m_deviceConfig["icarus"].get<bool>()) {
		// ERROR
		i86::util::LogFile::Generic.log_str("No icarus header bool field in device config (" + DevicesCfgPath + ")!");
		return false;
	}

    if (!m_deviceConfig["devices"].is_array()) {
		i86::util::LogFile::Generic.log_str("No devices array field in device config (" + DevicesCfgPath + ")!");
		return false;
	}

    // Validated
    return true;
}

bool i86::Icarus86::validateDevice(nlohmann::json& device) {
    if (!device["human"].is_string()) {
        i86::util::LogFile::Generic.log_str("Device config field error: No field of type <string> of name 'human'");
        return false;
    }

    if (!device["serial"].is_string()) {
        i86::util::LogFile::Generic.log_str("Device config field error: No field of type <string> of name 'serial'");
        return false;
    }

    if (!device["type"].is_string()) {
        i86::util::LogFile::Generic.log_str("Device config field error: No field of type <string> of name 'type'");
        return false;
    }

    // Check the type
    std::string deviceType = device["type"].get<std::string>();
    i86::device::DeviceType type = i86::device::GetDeviceTypeFromString(deviceType);
    if (type == i86::device::DeviceType::ERROR) {
        i86::util::LogFile::Generic.log_str("Device 'type' field error: Invalid type '" + deviceType + "'");
        return false;
    }

    if (!device["memoryspace"].is_array()) {
        i86::util::LogFile::Generic.log_str("Device config field error: No field of type <object> of name 'memoryspace'");
        return false;
    }

    if (!device["iospace"].is_array()) {
        i86::util::LogFile::Generic.log_str("Device config field error: No field of type <object> of name 'iospace'");
        return false;
    }

    return true;
}

bool i86::Icarus86::constructDevices(std::vector<std::shared_ptr<i86::device::GenericDevice>>& devices) {
    auto& devicesArray = m_deviceConfig["devices"];

    if (devicesArray.size() == 0) {
        i86::util::LogFile::Generic.log_str("There are no devices declared in '" + DevicesCfgPath + "'");
        return true;
    }

    size_t i = 0;
    for (auto& device : devicesArray) {
        // Check device has correct information
        if (!validateDevice(device)) {
            i86::util::LogFile::Generic.log_str("Invalid device!");
            if (device["human"].is_string()) {
                i86::util::LogFile::Generic.log_str("Device with human name '" + device["human"].get<std::string>() + "' invalid parameters");
            }
            i86::util::LogFile::Generic.log_str("Device index: " + std::to_string(i));
            return false;
        }

        std::shared_ptr<i86::device::GenericDevice> newDevice;

        i86::device::DeviceType type = i86::device::GetDeviceTypeFromString(device["type"].get<std::string>());

        switch (type) {
        case i86::device::DeviceType::ROM_CHIP:
        case i86::device::DeviceType::RAM_CHIP:
        case i86::device::DeviceType::GENERIC:
            newDevice = std::make_shared<i86::device::GenericDevice>(i86::device::GenericDevice(i86::device::GenericDevice::GetNextDeviceID(), device));
            break;
        default:
            i86::util::LogFile::Generic.log_str("Invalid device!");
            return false;
            break;
        }

        i86::util::LogFile::Generic.log_str("Device constucted, index=" + std::to_string(i) +
            ", id=" + std::to_string(newDevice->getID()) + ", human=" + newDevice->getHumanName() +
            ", type=" + device["type"].get<std::string>());

        i++;
    }

    return true;
}