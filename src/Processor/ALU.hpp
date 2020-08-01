#pragma once

/**************************************************************************

ALU.hpp

Created by Matthew Clarke

Arithmetic and Logic Unit implementation

 **************************************************************************/

#include "../Type.hpp"

#include <type_traits>
#include <limits>

namespace icarus {

	namespace processor {

		class ALU {
		private:
			// Internal status results
			bool m_carryBit;
			bool m_zeroFlag;
			bool m_negativeFlag;
			bool m_overflowFlag;
			bool m_parityFlag;

			/*
			void setParityFlag(T value)
			Sets the parity flag
			*/
			template <class T>
			void setParityFlag(T value) {
				unsigned int bitsSet = 0;
				size_t bits = sizeof(T) * 8;
				size_t shift;
				for (shift = 0; shift < bits; shift++) {
					bitsSet += (((value >> shift) & 0x1) ? 1 : 0);
				}
				m_parityFlag = (bitsSet % 2) == 0;
			}

			/*
			void setNegativeFlag(T value)
			Sets the negative flag
			*/
			template <class T>
			void setNegativeFlag(T value) {
				m_negativeFlag = isNegative(value);
			}

			/*
			bool isNegative(T value)
			Returns if the value is negative
			*/
			template <class T>
			bool isNegative(T value) {
				return (value >> ((sizeof(T) * 8) - 1)) & 0x1;
			}

			/*
			void setZeroFlag(T value)
			Sets the zero flag
			*/
			template <class T>
			void setZeroFlag(T value) {
				m_zeroFlag = (value == 0);
			}

			/*
			void setOverflowFlag(T result, T a, T b)
			Sets the overflow flag
			*/
			template <class T>
			void setOverflowFlag(T result, T a, T b) {
				m_overflowFlag = (isNegative(result) != isNegative(a)) && (isNegative(result) != isNegative(b));
			}

		public:
			/*
			bool test()
			Performs a test of the ALU operations. Returns true if it passed
			*/
			bool test();

			/*
			bool carryBit()
			Return the value of the carry bit
			*/
			bool carryBit();

			/*
			bool zeroFlag()
			Return the value of the zero flag
			*/
			bool zeroFlag();

			/*
			bool negativeFlag()
			Return the value of the negative flag
			*/
			bool negativeFlag();

			/*
			bool overflowFlag()
			Return the value of the overflow flag
			*/
			bool overflowFlag();

			/*
			bool parityFlag()
			Return the value of the parity flag
			*/
			bool parityFlag();

			/*
			void setCarryBit(bool c)
			Sets the value of the carry bit
			*/
			void setCarryBit(bool c);

			// ARITHMETIC

			/*
			T add(T a, T b, bool addCarry = false)
			Adds a to b and returns the result. If addCarry, then the carryBit is added too
			Flags: Zero, Carry, Overflow, Negative, Parity
			*/
			template <class T>
			std::enable_if_t<!std::is_signed<T>::value, T> add(T a, T b, bool addCarry = false) {
				size_t signMask = 0x8 << ((sizeof(T) * 8U) - 1U);
				T T_MAX = std::numeric_limits<T>::max();

				// Calculate result
				T result = a + b + (addCarry ? m_carryBit : 0);

				setZeroFlag(result);

				// Set carry bit
				m_carryBit = (a + b + (addCarry ? m_carryBit : 0)) > T_MAX;

				setOverflowFlag(result, a, b);
				setNegativeFlag(result);
				setParityFlag(result);

				return result;
			}

			/*
			T subtract(T a, T b, bool withBorrow = false)
			Subtracts b from a
			Flags: Zero, Carry (borrow), Overflow, Negative, Parity
			*/
			template <class T>
			std::enable_if_t<!std::is_signed<T>::value, T> subtract(T a, T b, bool withBorrow = false) {
				T result;
				if (withBorrow) {
					result = a - b - m_carryBit;
				}
				else {
					result = a - b;
				}
				setZeroFlag(result);

				// Set the carry / borrow bit
				m_carryBit = (a < b);

				setOverflowFlag(result, a, b);
				setNegativeFlag(result);
				setParityFlag(result);

				return result;
			}

