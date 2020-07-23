/**************************************************************************

InstructionSet.cpp

Created by Matthew Clarke

Abract implementation of a instruction set

 **************************************************************************/

#include "InstructionSet.hpp"

#include "../../COutSys.hpp"

#include <fstream>

namespace ipi = icarus::processor::instruction;

/***********************************/
// class ICode
/***********************************/

ipi::ICode::ICode(uint8_t code, bool isPrefix, bool modRM, unsigned int dispBytes, unsigned int immBytes,
	unsigned int clockCost, std::vector<Microcode> microCode, std::vector<ICode> childCodes) {

	// Init everything
	m_code = code;
	m_isPrefix = isPrefix;
	m_hasModRM = modRM;
	m_microcode = microCode;
	m_childCodes = childCodes;
	m_displacementBytes = dispBytes;
	m_immediateBytes = immBytes;
	m_clockCost = clockCost;
}

bool ipi::ICode::isPrefix() {
	return m_isPrefix;
}

bool ipi::ICode::hasModRM() {
	return m_hasModRM;
}

unsigned int ipi::ICode::clockCost() {
	return m_clockCost;;
}

unsigned int ipi::ICode::numDisplacementBytes() {
	return m_displacementBytes;
}

unsigned int ipi::ICode::numImmediateBytes() {
	return m_immediateBytes;
}

uint8_t ipi::ICode::getCode() {
	return m_code;
}

std::vector<ipi::ICode>& ipi::ICode::getChildCodes() {
	return m_childCodes;
}
std::vector<ipi::Microcode>& ipi::ICode::getMicrocode() {
	return m_microcode;
}

/***********************************/
// class InstructionSet : member
/***********************************/

ipi::InstructionSet::InstructionSet() {
	m_name = "Default";
	m_filesrc = "Default";
	m_valid = false;
}

ipi::InstructionSet::InstructionSet(std::string filesrc) {
	m_filesrc = filesrc;
	parseJSON();
}

std::string ipi::InstructionSet::getName() {
	return m_name;
}

std::string ipi::InstructionSet::getFilesrc() {
	return m_filesrc;
}

bool ipi::InstructionSet::isValid() {
	return m_valid;
}

void ipi::InstructionSet::parseJSON() {
	using namespace nlohmann;

	icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': opening", icarus::COutSys::LEVEL_INFO);

	json j;
	std::ifstream stream(m_filesrc);
	if (!stream.is_open()) {
		// ERROR
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "' couldn't open file stream. Aborted", icarus::COutSys::LEVEL_ERR);
		m_valid = false;
		return;
	}

	// We are clear to continue
	icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': got file. Reading", icarus::COutSys::LEVEL_INFO);
	stream >> j;
	icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': read. Checking header", icarus::COutSys::LEVEL_INFO);
	
	// Check that we have the right header information
	if (!j["icarus"].is_boolean() && j["icarus"].get<bool>()) {
		// ERROR
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "' couldn't find header 'icarus' bool element. Aborted", icarus::COutSys::LEVEL_ERR);
		return;
	}

	// Now load the name
	if (j["name"].is_string()) {
		m_name = j["name"].get<std::string>();
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "' loaded. Name = " + m_name, icarus::COutSys::LEVEL_INFO);
	}
	else {
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "' couldn't find header 'name' string element. Aborted", icarus::COutSys::LEVEL_ERR);
		return;
	}

	if (!j["icode"].is_array()) {
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "' couldn't find header 'icode' array element. Aborted", icarus::COutSys::LEVEL_ERR);
		return;
	}

	icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "' icode found. Parsing", icarus::COutSys::LEVEL_INFO);
	// Now we must parse the instructions
	auto& iCodeNode = j["icode"];

	for (auto& iCode : iCodeNode) {
		parseICodeEntry(iCode);
	}
}

void ipi::InstructionSet::parseICodeEntry(nlohmann::json entry) {
	using namespace nlohmann;

	// Sanity check for the important things
	if (!entry["hexcode"].is_string()) {
		// Fail this entry
		return;
	}
	std::string code_s = entry["hexcode"].get<std::string>();
	icarus::COutSys::Print("iCode: hexcode_s=" + code_s + " ", icarus::COutSys::LEVEL_INFO);

	// Check for prefix
	if (entry["isPrefix"].is_boolean() && entry["isPrefix"].get<bool>()) {
		icarus::COutSys::Print("[ PREFIX] Checking for child codes: ");
		if (entry["childCodes"].is_array()) {
			icarus::COutSys::Println("[FOUND]. Parsing");
			for (auto& iCode : entry["childCodes"]) {
				parseICodeEntry(iCode);
			}
			icarus::COutSys::Println("Parsed all child codes", icarus::COutSys::LEVEL_INFO);
			return;
		}
		else {
			icarus::COutSys::Println("[NOT FOUND]. Ignoring");
			return;
		}
	}
	else {
		icarus::COutSys::Print("[!PREFIX] ");
	}

	icarus::COutSys::Println("[PARSED]");
}