/**************************************************************************

MMU.cpp

Created by Matthew Clarke

Memory Management Unit implementation

 **************************************************************************/

#include "MMU.hpp"

namespace im = icarus::memory;

void im::MMU::addMemoryBlock(size_t startAddress, size_t size) {
	memoryBlocks.push_back(im::MemoryBlock(startAddress, size));
}
