#ifndef GROUPLABEL_NETWORKCOUNT_H
#define GROUPLABEL_NETWORKCOUNT_H
#include "UserNetworkSocialAdapter.h"
//#include "UserNetworkAdapter.h"
#include <map>
#include <vector>
#include "StringInt.h"
#include "GroupCountMap.h"
#include <stdlib.h>
#include <stdio.h>
using namespace std;
using namespace mop::hi::svc::adapter;
class NetworkCount{
	private:
		map<int, NetworkBigObjectNPtr> networkObjectMap;
	public:
		NetworkCount(const vector<int> &userIdList);
		void disp();
		void countUniv(const vector<int> &userlist, vector<StringInt> &res);
		void countHighSchool(const vector<int> &userList, vector<StringInt> &res);
		void countElementarySchool(const vector<int> &userList, vector<StringInt> &res);
		void countJuniorHighSchool(const vector<int> &userList, vector<StringInt> &res);
		void countWorkplace(const vector<int> &userList, vector<StringInt> &res);
};
#endif
