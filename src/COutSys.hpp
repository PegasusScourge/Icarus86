#pragma once

/**************************************************************************

COutSys.hpp

Created by Matthew Clarke

Console Output System. Handles formating and displaying information to the console

 **************************************************************************/

#include <string>
#include <iomanip>
#include <sstream>

namespace icarus {

	class COutSys {
	private:
		static bool PREPEND_DATE;

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
		static std::string ToHexStr(T v, bool pad = false, bool prefix = false) {
			std::stringstream stream;
			if (prefix)
				stream << "0x";
			if (pad)
				stream << std::setw(sizeof(T) * 2) << std::setfill('0');
			stream << std::hex << v;
			return stream.str();
		}

	};

}