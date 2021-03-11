/*
    **> \File           InstructionSet.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-11
    **> \Brief          InstructionSet
    **> \Details        None
    **> \Notes          None
*/

#include "Intel8086/Instruction/InstructionSet.hpp"
#include "Util/HexUtil.hpp"
#include "Util/LogFile.hpp"

#include <fstream>
#include <sstream>

namespace ipi = i86::intel86::instruction;

inline i86::util::LogFile InstructionSetLog("ISet.log");

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
	std::stringstream& log = InstructionSetLog.strs();

	// Sanity check for the important things
	if (!entry["hexcode"].is_string()) {
		// Fail this entry
		return;
	}
	std::string code_s = entry["hexcode"].get<std::string>();
	log << "iCode: hexcode_s=" << code_s << " ";

	// Convert the code to hex number
	m_code = i86::util::HexStrToNum<uint8_t>(code_s);
	log << "(#=" << std::to_string(m_code) << ")\t\t";

	// Check for prefix
	if (entry["isPrefix"].is_boolean() && entry["isPrefix"].get<bool>()) {
		m_isPrefix = true;
		log << "[ PREFIX] Checking for child codes: ";
		if (entry["childCodes"].is_array()) {
			uint64_t newPrefix = (prefix << 8) & m_code;
			log << "[FOUND]. Parsing (newPrefix=" << i86::util::NumToHexStr(newPrefix) << ")";
			log << "iCode: <children>"; InstructionSetLog.flushss();
			for (auto& iCode : entry["childCodes"]) {
				m_childCodes.push_back(ICode(iCode, newPrefix));
			}
			log << "iCode: </children>"; InstructionSetLog.flushss();
			m_valid = true;
			return;
		}
		else {
			log << "[NOT FOUND]. Ignoring"; InstructionSetLog.flushss();
		}
	}
	else {
		log << "[!PREFIX] ";
		m_isPrefix = false;
	}

	// Gather other information
	if (entry["hasModRM"].is_boolean()) {
		m_hasModRM = entry["hasModRM"].get<bool>();
		log << (m_hasModRM ? "[ MODRM] " : "[!MODRM] ");
	}
	else {
		m_hasModRM = false;
	}

	if (entry["immediateBytes"].is_number_unsigned()) {
		m_immediateBytes = entry["immediateBytes"].get<unsigned int>();
		log << "[IMM: " << std::to_string(m_immediateBytes) << "] ";
	}
	else {
		m_immediateBytes = 0;
	}

	if (entry["displacementBytes"].is_number_unsigned()) {
		m_displacementBytes = entry["displacementBytes"].get<unsigned int>();
		log << "[DISP: " << std::to_string(m_displacementBytes) << "] ";
	}
	else {
		m_displacementBytes = 0;
	}
		
	if (entry["clockCost"].is_number_unsigned()) {
		m_clockCost = entry["clockCost"].get<unsigned int>();
		log << "[CLK#: " << std::to_string(m_clockCost) << "] ";
	}	
	else {
		m_clockCost = 0;
	}

	if (entry["mnemonic"].is_string()) {
		m_mnemonic = entry["mnemonic"].get<std::string>();
	}
	else {
		m_mnemonic = "NONE";
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

		log << "[MCODE#: " << std::to_string(microcodeAdded) << "] ";
		if (microcodeAdded == 0) {
			log << "[FAIL: MCODE len = 0]"; InstructionSetLog.flushss();
			return;
		}
	}
	else {
		log << "[FAIL: MCODE missing]"; InstructionSetLog.flushss();
		return;
	}

	log << "[PARSED]"; InstructionSetLog.flushss();
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

std::string ipi::ICode::getMnemonic() {
	return m_mnemonic;
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
	std::stringstream& log = InstructionSetLog.strs();

	log << "InstructionSet from file '" << m_filesrc << "': Opening"; InstructionSetLog.flushss();

	json j;
	std::ifstream stream(m_filesrc);
	if (!stream.is_open()) {
		// ERROR
		log << "InstructionSet from file '" << m_filesrc << "': Couldn't open file stream. Aborted"; InstructionSetLog.flushss();
		m_valid = false;
		return;
	}

	// We are clear to continue
	log << "InstructionSet from file '" << m_filesrc << "': Got file. Reading"; InstructionSetLog.flushss();
	stream >> j;
	log << "InstructionSet from file '" << m_filesrc << "': Read. Checking header"; InstructionSetLog.flushss();
	
	// Check that we have the right header information
	if (!j["icarus"].is_boolean() && j["icarus"].get<bool>()) {
		// ERROR
		log << "InstructionSet from file '" << m_filesrc << "': Couldn't find header 'icarus' bool element. Aborted"; InstructionSetLog.flushss();
		m_valid = false;
		return;
	}

	// Now load the name
	if (j["name"].is_string()) {
		m_name = j["name"].get<std::string>();
		log << "InstructionSet from file '" << m_filesrc << "': Loaded. Name = " << m_name; InstructionSetLog.flushss();
	}
	else {
		log << "InstructionSet from file '" << m_filesrc << "': Couldn't find header 'name' string element. Aborted"; InstructionSetLog.flushss();
		m_valid = false;
		return;
	}

	if (!j["icode"].is_array()) {
		log << "InstructionSet from file '" << m_filesrc << "': Couldn't find header 'icode' array element. Aborted"; InstructionSetLog.flushss();
		m_valid = false;
		return;
	}

	log << "InstructionSet from file '" << m_filesrc << "': iCode found! Parsing..."; InstructionSetLog.flushss();
	// Now we must parse the instructions
	auto& iCodeNode = j["icode"];
	
	bool failedICode = false;

	for (auto& iCode : iCodeNode) {
		ICode i(iCode, 0);
		if (!i.isValid())
			failedICode = true;
		m_iCodes.push_back(i);
	}

	m_valid = !failedICode;
}

ipi::ICode& ipi::InstructionSet::operator[](uint8_t code) {
	for (auto& i : m_iCodes) {
		if (i.getCode() == code)
			return i;
	}
	return m_NOICODE;
}