#include "GroupCountMap.h"
GroupCountMap::GroupCountMap()
{
	countForMost = 0;
}

void GroupCountMap::insert(string &indexName, vector<string> &elementNames, \
		int increasement)
{
	map<string, GroupCountElement>::iterator it;
	if (indexName.size() ==0 )
		return;
	it = countMap.find(indexName);
	if (it != countMap.end())
	{
		it->second.update(elementNames, increasement);
	}
	else
	{
		GroupCountElement groupElement(elementNames);
		countMap.insert(pair<string, GroupCountElement>(indexName, groupElement));
	}
	int count = countMap.find(indexName)->second.count;
       if (count > countForMost)
       {
	       countForMost = count;
	       nameForMost = indexName;
       }
}
void GroupCountMap::getMostCount(vector<StringInt> &res)
{
	StringInt stringInt(nameForMost, countForMost);
	res.push_back(stringInt);
	if (countForMost == 0)
		return;
	else
	{
		vector<StringCountMap> &vectorSCM = countMap.find(nameForMost)->second.stringCountMapList;
		for (int i = 0; i < vectorSCM.size(); i++)
		{
			StringCountMap &stringCountMap = vectorSCM.at(i);
			res.push_back(StringInt(stringCountMap.stringForMost, stringCountMap.countStringMost));
		}
	}
}

void GroupCountMap::disp()
{
	map<string, GroupCountElement>::iterator it;
	for( it = countMap.begin(); it != countMap.end(); it ++ )
	{
		cout<<"GroupName:  "<<it->first<<"  Count:  "<<it->second.count<<endl;
		it->second.disp();
	}
	cout<<"Group with the most people: "<< nameForMost<< "-----" << countForMost<<endl;
}
			
	

