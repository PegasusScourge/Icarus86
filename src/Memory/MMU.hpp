#pragma once

/**************************************************************************

MMU.hpp

Created by Matthew Clarke

Memory Management Unit implementation

 **************************************************************************/

#include "MemoryBlock.hpp"

#include "../Bus/Bus.hpp"

#include <vector>

namespace icarus {

	namespace memory {

		class MMU {
		private:
			std::vector<icarus::memory::MemoryBlock> memoryBlocks;

		public:
			/*
			void addMemoryBlock(size_t startAddress, size_t size)
			Adds a memory block at the start address with specified size
			*/
			void addMemoryBlock(size_t startAddress, size_t size);

			/*
			void writeByte(uint8_t value)
			Writes a byte to the address
			*/
			template <class D_BUS_TYPE, class A_BUS_TYPE>
			void writeByte(icarus::bus::Bus<D_BUS_TYPE>& dBus, icarus::bus::Bus<A_BUS_TYPE>& aBus) {
				for (auto& block : memoryBlocks) {
					block.tryByteWrite(aBus.readData(), dBus.readData());
				}
			}
			/*
			uint8_t readByte()
			Reads a byte from address. Returns 0 in the event of a failure (undetectable failure, use tryReadByte if failure needs to be detected)
			*/
			template <class D_BUS_TYPE, class A_BUS_TYPE>
			void readByte(icarus::bus::Bus<D_BUS_TYPE>& dBus, icarus::bus::Bus<A_BUS_TYPE>& aBus) {
				uint8_t v = 0;
				for (auto& block : memoryBlocks) {
					if (block.tryByteRead(aBus.readData(), &v)) {
						dBus.putData(v);
						return;
					}
				}
			}
			/*
			bool tryReadByte()
			Returns true if a byte was read from the address into dest
			*/
			template <class D_BUS_TYPE, class A_BUS_TYPE>
			bool tryReadByte(icarus::bus::Bus<D_BUS_TYPE>& dBus, icarus::bus::Bus<A_BUS_TYPE>& aBus) {
				uint8_t v = 0;
				bool readSuccess = false;
				for (auto& block : memoryBlocks) {
					if (block.tryByteRead(aBus.readData(), &v)) {
						dBus.putData(v);
						readSuccess = true;
					}
				}
				return readSuccess;
			}

		};

	}
}