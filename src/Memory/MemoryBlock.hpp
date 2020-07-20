#pragma once

/**************************************************************************

MemoryBlock.hpp

Created by Matthew Clarke

Controls a block of memory that the CPU can access

 **************************************************************************/

#include "../Type.hpp"

#include <vector>

namespace icarus {

	namespace memory {

		class MemoryBlock {
		private:
			std::vector<uint8_t> m_data;
			size_t m_startAddress = 0;
			size_t m_size;

			bool isInRange(size_t address);

		public:
			MemoryBlock(size_t startAddress, size_t size);
			~MemoryBlock();

			void tryByteWrite(size_t address, uint8_t value);
			bool tryByteRead(size_t address, uint8_t* dest);

			size_t getSize();
			size_t getStartAddress();

		};

	}

}