			/*
			T increment(T a)
			Increments a by one and outputs the value, doesn't affect flags. Use add(a, b = 1) for this
			*/
			template <class T>
			T increment(T a) {
				T result = a + 1;

				setZeroFlag(result);
				setOverflowFlag(result, a, b);
				setNegativeFlag(result);
				setParityFlag(result);

				return result;
			}

			/*
			T decrement(T a)
			Decrements a by one and outputs the value, doesn't affect flags. Use subtract(a, b = 1) for this
			*/
			template <class T>
			T decrement(T a) {
				T result = a - 1;

				setZeroFlag(result);
				setOverflowFlag(result, a, b);
				setNegativeFlag(result);
				setParityFlag(result);

				return result;
			}

			/*
			T twosComplement(T a)
			Subtracts a from 0 (twos complement)
			*/
			template <class T>
			T twosComplement(T a) {
				return 0U - a;
			}

			// BIT SHIFTING

			/*
			T rotateLeft(T a, bool useCarryBit = false, unsigned int shiftAmount = 1)
			Rotates a left x bits. If useCarryBit, then we use the carry bit to move the digits into and out of as well
			*/
			template <class T>
			T rotateLeft(T a, bool useCarryBit = false, unsigned int shiftAmount = 1) {
				T result = a;
				size_t topBitShiftN = ((sizeof(T) * 8U) - 1U);
				bool tempCarryBit;
				if (useCarryBit) {
					for (unsigned int sc = 0; sc < shiftAmount; sc++) {
						tempCarryBit = m_carryBit;
						m_carryBit = (result >> topBitShiftN) & 0x1;
						result = result << 1;
						result |= (T)tempCarryBit;
					}
				}
				else {
					for (unsigned int sc = 0; sc < shiftAmount; sc++) {
						tempCarryBit = (result >> topBitShiftN) & 0x1;
						result = result << 1;
						result |= (T)tempCarryBit;
					}
				}
				return result;
			}

			/*
			T rotateRight(T a, bool useCarryBit = false, unsigned int shiftAmount = 1)
			Rotates a right x bits. If useCarryBit, then we use the carry bit to move the digits into and out of as well
			*/
			template <class T>
			T rotateRight(T a, bool useCarryBit = false, unsigned int shiftAmount = 1) {
				T result = a;
				size_t topBitShiftN = ((sizeof(T) * 8U) - 1U);
				bool tempCarryBit;
				if (useCarryBit) {
					for (unsigned int sc = 0; sc < shiftAmount; sc++) {
						tempCarryBit = m_carryBit;
						m_carryBit = result & 0x1;
						result = result >> 1;
						result |= (T)tempCarryBit << topBitShiftN;
					}
				}
				else {
					for (unsigned int sc = 0; sc < shiftAmount; sc++) {
						tempCarryBit = result & 0x1;
						result = result >> 1;
						result |= (T)tempCarryBit << topBitShiftN;
					}
				}
				return result;
			}

			// BINARY LOGIC

			/*
			T binaryAND(T a, T b)
			Returns the value of a & b
			*/
			template <class T>
			T binaryAND(T a, T b) {
				T result = a & b;
				setZeroFlag(result);
				setOverflowFlag(result, a, b);
				setNegativeFlag(result);
				setParityFlag(result);
				return result;
			}

			/*
			T binaryOR(T a, T b)
			Returns the value of a | b
			*/
			template <class T>
			T binaryOR(T a, T b) {
				T result = a | b;
				setZeroFlag(result);
				setOverflowFlag(result, a, b);
				setNegativeFlag(result);
				setParityFlag(result);
				return result;
			}

			/*
			T binaryXOR(T a, T b)
			Returns the value of a ^ b
			*/
			template <class T>
			T binaryXOR(T a, T b) {
				T result = a ^ b;
				setZeroFlag(result);
				setOverflowFlag(result, a, b);
				setNegativeFlag(result);
				setParityFlag(result);
				return result;
			}

			/*
			T binaryNOT(T a)
			Inverts the bits of a and returns
			*/
			template <class T>
			T binaryNOT(T a) {
				T result = ~a;
				setZeroFlag(result);
				setNegativeFlag(result);
				setParityFlag(result);
				return result;
			}

		};

	}

}