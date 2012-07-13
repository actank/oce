#ifndef GROUPLABEL_USERBASICCOUNT_H
#define GROUPLABEL_USERBASICCOUNT_H
#include "GroupCountMap.h"
#include "StringInt.h"
#include <string>
#include <map>
#include <vector>
using namespace mop::hi::svc::adapter;
using namespace std;
class UserBasicCount{
	public:
	map<int, map<string, string> > userBasicMap;
	UserBasicCount(const vector<int> &userIdList);
	void countCity(const vector<int> &userIdList, vector<StringInt> &res);

};
#endif
