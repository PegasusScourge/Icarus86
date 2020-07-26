/**************************************************************************

InstructionSet.cpp

Created by Matthew Clarke

Abract implementation of a instruction set

 **************************************************************************/

#include "InstructionSet.hpp"

#include "../../COutSys.hpp"

#include <fstream>
#include <sstream>

namespace ipi = icarus::processor::instruction;

/***********************************/
// class ICode
/***********************************/

ipi::ICode::ICode() {
	// Init everything
	m_code = 0;
	m_isPrefix = false;
	m_hasModRM = false;
	m_displacementBytes = 0;
	m_immediateBytes = 0;
	m_clockCost = 0;
}

ipi::ICode::ICode(nlohmann::json entry, uint64_t prefix) {
	parseICodeEntry(entry, prefix);
}

void ipi::ICode::parseICodeEntry(nlohmann::json entry, uint64_t prefix) {
	using namespace nlohmann;

	// Sanity check for the important things
	if (!entry["hexcode"].is_string()) {
		// Fail this entry
		return;
	}
	std::string code_s = entry["hexcode"].get<std::string>();
	icarus::COutSys::Print("iCode: hexcode_s=" + code_s + " ", icarus::COutSys::LEVEL_INFO);

	// Convert the code to hex number
	size_t code;
	std::stringstream ss;
	ss << std::hex << code_s;
	ss >> code;
	m_code = (uint8_t)code;
	icarus::COutSys::Print("(#=" + std::to_string(m_code) + ")\t\t");

	// Check for prefix
	if (entry["isPrefix"].is_boolean() && entry["isPrefix"].get<bool>()) {
		m_isPrefix = true;
		icarus::COutSys::Print("[ PREFIX] Checking for child codes: ");
		if (entry["childCodes"].is_array()) {
			uint64_t newPrefix = (prefix << 8) & m_code;
			icarus::COutSys::Println("[FOUND]. Parsing (newPrefix=" + icarus::COutSys::ToHexStr(newPrefix) + ")");
			icarus::COutSys::Println("iCode: <children>", icarus::COutSys::LEVEL_INFO);
			for (auto& iCode : entry["childCodes"]) {
				m_childCodes.push_back(ICode(iCode, newPrefix));
			}
			icarus::COutSys::Println("iCode: </children>", icarus::COutSys::LEVEL_INFO);
			m_valid = true;
			return;
		}
		else {
			icarus::COutSys::Println("[NOT FOUND]. Ignoring");
		}
	}
	else {
		icarus::COutSys::Print("[!PREFIX] ");
		m_isPrefix = false;
	}

	// Gather other information
	if (entry["hasModRM"].is_boolean()) {
		m_hasModRM = entry["hasModRM"].get<bool>();
		icarus::COutSys::Print(m_hasModRM ? "[ MODRM] " : "[!MODRM] ");
	}
	else {
		m_hasModRM = false;
	}

	if (entry["immediateBytes"].is_number_unsigned()) {
		m_immediateBytes = entry["immediateBytes"].get<unsigned int>();
		icarus::COutSys::Print("[IMM: " + std::to_string(m_immediateBytes) + "] ");
	}
	else {
		m_immediateBytes = 0;
	}

	if (entry["displacementBytes"].is_number_unsigned()) {
		m_displacementBytes = entry["displacementBytes"].get<unsigned int>();
		icarus::COutSys::Print("[DISP: " + std::to_string(m_displacementBytes) + "] ");
	}
	else {
		m_displacementBytes = 0;
	}
		
	if (entry["clockCost"].is_number_unsigned()) {
		m_clockCost = entry["clockCost"].get<unsigned int>();
		icarus::COutSys::Print("[CLK#: " + std::to_string(m_clockCost) + "] ");
	}	
	else {
		m_clockCost = 0;
	}

	// Get microcode
	if (entry["microcode"].is_array()) {
		// We have microcode
		unsigned int microcodeAdded = 0;
		for (auto& element : entry["microcode"]) {
			if (!element.is_string())
				continue;
			std::string s = element.get<std::string>();
			Microcode::MicrocodeType type = Microcode::GetTypeFromString(s);
			microcodeAdded++;
			m_microcode.push_back(Microcode(s, type));
		}

		icarus::COutSys::Print("[MCODE#: " + std::to_string(microcodeAdded) + "] ");
		if (microcodeAdded == 0) {
			icarus::COutSys::Println("[FAIL: MCODE len = 0]");
			return;
		}
	}
	else {
		icarus::COutSys::Println("[FAIL: MCODE missing]");
		return;
	}

	icarus::COutSys::Println("[PARSED]");
	m_valid = true;
}

bool ipi::ICode::isPrefix() {
	return m_isPrefix;
}

bool ipi::ICode::hasModRM() {
	return m_hasModRM;
}

bool ipi::ICode::isValid() {
	return m_valid;
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

uint64_t ipi::ICode::getPrefix() {
	return m_prefix;
}

std::vector<ipi::ICode>& ipi::ICode::getChildCodes() {
	return m_childCodes;
}
std::vector<ipi::Microcode>& ipi::ICode::getMicrocode() {
	return m_microcode;
}

ipi::ICode& ipi::ICode::operator[](uint8_t code) {
	for (auto& i : m_childCodes) {
		if (i.getCode() == code)
			return i;
	}
	return *this;
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

	icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': Opening", icarus::COutSys::LEVEL_INFO);

	json j;
	std::ifstream stream(m_filesrc);
	if (!stream.is_open()) {
		// ERROR
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': Couldn't open file stream. Aborted", icarus::COutSys::LEVEL_ERR);
		m_valid = false;
		return;
	}

	// We are clear to continue
	icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': Got file. Reading", icarus::COutSys::LEVEL_INFO);
	stream >> j;
	icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': Read. Checking header", icarus::COutSys::LEVEL_INFO);
	
	// Check that we have the right header information
	if (!j["icarus"].is_boolean() && j["icarus"].get<bool>()) {
		// ERROR
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': Couldn't find header 'icarus' bool element. Aborted", icarus::COutSys::LEVEL_ERR);
		return;
	}

	// Now load the name
	if (j["name"].is_string()) {
		m_name = j["name"].get<std::string>();
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': Loaded. Name = " + m_name, icarus::COutSys::LEVEL_INFO);
	}
	else {
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': Couldn't find header 'name' string element. Aborted", icarus::COutSys::LEVEL_ERR);
		return;
	}

	if (!j["icode"].is_array()) {
		icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': Couldn't find header 'icode' array element. Aborted", icarus::COutSys::LEVEL_ERR);
		return;
	}

	icarus::COutSys::Println("InstructionSet from file '" + m_filesrc + "': iCode found! Parsing...", icarus::COutSys::LEVEL_INFO);
	// Now we must parse the instructions
	auto& iCodeNode = j["icode"];

	for (auto& iCode : iCodeNode) {
		m_iCodes.push_back(ICode(iCode, 0));
	}
}

ipi::ICode& ipi::InstructionSet::operator[](uint8_t code) {
	for (auto& i : m_iCodes) {
		if (i.getCode() == code)
			return i;
	}
	return m_NOICODE;
}