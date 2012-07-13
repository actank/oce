#include "UserBasicCount.h"
UserBasicCount::UserBasicCount(const vector<int> &userIdList)
{
	for(int i = 0; i < userIdList.size(); i ++)
	{
		int userId = userIdList.at(i);
		try{
			userBasicMap[userId] =  UserAdapter::instance().getUserBasic(userId);
			map<string, string>::iterator it;
		}
		catch(exception &e)
		{}
	}

}
void UserBasicCount::countCity(const vector<int> &userIdList, vector<StringInt> &res)
{
	GroupCountMap count;
	for (int i = 0; i < userIdList.size(); i++)
	{
		int userId = userIdList.at(i);
		map<int, map<string, string> >::iterator it1 = userBasicMap.find(userId);
		if (it1 == userBasicMap.end())
			continue;
		map<string, string>::iterator it2;
		it2 = it1->second.find("HOME_PROVINCE");
		if (it2 == it1->second.end())
			continue;
		map<string, string>::iterator it3;
		it3 = it1->second.find("HOME_CITY");
		string cityname;
		if(it3 != it1->second.end())
		{
			cityname = it3->second;
		}
		vector<string> element;
		element.push_back(cityname);
		count.insert(it2->second, element);
	}
	count.getMostCount(res);
}	




