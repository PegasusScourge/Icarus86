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

#include "nlohmann/json.hpp"
#include "Device/GenericDevice.hpp"

#include <vector>
#include <string>
#include <memory>

namespace i86 {

// ****************************************************************
// Class Icarus86
// Properties: None
// ****************************************************************

class Icarus86 {

private:
    static std::string ISetPath;
    static std::string DevicesCfgPath;

    std::vector<std::string> m_cmdArgs;

    nlohmann::json m_deviceConfig;

    /*
        \Function   buildDeviceConfig
        \Brief      Constructs device config
        \Details    Constructs device config
        \Parameter  None
        \Returns    bool success
    */
    bool buildDeviceConfig();

    /*
        \Function   validateDevice
        \Brief      Validates device
        \Details    Validates device
        \Parameter  nlohmann::json& device
        \Returns    bool valid
    */
    bool validateDevice(nlohmann::json& device);

    /*
        \Function   constructDevices
        \Brief      Constructs devices
        \Details    Constructs devices
        \Parameter  std::vector<std::shared_ptr<i86::device::GenericDevice>>& devices
        \Returns    bool success
    */
    bool constructDevices(std::vector<std::shared_ptr<i86::device::GenericDevice>>& devices);

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