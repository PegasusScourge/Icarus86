/**************************************************************************

COutSys.cpp

Created by Matthew Clarke

Console Output System. Handles formating and displaying information to the console

 **************************************************************************/

#include "COutSys.hpp"
#include "Constexprs.hpp"

#include <iostream>

using std::cout;
using icarus::endl;

namespace i = icarus;

bool i::COutSys::PREPEND_DATE = false;

void i::COutSys::Initialize(bool prependDate) {
	PREPEND_DATE = prependDate;
}

void i::COutSys::PrintDate() {
	// Do nothing for now
	// TODO
}

void i::COutSys::PrintLevel(int level) {
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
	if (PREPEND_DATE)
		PrintDate();
	PrintLevel(level);
	cout << s;
}