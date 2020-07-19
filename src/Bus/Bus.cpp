/**************************************************************************

Bus.cpp

Created by Matthew Clarke

Bus implementation for data transfer

 **************************************************************************/

#include "Bus.hpp"

namespace i = icarus;

template <class T>
i::Bus<T>::Bus() {
	m_dataBitWidth = sizeof(m_data) * 8;
}

template <class T>
T i::Bus<T>::readData() {
	return m_data;
}

template <class T>
void i::Bus<T>::putData(T d) {
	m_data = d;
}

template <class T>
unsigned int i::Bus<T>::getBusBitWidth() {
	return m_dataBitWidth;
}