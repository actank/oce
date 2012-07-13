#include "NetworkCount.h"
NetworkCount::NetworkCount(const vector<int> &userIdList)
{
	for (int i = 0; i < userIdList.size(); i++)
	{
		int userId = userIdList.at(i);
		//cout<<"**** Network Init userId : "<< userId << "  the "<< i <<endl;
		NetworkBigObjectNPtr p = UserNetworkAdapter::instance().getBigObjectN(userId);
		//cout<< "**** Network read from ice done"<<endl;
		networkObjectMap.insert(pair<int, NetworkBigObjectNPtr>(userId, p));
		//cout<<"****  Network init:  insert done"<<endl;
	}
}

void NetworkCount::disp()
{
	map<int, NetworkBigObjectNPtr>::iterator it;
	for(it = networkObjectMap.begin(); it != networkObjectMap.end(); it ++)
	{
		if(it->second->universitys.size() != 0)
		cout<<it->second->universitys.at(0)->universityName<<endl;
	}
}

void NetworkCount::countUniv(const vector<int> &userList, vector<StringInt> &res)
{
	GroupCountMap countUniv;
	for (int i = 0; i < userList.size(); i++) 
	{
		int userId = userList.at(i);
		map<int, NetworkBigObjectNPtr>::iterator it;
		it = networkObjectMap.find(userId);
		if (it != networkObjectMap.end())
		{
			UniversityInfoSeq &univList = it->second->universitys;
			for ( int j = 0; j < univList.size(); j ++)
			{
				UniversityInfoPtr &univInfo = univList.at(j);
				if(!univInfo)
					continue;
				vector<string> element;
				element.push_back(univInfo->department);
				element.push_back(univInfo->dorm);
				char tmpstr[25];
				sprintf(tmpstr, "%d", univInfo->enrollYear);
				element.push_back(string(tmpstr));
				int inc = 1;
				if (j > 0 && univInfo->universityName == \
						univList.at(j-1)->universityName)
					inc = 0;
				countUniv.insert(univInfo->universityName, element, inc);
//				cout<< univInfo->department <<"   "<<univInfo->dorm<<endl;
			}
		}
	}
	countUniv.getMostCount(res);
	return ;
}
void NetworkCount::countHighSchool(const vector<int> &userList, vector<StringInt> &res)
{
	GroupCountMap countHigh;
	for (int i=0; i< userList.size(); i++)
	{
		int userId = userList.at(i);
		map<int, NetworkBigObjectNPtr>::iterator it;
		it = networkObjectMap.find(userId);
		if (it != networkObjectMap.end())
		{
			HighSchoolInfoSeq &highList = it->second->highschools;
			for ( int j = 0; j < highList.size(); j++)
			{
				HighSchoolInfoPtr &highPtr = highList.at(j);
				vector<string> element;
				element.push_back(highPtr->hClass1);
				element.push_back(highPtr->hClass2);
				element.push_back(highPtr->hClass3);
				countHigh.insert(highPtr->HighSchoolName, element);
			}
		}
	}
	countHigh.getMostCount(res);
	return;
}
void NetworkCount::countElementarySchool(const vector<int> &userList, vector<StringInt> &res)
{
	StringCountMap eleCount;
	for (int i = 0; i < userList.size(); i++)
	{
		int userId = userList.at(i);
		map<int, NetworkBigObjectNPtr>::iterator it;
		it = networkObjectMap.find(userId);
		if (it != networkObjectMap.end())
		{
			ElementarySchoolInfoPtr &elePtr = it->second->elementary;
			if(elePtr)
			{
				eleCount.insert(elePtr->elementarySchoolName);
			}
		}
	}
	res.push_back(StringInt(eleCount.stringForMost, eleCount.countStringMost));
	return;
}
void NetworkCount::countJuniorHighSchool(const vector<int> &userList, vector<StringInt> &res)
{
	StringCountMap juniorCount;
	for (int i = 0; i < userList.size(); i++)
	{
		int userId = userList.at(i);
		map<int, NetworkBigObjectNPtr>::iterator it;
		it = networkObjectMap.find(userId);
		if (it != networkObjectMap.end())
		{
			JuniorHighSchoolInfoPtr &juniorPtr = it->second->juniorhighschool;
			if(juniorPtr)
			{
				juniorCount.insert(juniorPtr->juniorHighSchoolName);
			}
		}
	}
	res.push_back(StringInt(juniorCount.stringForMost, juniorCount.countStringMost));
	return;
}
void NetworkCount::countWorkplace(const vector<int> &userList, vector<StringInt> &res)
{
	StringCountMap workCount;
	for (int i = 0; i < userList.size(); i++)
	{
		int userId = userList.at(i);
		map<int, NetworkBigObjectNPtr>::iterator it;
		it = networkObjectMap.find(userId);
		if (it != networkObjectMap.end())
		{
			WorkspaceInfoSeq &workList = it->second->workspaces;
			for ( int j = 0; j < workList.size(); j ++)
			{
				WorkspaceInfoPtr &workPtr = workList.at(j);
				if(workPtr)
				{
					workCount.insert(workPtr->workplaceName);
				}
			}
		}
	}
	res.push_back(StringInt(workCount.stringForMost, workCount.countStringMost));
	return;
}
