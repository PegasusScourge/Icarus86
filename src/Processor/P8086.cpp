/**************************************************************************

P8086.cpp

Created by Matthew Clarke

Implementation of the 8086 processor

 **************************************************************************/

#include "P8086.hpp"

#include "../COutSys.hpp"
#include "../Constexprs.hpp"
#include "../Util.hpp"
#include "Instruction/ModRMByte.hpp"

namespace ip = icarus::processor;
namespace ipi = ip::instruction;

#ifdef DECODE8086_DEBUG_PRINT
	#define DECODE8086_DEBUG(s) icarus::COutSys::Println(std::string(__FUNCTION__) +  ":\t" + s, icarus::COutSys::LEVEL_INFO)
	#define DECODE8086_DEBUG_ERR(s) icarus::COutSys::Println(std::string(__FUNCTION__) +  ":\t" + s, icarus::COutSys::LEVEL_ERR)
#else
	#define DECODE8086_DEBUG(s)
	#define DECODE8086_DEBUG_ERR(s)
#endif
#ifdef EXEC8086_DEBUG_PRINT
	#define EXEC8086_DEBUG(s) icarus::COutSys::Println(std::string(__FUNCTION__) +  ":\t" + s, icarus::COutSys::LEVEL_INFO)
	#define EXEC8086_DEBUG_ERR(s) icarus::COutSys::Println(std::string(__FUNCTION__) +  ":\t" + s, icarus::COutSys::LEVEL_ERR)
#else
	#define EXEC8086_DEBUG(s)
	#define EXEC8086_DEBUG_ERR(s)
#endif

/***********************************/
// class Processor_8086 : public
/***********************************/

std::string ip::Processor_8086::REGISTER_NAMES[14] = { "AX", "BX", "CX", "DX", "SI", "DI", "BP", "SP", "IP", "FR", "CS", "DS", "ES", "SS" };

ip::Processor_8086::Processor_8086(icarus::memory::MMU& mmu, icarus::bus::Bus& dataBus, icarus::bus::Bus& addressBus) : m_mmu(mmu), m_dataBus(dataBus), m_addressBus(addressBus) {
	m_name = "Intel 8086";

	// Configure our state
	m_state.registerValues_names = REGISTER_NAMES;
	m_state.flagsNames = "....ODITSZ.A.P.C";

	// Create the registers
	for(int i = 0; i < 4; i++)
		m_registers.push_back(Register16{ true }); // Create registers AX through DX

	for (int i = 0; i < 5; i++)
		m_registers.push_back(Register16{ false }); // Create registers SI through IP

	m_registers.push_back(Register16{ true }); // Create register FLAGS

	for (int i = 0; i < 4; i++)
		m_registers.push_back(Register16{ false }); // Create registers CS through SS

	// Get the instrction set
	m_iSet = icarus::processor::instruction::InstructionSet(icarus::processor::instruction::InstructionPath + "8086.json");

	m_alu.test();

	m_cInstr.hasSegOverride = false;
	m_cInstr.usedSegOverride = false;
}

