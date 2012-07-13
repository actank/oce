#ifndef GROUPLABEL_GROUPCOUNTELEMENT_H
#define GROUPLABEL_GROUPCOUNTELEMENT_H
#include "StringCountMap.h"
#include <vector>
using namespace std;
class GroupCountElement {
	public:
	       int count;
	       vector<StringCountMap> stringCountMapList;
		GroupCountElement(vector<string> &nameList);
		void disp();
		void update(vector<string> &nameList, int increasement);
};
#endif
