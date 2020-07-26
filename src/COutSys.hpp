#pragma once

/**************************************************************************

COutSys.hpp

Created by Matthew Clarke

Console Output System. Handles formating and displaying information to the console

 **************************************************************************/

#include <string>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <bitset>

#include "LOutSys.hpp"

namespace icarus {

	class COutSys {
	private:
		static bool PREPEND_DATE;

		static icarus::LOutSys CONSOLE_OUT;

		/*
		void PrintLevel(int level)
		Prints the level to the console
		*/
		static void PrintLevel(int level);

		/*
		void PrintDate()
		Prints the date to the console
		*/
		static void PrintDate();


	public:
		/*
		Level consts
		*/
		const static int LEVEL_NONE = -1;
		const static int LEVEL_INFO = 0;
		const static int LEVEL_WARN = 1;
		const static int LEVEL_ERR = 2;

		COutSys() = delete; // Remove the constructor so there are no instances: this is a static class

		/*
		void Initialize()
		Initialization function that preps the output and formatting
		*/
		static void Initialize(bool prependDate = false);

		/*
		void Println(std::string, int level)
		Prints a string to the console with the appropriate warning level
		*/
		static void Println(std::string s, int level = LEVEL_NONE);

		/*
		void Print(std::string, int level)
		Prints a string to the console with the appropriate warning level, without a new line
		*/
		static void Print(std::string s, int level = LEVEL_NONE);

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

	};

}