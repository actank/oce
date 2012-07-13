#ifndef _GENERICSORTUTIL_H_
#define _GENERICSORTUTIL_H_
#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/shared_ptr.hpp>

#include "friendsgraph.h"

struct RelationNumItem {
	int index;
	int relation;
};

//-----------------------------------------------------------------------------------------

struct RelationNumComp {
	bool operator() (const RelationNumItem& item1, const RelationNumItem& item2) {
		return item1.relation > item2.relation;
	}
};

//-----------------------------------------------------------------------------------------

class GenericSortUtil {
public:
	static void SortFriendsOrderByRelationNum(boost::shared_ptr<FriendsGraph>& friendgraphptr, 
																						std::vector<int>& friendlist);
	static int AccountNumber(int* members, int friendsnum);
};

#endif