unsigned int ip::Processor_8086::fetchDecode() {
	unsigned int cyclesToWait = 1;

	// Load the address bus with the address
	unsigned int increment = 0;
	uint16_t ipVal = m_registers[REGISTERS::R_IP].read();
	m_addressBus.putData(getSegmentedAddress(SEGMENT::S_CODE,ipVal));
	if (!m_mmu.tryReadByte(m_dataBus, m_addressBus)) {
		// Failed to read a byte
		triggerError();
		return 0;
	}

	ipi::ICode& instr = m_iSet[(uint8_t)m_dataBus.readData()];

	while (instr.isValid() && instr.isPrefix()) {
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_CODE,ipVal + (++increment)));
		if (!m_mmu.tryReadByte(m_dataBus, m_addressBus)) {
			// Failed to read a byte
			icarus::COutSys::Println("Processor8086 failed to read byte", icarus::COutSys::LEVEL_ERR);
			triggerError();
			return 0;
		}

		instr = instr[(uint8_t)m_dataBus.readData()];
	}

	DECODE8086_DEBUG("**********************---- <DECODE> ----**********************");
	DECODE8086_DEBUG("Instr code = " + icarus::COutSys::ToHexStr(instr.getCode()) + ", dBusCode = " + icarus::COutSys::ToHexStr(m_dataBus.readData()));
	DECODE8086_DEBUG("AddressBus = " + icarus::COutSys::ToHexStr(m_addressBus.readData()));

	if (!instr.isValid()) {
		// Failed to get a valid instruction
		icarus::COutSys::Println("Processor8086 failed to get valid instr", icarus::COutSys::LEVEL_ERR);
		triggerError();
		return 0;
	}

	DECODE8086_DEBUG("Instr valid");

	m_cInstr.code = instr.getCode();
	
	// Check for segmenet override
	if (m_cInstr.hasSegOverride && m_cInstr.usedSegOverride) {
		// We have used the seg override, discard it now
		m_cInstr.hasSegOverride = false;
		m_cInstr.usedSegOverride = false;
	}
	else if (m_cInstr.hasSegOverride && !m_cInstr.usedSegOverride) {
		// We have a seg override set up from the past instruction (which was a seg override),
		// mark as used so we can discard it next time
		m_cInstr.usedSegOverride = true;
	}

	DECODE8086_DEBUG("Instr mnemonic: " + instr.getMnemonic());

	// We have a valid instruction, now lets get the microcode
	m_cInstr.microcode = instr.getMicrocode();
	if (m_cInstr.microcode.size() == 0) {
		// No microcode!
		icarus::COutSys::Println("Processor8086 found no microcode in instr", icarus::COutSys::LEVEL_WARN);
		return 0;
	}
	DECODE8086_DEBUG("Has microcode");

	// The number of displacement and immediate bytes can be modified by the ModRM value. So we cache here in order to increment
	unsigned int numDispBytes = instr.numDisplacementBytes();
	unsigned int numImmBytes = instr.numImmediateBytes();

	// Get a ModRMByte if needed
	m_cInstr.modRMByte.setByte(0);
	if (instr.hasModRM()) {
		DECODE8086_DEBUG("Has modrm");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_CODE,ipVal + (++increment)));
		if (!m_mmu.tryReadByte(m_dataBus, m_addressBus)) {
			// Failed to read a byte
			icarus::COutSys::Println("Processor8086 failed to read byte (for ModRM)", icarus::COutSys::LEVEL_ERR);
			triggerError();
			return 0;
		}
		m_cInstr.modRMByte.setByte((uint8_t)m_dataBus.readData());
		DECODE8086_DEBUG("MODRM = " + icarus::COutSys::ToHexStr(m_dataBus.readData()));

		// Now we need to detect if we have a modifying value that will necessitate adding disp or imm bytes.
		if (numDispBytes == 0) {
			if (m_cInstr.modRMByte.MOD() == 0b01) {
				// We need to have one displacement byte!
				numDispBytes++;
				DECODE8086_DEBUG("MODRM indicated need for one displacement byte where instruction set doesn't, adding");
			}
			if (m_cInstr.modRMByte.MOD() == 0b10) {
				// We need to have two displacement bytes!
				numDispBytes += 2;
				DECODE8086_DEBUG("MODRM indicated need for two displacement bytes where instruction set doesn't, adding");
			}
			if (m_cInstr.modRMByte.MOD() == 0b00 && m_cInstr.modRMByte.RM() == 0b110) {
				// We need to have two displacement bytes!
				numDispBytes += 2;
				DECODE8086_DEBUG("MODRM indicated need for two displacement bytes where instruction set doesn't, adding");
			}
		}
	}

	// Get displacment bytes if needed
	m_cInstr.displacement = 0;
	if (numDispBytes > 0) {
		DECODE8086_DEBUG("Has displacement");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_CODE,ipVal + (++increment)));
		m_cInstr.numDisplacementBytes = numDispBytes;
		switch (numDispBytes) {
		case 2:
			DECODE8086_DEBUG("2 Byte");
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.displacement |= m_dataBus.readData();
			m_addressBus.putData(getSegmentedAddress(SEGMENT::S_CODE,ipVal + (++increment)));
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.displacement |= (m_dataBus.readData() << 8);
			break;
		case 1:
			DECODE8086_DEBUG("1 Byte");
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.displacement |= m_dataBus.readData();
			break;

		default:
			// Error getting displacement bytes
			icarus::COutSys::Println("Processor8086 failed to get displacement bytes (num=" + 
				std::to_string(numDispBytes) + ")", icarus::COutSys::LEVEL_ERR);
			triggerError();
			return 0;
		}
	}

	// Get immediate bytes if needed
	m_cInstr.immediate = 0;
	if (numImmBytes > 0) {
		DECODE8086_DEBUG("Has immediate");
		m_addressBus.putData(getSegmentedAddress(SEGMENT::S_CODE,ipVal + (++increment)));
		m_cInstr.numImmeditateBytes = numImmBytes;
		switch (numImmBytes) {
		case 2:
			DECODE8086_DEBUG("2 Byte");
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.immediate |= m_dataBus.readData();
			m_addressBus.putData(getSegmentedAddress(SEGMENT::S_CODE,ipVal + (++increment)));
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.immediate |= (m_dataBus.readData() << 8);
			break;
		case 1:
			DECODE8086_DEBUG("1 Byte");
			m_mmu.readByte(m_dataBus, m_addressBus);
			m_cInstr.immediate |= m_dataBus.readData();
			break;

		default:
			// Error getting displacement bytes
			icarus::COutSys::Println("Processor8086 failed to get immediate bytes (num=" +
				std::to_string(numImmBytes) + ")", icarus::COutSys::LEVEL_ERR);
			triggerError();
			return 0;
		}
	}
	
	DECODE8086_DEBUG("IP = " + COutSys::ToHexStr(m_registers[REGISTERS::R_IP].read()));

	// Update the last instruction
	LastInstruction_t lastInstr;
	lastInstr.iCode = instr;
	lastInstr.ip = ipVal;
	lastInstr.disp = m_cInstr.displacement;
	lastInstr.imm = m_cInstr.immediate;

	m_state.lastInstrs.push(lastInstr);
	m_registers[REGISTERS::R_IP].put(ipVal + increment + 1);

	DECODE8086_DEBUG("IP incremented to = " + COutSys::ToHexStr(m_registers[REGISTERS::R_IP].read()));
	DECODE8086_DEBUG("**********************---- </DECODE> ----**********************");

	return cyclesToWait;
}

