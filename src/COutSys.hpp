#pragma once

/**************************************************************************

COutSys.hpp

Created by Matthew Clarke

Console Output System. Handles formating and displaying information to the console

 **************************************************************************/

#include <string>

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

	};

}