/**************************************************************************

Icarus86.cpp

Created by Matthew Clarke

Entry point to the emulator

 **************************************************************************/

#include "Icarus86.hpp"
#include "Constexprs.hpp"
#include "COutSys.hpp"

#include <iostream>

using std::cout;
using icarus::endl; // Use our endl so we don't do constant flushes to cout. This line also simplifies changing to the flushing varient if necessary.

namespace i = icarus;

int main(int argc, char* argv[]) {

	// Compile the command line arguments into strings to pass to the emulator
	std::vector<std::string> cmdArgs;
	for (int i = 0; i < argc; i++) {
		cmdArgs.push_back(std::string(argv[i]));
	}

	// For now we do raw cout writes. We will start the console output system soon enough
	cout << "Icarus86 emulator start up. Cmd args:" << endl;
	for (auto& s : cmdArgs) {
		cout << " - " << s << endl;
	}
	cout << endl << "Starting emulation:" << endl;

	// Initialize the console output system
	i::COutSys::Initialize(false);
	i::COutSys::Println("COutSys initialized", i::COutSys::LEVEL_INFO);
	
	// Start the emulator
	i::Icarus86 icarus;

	// Check for initilization failures
	if (icarus.failure())
		return icarus.getReturnValue();

	icarus.run();
	return icarus.getReturnValue();
}

/***********************************/
// class Icarus
/***********************************/

i::Icarus86::Icarus86() {
	m_intialized = true;
	i::COutSys::Println("Icarus86 initialized", i::COutSys::LEVEL_INFO);

	i::COutSys::Println("Data bus width: " + std::to_string(m_dataBus.getBitWidth()), i::COutSys::LEVEL_INFO);
	i::COutSys::Println("Address bus width: " + std::to_string(m_addressBus.getBitWidth()), i::COutSys::LEVEL_INFO);
}

void i::Icarus86::run() {
	i::COutSys::Println("Icarus86 running", i::COutSys::LEVEL_INFO);


}

// Setters


// Getters

int i::Icarus86::getReturnValue() {
	return m_returnValue;
}

bool i::Icarus86::failure() {
	return m_hasErrored || !m_intialized;
}