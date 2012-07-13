#ifndef GROUPLABEL_GROUPCOUNTMAP_H
#define GROUPLABEL_GROUPCOUNTMAP_H
#include <map>
#include <string>
#include <vector>
#include "GroupCountElement.h"
#include "StringInt.h"
using namespace std;
class GroupCountMap {
	private:
		map<string, GroupCountElement> countMap;
		string nameForMost;	//拥有最多人的univID
		int countForMost;//相应的人数
	public:
		void insert(string &indexName, vector<string> &elementNames,\
			       int increasement = 1);
		void getMostCount(vector<StringInt> &res);
		void disp();
		GroupCountMap();
};
#endif
