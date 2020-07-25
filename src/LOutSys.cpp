/**************************************************************************

LOutSys.hpp

Created by Matthew Clarke

Logging Output System. Handles formating and output to log files

 **************************************************************************/

#include "LOutSys.hpp"
#include "Constexprs.hpp"

#include <ctime>

using icarus::endl;

namespace i = icarus;

i::LOutSys::LOutSys(std::string file, bool prependDate) {
	m_prependDate = prependDate;
	output.open(file);
}

i::LOutSys::~LOutSys() {
	if (output.is_open())
		output.close();
}

void i::LOutSys::printDate() {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "[%H:%M:%S] ", timeinfo);
	std::string str(buffer);
	output << str;
}

void i::LOutSys::printLevel(int level) {
	if (level != LOutSys::LEVEL_NONE && m_prependDate) {
		printDate();
	}

	switch (level) {
	case LOutSys::LEVEL_INFO:
		output << "[INFO] ";
		break;

	case LOutSys::LEVEL_WARN:
		output << "[WARN] ";
		break;

	case LOutSys::LEVEL_ERR:
		output << "[ ERR] ";
		break;

	default:
		break;
	}
}

void i::LOutSys::println(std::string s, int level) {
	print(s, level);
	output << endl;
}

void i::LOutSys::print(std::string s, int level) {
	printLevel(level);
	output << s;
}