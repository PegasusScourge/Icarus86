#pragma once

/**************************************************************************

MMU.hpp

Created by Matthew Clarke

Memory Management Unit implementation

 **************************************************************************/

#include "MemoryBlock.hpp"
#include "../Bus/Bus.hpp"
#include "../COutSys.hpp"
#include "../Constexprs.hpp"

#include <vector>

#ifdef MMU_DEBUG_PRINT
#define MMU_DEBUG(s) icarus::COutSys::Println(std::string(__FUNCTION__) +  ":\t" + s, icarus::COutSys::LEVEL_INFO)
#define MMU_DEBUG_ERR(s) icarus::COutSys::Println(std::string(__FUNCTION__) +  ":\t" + s, icarus::COutSys::LEVEL_ERR)
#else
#define MMU_DEBUG(s)
#define MMU_DEBUG_ERR(s)
#endif

namespace icarus {

	namespace memory {

		class MMU {
		private:
			std::vector<icarus::memory::MemoryBlock> memoryBlocks;

			size_t m_memoryTotal = 0;

		public:
			enum class ReadType { BigEndian, LittleEndian };

			/*
			void addMemoryBlock(size_t startAddress, size_t size)
			Adds a memory block at the start address with specified size
			*/
			void addMemoryBlock(size_t startAddress, size_t size);

			/*
			size_t getTotalMemory()
			Returns the amount of memory registered to the MMU
			*/
			size_t getTotalMemory();

			/*
			void writeByte(icarus::bus::Bus<D_BUS_TYPE>& dBus, icarus::bus::Bus<A_BUS_TYPE>& aBus)
			Writes a byte to the address
			*/
			void writeByte(icarus::bus::Bus& dBus, icarus::bus::Bus& aBus) {
				MMU_DEBUG("address = " + icarus::COutSys::ToHexStr(aBus.readData()));
				for (auto& block : memoryBlocks) {
					block.tryByteWrite(aBus.readData(), dBus.readData());
				}
			}
			/*
			void readByte(icarus::bus::Bus<D_BUS_TYPE>& dBus, icarus::bus::Bus<A_BUS_TYPE>& aBus)
			Reads a byte from address. Returns 0 in the event of a failure (undetectable failure, use tryReadByte if failure needs to be detected)
			*/
			void readByte(icarus::bus::Bus& dBus, icarus::bus::Bus& aBus) {
				uint8_t v = 0;
				for (auto& block : memoryBlocks) {
					if (block.tryByteRead(aBus.readData(), &v)) {
						// MMU_DEBUG("address = " + icarus::COutSys::ToHexStr(aBus.readData()));
						dBus.putData(v);
						return;
					}
				}
				dBus.putData(v);
			}
			/*
			bool tryReadByte(icarus::bus::Bus<D_BUS_TYPE>& dBus, icarus::bus::Bus<A_BUS_TYPE>& aBus)
			Returns true if a byte was read from the address into dest
			*/
			bool tryReadByte(icarus::bus::Bus& dBus, icarus::bus::Bus& aBus) {
				uint8_t v = 0;
				bool readSuccess = false;
				for (auto& block : memoryBlocks) {
					if (block.tryByteRead(aBus.readData(), &v)) {
						MMU_DEBUG("address = " + icarus::COutSys::ToHexStr(aBus.readData()));
						dBus.putData(v);
						readSuccess = true;
						break;
					}
				}
				return readSuccess;
			}

			/*
			void fillBus(icarus::bus::Bus& dBus, icarus::bus::Bus& aBus, ReadType endianness)
			Fills the dBus with bytes read from memory starting at the address given by aBus.
			*/
			void fillBus(icarus::bus::Bus& dBus, icarus::bus::Bus& aBus, ReadType endianness) {
				MMU_DEBUG("address = " + icarus::COutSys::ToHexStr(aBus.readData()));
				uint32_t address = aBus.readData();
				uint32_t dataToPush = 0;

				size_t dataBusByteWidth = dBus.getBitWidth() / 8;
				long long currentShift = 0;
				
				if (endianness == ReadType::BigEndian) {
					// Correct for BigEndian
					currentShift = dataBusByteWidth - 1;
					for (; currentShift >= 0; currentShift--, address++) {
						aBus.putData(address); // Put the new address
						readByte(dBus, aBus);
						dataToPush |= (dBus.readData() & 0xFF) << (currentShift * 8);

						MMU_DEBUG("shift = " + std::to_string(currentShift) + ", address = " +
							icarus::COutSys::ToHexStr(aBus.readData()) + ", val_so_far = " + icarus::COutSys::ToHexStr(dataToPush));
					}
				}
				else if (endianness == ReadType::LittleEndian) {
					// No correction for LittleEndian
					for (; currentShift < dataBusByteWidth; currentShift++, address++) {
						aBus.putData(address); // Put the new address
						readByte(dBus, aBus);
						dataToPush |= (dBus.readData() & 0xFF) << (currentShift * 8);

						MMU_DEBUG("shift = " + std::to_string(currentShift) + ", address = " +
							icarus::COutSys::ToHexStr(aBus.readData()) + ", val_so_far = " + icarus::COutSys::ToHexStr(dataToPush));
					}
				}
				else {
					// FAIL
				}

				dBus.putData(dataToPush);
			}

			/*
			void writeBus(icarus::bus::Bus& dBus, icarus::bus::Bus& aBus, ReadType endianness)
			Writes the content of dBus to memory starting at address given in aBus. Writes according to endianness
			*/
			void writeBus(icarus::bus::Bus& dBus, icarus::bus::Bus& aBus, ReadType endianness) {
				MMU_DEBUG("address = " + icarus::COutSys::ToHexStr(aBus.readData()));
				uint32_t address = aBus.readData();
				uint32_t dBusData = dBus.readData();
				size_t dataBusByteWidth = dBus.getBitWidth() / 8;
				long long currentShift = 0;

				if (endianness == ReadType::BigEndian) {
					// Correct for BigEndian
					currentShift = dataBusByteWidth - 1;
					for (; currentShift >= 0; currentShift--, address++) {
						aBus.putData(address); // Put the new address
						dBus.putData((dBusData >> (currentShift * 8)) & 0xFF);
						writeByte(dBus, aBus);
					}
				}
				else if (endianness == ReadType::LittleEndian) {
					// No correction for LittleEndian
					for (; currentShift < dataBusByteWidth; currentShift++, address++) {
						aBus.putData(address); // Put the new address
						dBus.putData((dBusData >> (currentShift * 8)) & 0xFF);
						writeByte(dBus, aBus);
					}
				}
				else {
					// FAIL
				}
			}

		};

	}
}