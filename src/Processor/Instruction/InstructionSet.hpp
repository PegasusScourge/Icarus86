#pragma once

/**************************************************************************

InstructionSet.hpp

Created by Matthew Clarke

Abract implementation of a instruction set

 **************************************************************************/

#include "../../Type.hpp"
#include "ModRMByte.hpp"

#include <vector>

namespace icarus {

	namespace processor {

		namespace instruction {

			enum class IType {

				I_ADD

			};

			class ICode {
			private:
				bool m_isPrefix = false;
				bool m_hasModRM = false;
				bool m_hasImmediate = false;
				bool m_hasDisplacement = false;
				uint8_t m_code = 0;
				IType type;
				unsigned int m_displacementBytes = 0;
				unsigned int m_immediateBytes = 0;
				std::vector<ICode> m_childCodes;

			public:
				ICode(IType type, uint8_t code, bool isPrefix, bool modRM, bool imm, bool disp,
					std::vector<ICode> childCodes = std::vector<ICode>(0), unsigned int dispBytes = 0, unsigned int immBytes = 0);

				bool isPrefix();
				bool hasModRM();
				bool hasImmediate();
				bool hasDisplacement();

				unsigned int numDisplacementBytes();
				unsigned int numImmediateBytes();

				IType getType();
				uint8_t getCode();

				std::vector<ICode>& getChildCodes();

			};

			class InstructionSet {
			private:


			public:


			};

		}

	}

}