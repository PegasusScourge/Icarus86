/**************************************************************************

Icarus86.cpp

Created by Matthew Clarke

Entry point to the emulator

 **************************************************************************/

#include "Util.hpp"
#include "Type.hpp"
#include "Constexprs.hpp"

#include <ctime>
#include <sstream>

std::string icarus::util::GetTimeStr() {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
	std::string str(buffer);
	return str;
}

size_t icarus::util::HexStrToNum(std::string s) {
	size_t code;
	std::stringstream ss;
	ss << std::hex << s;
	ss >> code;
	return code;
}

