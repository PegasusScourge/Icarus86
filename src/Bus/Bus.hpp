#pragma once

/**************************************************************************

Bus.hpp

Created by Matthew Clarke

Bus implementation for data transfer

 **************************************************************************/

#include "../Type.hpp"

namespace icarus {

	template <class T>
	class Bus {
	private:
		T m_data;
		T m_dataBitWidth;
		T m_mask; // If non-zero, we apply it

	public:
		Bus() {
			m_dataBitWidth = sizeof(m_data) * 8;
			m_data = 0;
			m_mask = 0;
		}
		Bus(T bitWidthLimit) { // Enforce a specific bit width limit
			unsigned long widthTemp = sizeof(m_data) * 8;
			m_dataBitWidth = bitWidthLimit;
			if (m_dataBitWidth > widthTemp) {
				m_dataBitWidth = widthTemp;
			}

			// Set the mask
			m_mask = (1U << m_dataBitWidth) - 1U;

			m_data = 0;
		} 

		T readData() {
			if (m_mask)
				return m_data & m_mask;
			return m_data;
		}

		void putData(T d) {
			if (m_mask)
				m_data = d & m_mask;
			else
				m_data = d;
		}

		T getBitWidth() {
			return m_dataBitWidth;
		}
	};

	typedef Bus<uint16_t> Bus16;
	typedef Bus<uint32_t> Bus32;
	typedef Bus<uint8_t> Bus8;

}