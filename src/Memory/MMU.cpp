/**************************************************************************

MMU.cpp

Created by Matthew Clarke

Memory Management Unit implementation

 **************************************************************************/

#include "MMU.hpp"

namespace im = icarus::memory;

/***********************************/
// class MMU : public
/***********************************/

void im::MMU::addMemoryBlock(size_t startAddress, size_t size) {
	memoryBlocks.push_back(im::MemoryBlock(startAddress, size));
	m_memoryTotal += size;
}

size_t im::MMU::getTotalMemory() {
	return m_memoryTotal;
}