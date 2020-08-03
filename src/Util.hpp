#pragma once

/**************************************************************************

Util.hpp

Created by Matthew Clarke

Util functions

 **************************************************************************/

#include <string>
#include <bitset>

namespace icarus {

	namespace util {

		/*
		size_t HexStrToNum(std::string s)
		Converts a string to number
		*/
		size_t HexStrToNum(std::string s);

		/*
		std::string GetTimeStr()
		Returns the time in format H:M:S
		*/
		std::string GetTimeStr();

		/*
		std::string ToHexStr(T v)
		Returns a string with a number formatted into HEX
		*/
		template <class T>
		static std::string ToHexStr(T v, bool prefix = false) {
			char str[50];

			std::string fmt = "%0" + std::to_string(sizeof(T) * 2) + "X";
			if (prefix)
				fmt = "0x" + fmt;
			sprintf(str, fmt.c_str(), v);

			return std::string(str);
		}

		/*
		std::string ToBinaryStr(T v)
		Returns a string with a number formatted into BINARY
		*/
		template <class T>
		static std::string ToBinaryStr(T v) {
			std::string str = std::bitset<sizeof(T) * 8>(v).to_string();
			return str;
		}

	}

}