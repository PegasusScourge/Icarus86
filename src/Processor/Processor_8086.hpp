#pragma once

/**************************************************************************

Processor_8086.hpp

Created by Matthew Clarke

Implementation of the 8086 processor

 **************************************************************************/

#include "Processor.hpp"

#include "../Bus/Bus.hpp"
#include "Register.hpp"

#include <vector>

namespace icarus {

	namespace processor {

		class Processor_8086 : public Processor {
		private:
			// Bus references
			icarus::bus::Bus16& m_dataBus;
			icarus::bus::Bus32& m_addressBus;

			// Register definitions
			std::vector<Register16> m_registers;
			enum class REGISTERS { R_AX = 0, R_BX, R_CX, R_DX, R_SI, R_DI, R_BP, R_SP, R_IP, R_FLAGS, R_CS, R_DS, R_ES, R_SS };
			static std::string REGISTER_NAMES[14];

			/*
			uint32_t resolveAddress(uint16_t segment, uint16_t offset)
			Returns the resolved address of a segment-offset register pair
			*/
			uint32_t resolveAddress(uint16_t segment, uint16_t offset);

		public:
			Processor_8086(icarus::bus::Bus16& dataBus, icarus::bus::Bus32& addressBus);

			/*
			Overridden main functions
			*/

			void fetch() override;
			unsigned int decode() override;
			void execute() override;
			std::vector<uint64_t> getRegisterValues() override;
			std::vector<std::string> getRegisterValuesAsStr() override;
			std::string* getRegisterNames() override;
		};

	}

}