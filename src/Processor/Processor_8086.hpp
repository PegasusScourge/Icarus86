#pragma once

/**************************************************************************

Processor_8086.hpp

Created by Matthew Clarke

Implementation of the 8086 processor

 **************************************************************************/

#include "Processor.hpp"

#include "../Bus/Bus.hpp"
#include "Register.hpp"

namespace icarus {

	class Processor_8086 : public Processor {
	private:
		// Bus references
		icarus::Bus16& m_dataBus;
		icarus::Bus32& m_addressBus;

		// Register declarations
		Register16 m_AX{ true };
		Register16 m_BX{ true };
		Register16 m_CX{ true };
		Register16 m_DX{ true };
		// Pointer and Index Registers
		Register16 m_SI{ false };
		Register16 m_DI{ false };
		Register16 m_BP{ false };
		Register16 m_SP{ false }; // Stack pointer
		// Misc Registers / Others
		Register16 m_IP{ false }; // Instruction pointer
		Register16 m_FLAGS{ true }; // Flag register
		// Segment Registers
		Register16 m_CS{ false };
		Register16 m_DS{ false };
		Register16 m_ES{ false };
		Register16 m_SS{ false };

	public:
		Processor_8086(icarus::Bus16& dataBus, icarus::Bus32& addressBus);

		/*
		Overridden main functions
		*/

		void fetch() override;
		unsigned int decode() override;
		void execute() override;

		/*
		uint32_t resolveAddress(uint16_t segment, uint16_t offset)
		Returns the resolved address of a segment-offset register pair
		*/
		uint32_t resolveAddress(uint16_t segment, uint16_t offset);

	};

}