/**************************************************************************

Microcode.cpp

Created by Matthew Clarke

A set of microcode instructions to complete

 **************************************************************************/

#include "Microcode.hpp"

using namespace icarus::processor::instruction;

 /***********************************/
 // class Microcode
 /***********************************/

Microcode::Microcode(MicrocodeType type, MicrocodeSrcSlot srcSlot) {
	m_type = type;
	m_srcSlot = srcSlot;
}

Microcode::MicrocodeType Microcode::getType() {
	return m_type;
}

Microcode::MicrocodeSrcSlot Microcode::getSrcSlot() {
	return m_srcSlot;
}

uint8_t Microcode::getSrcByteSize() {
	return m_srcByteSize;
}

uint8_t Microcode::getSrc() {
	return m_src;
}

uint8_t Microcode::getDstByteSize() {
	return m_dstByteSize;
}

uint8_t Microcode::getDst() {
	return m_dst;
}

void Microcode::setSrcByteSize(uint8_t bs) {
	m_srcByteSize = bs;
}

void Microcode::setSrc(uint8_t s) {
	m_src = s;
}

void Microcode::setDstByteSize(uint8_t bs) {
	m_dstByteSize = bs;
}

void Microcode::setDst(uint8_t d) {
	m_dst = d;
}