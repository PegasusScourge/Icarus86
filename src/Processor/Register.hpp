#pragma once

/**************************************************************************

Register.hpp

Created by Matthew Clarke

Register implementation

 **************************************************************************/

#include "../Type.hpp"

namespace icarus {

	namespace processor {

		template <class T>
		class Register {
		private:
			T m_data;
			T m_registerBitWidth;
			T m_upperMask;
			T m_lowerMask;
			T m_upperShiftAmount;
			bool m_hasUpperLower;

		public:
			Register(bool hasUpperLower) {
				m_registerBitWidth = sizeof(m_data) * 8;
				m_hasUpperLower = hasUpperLower;

				if (m_hasUpperLower) {
					// Create the masks
					T mask = (1U << (m_registerBitWidth / 2)) - 1U;
					m_upperShiftAmount = (m_registerBitWidth / 2);
					m_upperMask = mask << m_upperShiftAmount;
					m_lowerMask = mask;
				}
				else {
					m_upperMask = 0;
					m_upperShiftAmount = 0;
					m_lowerMask = 0;
				}
				m_data = 0;
			}

			void setBit(T bit) {
				m_data |= (1U << bit);
			}

			void clearBit(T bit) {
				m_data &= ~(1U << bit);
			}

			void put(T value) {
				m_data = value;
			}
			T read() {
				return m_data;
			}

			void putUpper(T value) {
				if (m_hasUpperLower)
					m_data = (value << m_upperShiftAmount) | (m_data & m_lowerMask);
				else
					m_data = value;
			}
			T readUpper() {
				if (m_hasUpperLower)
					return (m_data >> m_upperShiftAmount);
				else
					return m_data;
			}

			void putLower(T value) {
				if (m_hasUpperLower)
					m_data = value | (m_data & m_upperMask);
				else
					m_data = value;
			}
			T readLower() {
				if (m_hasUpperLower)
					return (m_data & m_lowerMask);
				else
					return m_data;
			}

		};

		typedef Register<uint8_t> Register8;
		typedef Register<uint16_t> Register16;
		typedef Register<uint32_t> Register32;

	}

}