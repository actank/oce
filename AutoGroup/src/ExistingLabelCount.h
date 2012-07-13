#ifndef GROUPLABEL_EXISTINGLABEL_COUNT
#define GROUPLABEL_EXISTINGLABEL_COUNT
#include "StringCountMap.h"
#include "StringInt.h"
#include "BuddyGroupAdapter.h"
#include <vector>
#include <map>
using namespace mop::hi::svc::adapter;
class ExistingLabelCount{
	public:
	map<int, vector<string> > existingLabelMap;
	ExistingLabelCount(int host);
	void countLabel(const vector<int> &friendsList, vector<StringInt> &res);
	void testdisp();
};

#endif
