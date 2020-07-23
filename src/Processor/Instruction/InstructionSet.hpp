#pragma once

/**************************************************************************

InstructionSet.hpp

Created by Matthew Clarke

Abract implementation of a instruction set

 **************************************************************************/

#include "../../Type.hpp"
#include "ModRMByte.hpp"
#include "Microcode.hpp"

#include <vector>
#include <string>

namespace icarus {

	namespace processor {

		namespace instruction {

			class ICode {
			private:
				bool m_isPrefix = false;
				bool m_hasModRM = false;
				uint8_t m_code = 0;
				unsigned int m_displacementBytes = 0;
				unsigned int m_immediateBytes = 0;
				unsigned int m_clockCost = 0;
				std::vector<ICode> m_childCodes;
				std::vector<Microcode> m_microcode;

			public:
				ICode(uint8_t code, bool isPrefix, bool modRM, unsigned int dispBytes, unsigned int immBytes,
					unsigned int clockCost, std::vector<Microcode> microCode, std::vector<ICode> childCodes = std::vector<ICode>(0));

				bool isPrefix();
				bool hasModRM();

				unsigned int numDisplacementBytes();
				unsigned int numImmediateBytes();
				unsigned int clockCost();

				uint8_t getCode();

				std::vector<ICode>& getChildCodes();
				std::vector<Microcode>& getMicrocode();

			};

			class InstructionSet {
			private:
				std::string m_filesrc;
				std::string m_name;

				bool m_valid = false;

				std::vector<ICode> m_iCodes;

				/*
				void parseJSON()
				Parses the JSON file provided in the src string
				*/
				void parseJSON();

			public:
				InstructionSet(); // Default constructor does nothing
				InstructionSet(std::string filesrc);

				std::string getName();
				std::string getFilesrc();
				bool isValid();

			};

		}

	}

}