void ip::Processor_8086::execute() {
	// Print out the information
	EXEC8086_DEBUG("**********************---- <EXECUTE> ----**********************");
	EXEC8086_DEBUG("code = " + icarus::COutSys::ToHexStr(m_cInstr.code));
	EXEC8086_DEBUG("immediate = " + icarus::COutSys::ToHexStr(m_cInstr.immediate));
	EXEC8086_DEBUG("displacement = " + icarus::COutSys::ToHexStr(m_cInstr.displacement));
	EXEC8086_DEBUG("MODRM = " + icarus::COutSys::ToHexStr(m_cInstr.modRMByte.byte()));

	// We now have to execute the microcode instructions

	// Init the mCodeI struct
	m_cInstr.mCodeI.bitMode8Bit = false;
	m_cInstr.mCodeI.srcAUsed = false;
	m_cInstr.mCodeI.dstEnabled = false; // Disable dst writing by default

	for (auto& mcode : m_cInstr.microcode) {
		mcode_execCode(mcode);
	}
	EXEC8086_DEBUG("**********************---- </EXECUTE> ----**********************" + icarus::endl);
}

void ip::Processor_8086::forceIP(uint64_t ip) {
	m_registers[REGISTERS::R_IP].put(ip);
}

void ip::Processor_8086::forceSP(uint64_t sp) {
	m_registers[REGISTERS::R_SP].put(sp);
}


/***********************************/
// class Processor_8086 : private
/***********************************/

uint32_t ip::Processor_8086::resolveAddress(uint16_t segment, uint16_t offset) {
	return (segment << 4) + offset;
}

uint32_t ip::Processor_8086::getSegmentedAddress(SEGMENT defaultSeg, uint16_t offset) {
	SEGMENT seg = defaultSeg;
	// Handle segment overrides
	if (m_cInstr.hasSegOverride) {
		if (defaultSeg == SEGMENT::S_DATA) {
			// Data segment is overriden
			seg = m_cInstr.segOverride;
		}
	}

	// Get the value of the segment register
	uint32_t sval = 0;
	switch (seg) {
	case SEGMENT::S_CODE:
		sval = m_registers[REGISTERS::R_CS].read();
		break;
	case SEGMENT::S_XTRA:
		sval = m_registers[REGISTERS::R_ES].read();
		break;
	case SEGMENT::S_STACK:
		sval = m_registers[REGISTERS::R_SS].read();
		break;
	case SEGMENT::S_DATA:	// On default, we go for the data segment as thats more than likely where we will read data from, right?
	default:				// Better than no default
		sval = m_registers[REGISTERS::R_DS].read();
		break;
	}
	// Return the resolved address
	return resolveAddress(sval, offset);
}

void ip::Processor_8086::onError() {

}

void ip::Processor_8086::onGetProcessorState() {
	std::vector<uint64_t> regs_i;
	for (auto& r : m_registers) {
		regs_i.push_back(r.read());
	}
	m_state.registerValues_num = regs_i;
	
	std::vector<std::string> regs_s;
	for (auto& r : m_registers) {
		regs_s.push_back(icarus::util::ToHexStr(r.read(), true));
	}
	m_state.registerValues_str = regs_s;

	m_state.flagsRegBin = icarus::util::ToBinaryStr(m_registers[REGISTERS::R_FLAGS].read());
}
