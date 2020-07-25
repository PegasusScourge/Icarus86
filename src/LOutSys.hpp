#pragma once

/**************************************************************************

LOutSys.hpp

Created by Matthew Clarke

Logging Output System. Handles formating and output to log files

 **************************************************************************/

#include <string>
#include <fstream>

namespace icarus {

	class LOutSys {
	private:
		bool m_prependDate;

		std::ofstream output;

		/*
		void printLevel(int level)
		Prints the level to the file
		*/
		void printLevel(int level);

		/*
		void printDate()
		Prints the date to the file
		*/
		void printDate();


	public:
		/*
		Level consts
		*/
		const static int LEVEL_NONE = -1;
		const static int LEVEL_INFO = 0;
		const static int LEVEL_WARN = 1;
		const static int LEVEL_ERR = 2;

		LOutSys(std::string file, bool prependDate = false);
		~LOutSys();

		/*
		void initialize()
		Initialization function that preps the output and formatting
		*/
		void initialize(bool prependDate = false);

		/*
		void println(std::string, int level)
		Prints a string to the file with the appropriate warning level
		*/
		void println(std::string s, int level = LEVEL_NONE);

		/*
		void print(std::string, int level)
		Prints a string to the file with the appropriate warning level, without a new line
		*/
		void print(std::string s, int level = LEVEL_NONE);

	};

}