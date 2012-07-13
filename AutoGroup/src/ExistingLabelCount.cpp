#include "ExistingLabelCount.h"
ExistingLabelCount::ExistingLabelCount(int host)
{
	existingLabelMap = BuddyGroupAdapter::instance().getBuddiedGroupsMap(host);
}
void ExistingLabelCount::testdisp()
{
	map<int, vector<string> >::iterator it;
	for (it = existingLabelMap.begin(); it != existingLabelMap.end(); it ++)
	{
		cout << " ("<<it->first;
		for (int i = 0; i< it->second.size(); i ++)
		{
			cout<< ", "<<it->second.at(i);
		}
		cout << ")"<<endl;
	}
}
void ExistingLabelCount::countLabel(const vector<int> &friendsList, vector<StringInt> &res)
{
	StringCountMap labelCount;
	map<int, vector<string> >::iterator it;
	for(int i = 0; i<friendsList.size(); i++)
	{
		int userId = friendsList.at(i);
		it = existingLabelMap.find(userId);
		if(it == existingLabelMap.end())
			continue;
		for(int j = 0; j < it->second.size(); j++)
			labelCount.insert(it->second.at(j));
	}
	res.push_back(StringInt(labelCount.stringForMost, labelCount.countStringMost));
}

