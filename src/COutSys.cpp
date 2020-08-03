/**************************************************************************

COutSys.cpp

Created by Matthew Clarke

Console Output System. Handles formating and displaying information to the console

 **************************************************************************/

#include "COutSys.hpp"
#include "Constexprs.hpp"
#include "Util.hpp"

#include <iostream>
#include <ctime>

using std::cout;
using icarus::endl;

namespace i = icarus;

bool i::COutSys::PREPEND_DATE = false;
icarus::LOutSys i::COutSys::CONSOLE_OUT("console.log", true);

void i::COutSys::Initialize(bool prependDate) {
	PREPEND_DATE = prependDate;
}

void i::COutSys::PrintDate() {
	cout << "[" << icarus::util::GetTimeStr() << "] ";
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
	CONSOLE_OUT.println("");
}

void i::COutSys::Print(std::string s, int level) {
	PrintLevel(level);
	cout << s;
	CONSOLE_OUT.print(s, level);
}