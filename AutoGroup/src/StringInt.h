#ifndef GROUPLABEL_STRINGINT_H
#define GROUPLABEL_STRINGINT_H
#include <string>
#include <iostream>
using namespace std;
class StringInt {
	public:
	string name;
	int cnt;
	StringInt(string n = string(""), int c = 0);
	void disp();
};
#endif
