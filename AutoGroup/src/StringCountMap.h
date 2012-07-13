#ifndef GROUPLABEL_STRINGCOUNTMAP_H
#define GROUPLABEL_STRINGCOUNTMAP_H
#include <map>
#include <string>
#include <iostream>
using namespace std;
class StringCountMap {
	public:
	map<string,int> stringMap;
	string stringForMost;  //计数最多的字符串
	int countStringMost;    //相应的次数
	void insert(string &stringname);
	StringCountMap();
	void disp();   //测试用的，运行时不用
};
#endif
