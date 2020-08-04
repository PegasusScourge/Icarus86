/**************************************************************************

Processor.cpp

Created by Matthew Clarke

Abract implementation of a Processor allowing multiple types to be accessed in a simple way

 **************************************************************************/

#include "Processor.hpp"

using namespace icarus::processor;

void Processor::triggerError() {
	m_failed = true;
	onError();
}

void Processor::setClockRateMHz(float clockRateMHz) { 
	if (clockRateMHz <= 0) { 
		return;
	} 
	m_clockRateMHz = clockRateMHz; 
}

float Processor::getClockRateMHz() { 
	return m_clockRateMHz; 
}

std::string Processor::getName() { 
	return m_name; 
}

bool Processor::isFailed() { 
	return m_failed; 
}

void Processor::setHLT(bool h) { 
	m_isHLT = h; 
}

bool Processor::isHLT() { 
	return m_isHLT; 
}

ProcessorState& Processor::getProcessorState() {
	onGetProcessorState();
	return m_state;
}

void Processor::addBreakpoint(Breakpoint bp) {
	m_breakpoints.push_back(bp);
}

bool Processor::checkBreakpoint() {
	for (auto& bp : m_breakpoints) {
		if (bp.byAddress) {
			if (m_state.lastInstrs[0].ip == bp.address)
				return true;
		}
		else if (bp.byInstruction) {
			if (m_state.lastInstrs[0].iCode.getCode() == bp.instruction)
				return true;
		}
	}
	return false;
}

void Processor::requestInterrupt(uint8_t intCode) {
	m_intCodeSchedule.push_back(intCode);
}

bool Processor::hasInterrruptRequests() {
	return m_intCodeSchedule.size() > 0;
}

uint8_t Processor::getNextInterruptRequest() {
	uint8_t t = m_intCodeSchedule.at(0);
	m_intCodeSchedule.erase(m_intCodeSchedule.begin());
	return t;
}