#pragma once

/**************************************************************************

Bus.hpp

Created by Matthew Clarke

Bus implementation for data transfer

 **************************************************************************/

#include "../Type.hpp"

namespace icarus {

	namespace bus {

		class Bus {
		private:
			uint32_t m_data;
			uint32_t m_dataBitWidth;
			uint32_t m_mask; // If non-zero, we apply it

		public:
			Bus() {
				m_dataBitWidth = sizeof(m_data) * 8;
				m_data = 0;
				m_mask = 0;
			}
			Bus(uint32_t bitWidthLimit) { // Enforce a specific bit width limit
				unsigned long widthTemp = sizeof(m_data) * 8;
				m_dataBitWidth = bitWidthLimit;
				if (m_dataBitWidth > widthTemp) {
					m_dataBitWidth = widthTemp;
				}

				// Set the mask
				m_mask = (1U << m_dataBitWidth) - 1U;

				m_data = 0;
			}

			uint32_t readData() {
				if (m_mask)
					return m_data & m_mask;
				return m_data;
			}

			void putData(uint32_t d) {
				if (m_mask)
					m_data = d & m_mask;
				else
					m_data = d;
			}

			uint32_t getBitWidth() {
				return m_dataBitWidth;
			}
		};

	}

}