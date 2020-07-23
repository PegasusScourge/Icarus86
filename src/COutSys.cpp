/**************************************************************************

COutSys.cpp

Created by Matthew Clarke

Console Output System. Handles formating and displaying information to the console

 **************************************************************************/

#include "COutSys.hpp"
#include "Constexprs.hpp"

#include <iostream>
#include <ctime>

using std::cout;
using icarus::endl;

namespace i = icarus;

bool i::COutSys::PREPEND_DATE = false;

void i::COutSys::Initialize(bool prependDate) {
	PREPEND_DATE = prependDate;
}

void i::COutSys::PrintDate() {
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "[%H:%M:%S] ", timeinfo);
	std::string str(buffer);
	cout << str;
}

void i::COutSys::PrintLevel(int level) {
	if (level != COutSys::LEVEL_NONE && PREPEND_DATE) {
		PrintDate();
	}

	switch (level) {
	case COutSys::LEVEL_INFO:
		cout << "[INFO] ";
		break;

	case COutSys::LEVEL_WARN:
		cout << "[WARN] ";
		break;

	case COutSys::LEVEL_ERR:
		cout << "[ ERR] ";
		break;

	default:
		break;
	}
}

void i::COutSys::Println(std::string s, int level) {
	Print(s, level);
	cout << endl;
}

void i::COutSys::Print(std::string s, int level) {
	PrintLevel(level);
	cout << s;
}