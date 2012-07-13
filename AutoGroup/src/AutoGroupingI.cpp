#include "ServiceI.h"
#include "AutoGroupingI.h"

void MyUtil::initialize()
{
    ServiceI& service = ServiceI::instance();
	service.registerObjectCache(AUTOGROUPING_DATA, "AU", new AutoGroupingFactoryI);
    service.getAdapter()->add(&AutoGroupingI::instance(), service.createIdentity("AutoGrouping", ""));
}

Ice::ObjectPtr AutoGroupingFactoryI::create(Ice::Int id)
{
	GroupResultPtr obj = new GroupResult;
	obj->isEmpty = 1;

	TaskManager::instance().execute(new AutoGroupingFactoryCreatTask(id));
    return obj;
}

void AutoGroupingFactoryCreatTask::handle()
{
	GroupResultPtr obj = AutoGroupingI::instance().grouping(_userId); 
	ServiceI::instance().addObject(obj, AUTOGROUPING_DATA, (long)_userId);
}

AutoGroupingI::AutoGroupingI():THRESHOLD1(0.66),THRESHOLD2(0.8),THRESHOLD3(0.5),THRESHOLD4(0.55),NUMBER(2)
{/*k=0;*/ }

GroupResultPtr AutoGroupingI::getGroup(int userId, const Ice::Current &)
{
	MCE_INFO("start getGroup userId : " << userId);
	return ServiceI::instance().locateObject<GroupResultPtr>(AUTOGROUPING_DATA, (long)userId);
}

GroupResultPtr AutoGroupingI::grouping(int userId)
{
	//初始化参数
	vector<int> userFriend;
	int **friendsGraph = NULL;	
	GroupResultPtr finalgroup = new GroupResult;   //最终要返回的分组结果
	
	int& friendsNumber = finalgroup->friendsNumber;
	int& friendsAccount = finalgroup->friendsAccount;
	float& friendsRate = finalgroup->friendsRate;
	int& ungroupingNumber = finalgroup->ungroupingNumber;
	Group& ungroupingMember = finalgroup->ungroupingMember;
	TotalGroup& totalgroup = finalgroup->totalGroupList;  
	finalgroup->isEmpty = 0;	

	MCE_INFO("start grouping userId : " << userId);

	//获取好友
	friendsGraph = getFriends(userId, userFriend, friendsGraph);
	friendsNumber = userFriend.size();	
	MCE_INFO("get total friendsAmount : " << friendsNumber);

	//获取全连通图
	map<int, vector<int> > group = getPureGroup(friendsNumber, friendsGraph);
	int groupAccount = group.size();	
	MCE_INFO("complete graph groupaccount : " << groupAccount);

	//下面为原始全连通图备份
	map<int, vector<int> > originalGroup(group);
	
	//向组中添加好友
	addFriendsToGroup(group, friendsGraph, friendsNumber);

	//在保证图中连通率的情况下，删除好友
	deleteFriendsFromGroup(group, originalGroup, groupAccount, friendsGraph, THRESHOLD2, 1);

	//将删除好友后的组备份
	map<int, vector<int> > beiGroup(group);

	//将相似连通图合并
	uniteSameGroup(group, groupAccount, friendsNumber, THRESHOLD3);

	//下面为重新计算图的内部连通度	
	deleteFriendsFromGroup(group, beiGroup, groupAccount, friendsGraph, THRESHOLD4, 0);
	
	//生成合并后的新组
	group = generateNewGroup(group, groupAccount);

	//统计分组好友数量
	accountFriends(group, friendsNumber, friendsAccount, friendsRate, ungroupingNumber, ungroupingMember, userFriend);

	//打印输出组员
	outputFriends(group, userId, ungroupingMember.friendsId, userFriend, ungroupingNumber, friendsAccount, friendsRate, totalgroup, friendsGraph);	

	totalgroup.push_back(ungroupingMember);       //根据产品需要，将为分组人员放在已分组人员后面

	MCE_INFO("resultgroup size : " << totalgroup.size());

    for(int i=0; i<friendsNumber; ++i)               //完成最后的内存处理
    {
        delete[] friendsGraph[i];
    }

    delete[] friendsGraph;
    friendsGraph = NULL;

	return finalgroup;
}

int** AutoGroupingI::getFriends(const int userId, vector<int>& userFriend, int **friendsGraph)
{
	userFriend = BuddyCacheByIdAdapter::instance().getFriendListOrderByIdDesc(userId);
	int friendsNumber = userFriend.size();
    /*
    for(int i=0; i<userFriend.size(); ++i)
    {
        cout<<userFriend[i]<<"  ";
    }
    cout<<endl;
   */ 
	map<int, vector<int> > friendsFriends;
    vector<int> friends;

    for(int i=0; i<friendsNumber; ++i)
    {
        friends = BuddyCacheByIdAdapter::instance().getFriendListOrderByIdDesc(userFriend[i]);
		friendsFriends.insert(pair<int, vector<int> >(userFriend[i], friends));
    }

	friendsGraph = new int *[friendsNumber];                    //要做内存处理
	for(int i=0; i<friendsNumber; ++i)
    {
        friendsGraph[i] = new int[friendsNumber];
    }

    for(int i=0; i<friendsNumber; ++i)
    {
        friendsGraph[i][i] = 0;
        for(int j=i+1; j<friendsNumber; ++j)
        {
            friendsGraph[i][j] = 0;
			friendsGraph[j][i] = 0;
        }
    }

	vector<int> tempFriends;

	for(int i=0; i<friendsNumber; ++i)
    {
        tempFriends = friendsFriends[(userFriend[i])];
        for(int j=i+1; j<friendsNumber; ++j)
        {
        	if(find(tempFriends.begin(), tempFriends.end(), userFriend[j]) != tempFriends.end())
        	{
            	friendsGraph[i][j] = 1;
            	friendsGraph[j][i] = 1;
       		}
        }
    }
	/*
	cout<<"friendsNumber : "<<friendsNumber<<endl;

	for(int i=0; i<friendsNumber; ++i)
	{
		for(int j=0; j<friendsNumber; ++j)
		{
			cout<<friendsGraph[i][j]<<" ";
		}
		cout<<endl;
	}
	*/
	//sortFriends(friendsNumber, friendsGraph, userFriend);
	GenericSortMember::sortFriends(friendsNumber, friendsGraph, userFriend);
	/*
	for(int i=0; i<userFriend.size(); ++i)
	{
		cout<<userFriend[i]<<"  ";
	}
	cout<<endl;
	*/	
	return friendsGraph;
}

map<int, vector<int> > AutoGroupingI::getPureGroup(int friendsNumber, int **friendsGraph)
{
	int groupAccount = 0;
    map<int, vector<int> > group;

	for(int i=0; i<friendsNumber; ++i)
    {
        if(groupAccount == 0)
        {
            groupAccount++;
            vector<int> groupMember;
            groupMember.push_back(i);

            group.insert(pair<int, vector<int> >(groupAccount, groupMember));

            continue;
        }

        int j;

        for(j=1; j<=groupAccount; ++j)
        {
            vector<int> groupMember = group[j];

            //int k;
			size_t k;

            for(k=0; k<groupMember.size(); ++k)
            {
                int member = groupMember[k];
                if(friendsGraph[i][member] == 0)
                {
                    break;
                }
            }

            if(k == groupMember.size())
            {
                break;
            }
        }


 		if(j > groupAccount)
        {
            groupAccount++;
            vector<int> groupMember;
            groupMember.push_back(i);

            group.insert(pair<int, vector<int> >(groupAccount, groupMember));
        }
        else
        {
            //map<int, vector<int> >::iterator iter = group.find(j);
            //vector<int>& groupMember = iter->second;
			vector<int>& groupMember = group[j];
            groupMember.push_back(i);
        }
    }
	//下面为一个人存在于多个组的算法
	for(int i=0; i<friendsNumber; ++i)
	{
		vector<int> groupMember;
		for(int j=1; j<=groupAccount; ++j)
		{
			groupMember = group[j];

			unsigned int k;

			for(k=0; k<groupMember.size(); ++k)
			{
				int member = groupMember[k];
				
				if(friendsGraph[i][member] == 0)
				{
					break;
				}
			}

			if(k == groupMember.size())
			{
				//map<int, vector<int> >::iterator iter = group.find(j);
    	        //vector<int>& groupMember = iter->second;
				vector<int>& groupMember = group[j];
	            groupMember.push_back(i);
			}
		}
	}
	//下面为只返回组成员>2的组
    map<int, vector<int> > newGroup;
    int newAccount = 0;
	unsigned int leftNumber = 2;    //根据程序没有必要计算两个人以下的全连通图

    for(int i=1; i<=groupAccount; ++i)
    {
        vector<int> groupMember = group[i];

        if(groupMember.size() > leftNumber)
        {
            newAccount++;
            newGroup.insert(pair<int, vector<int> >(newAccount, groupMember));
        }
    }

    return newGroup;
}

void AutoGroupingI::addFriendsToGroup(map<int, vector<int> >& group, int **friendsGraph, int friendsNumber)
{
	map<int, vector<int> > addMember;
	
	for(unsigned int i=1; i<=group.size(); ++i)
	{
		vector<int> members;
		addMember.insert(pair<int, vector<int> >(i, members));
	}

	for(int i=0; i<friendsNumber; ++i)
	{
		vector<int> groupMember;
		for(unsigned int j=1; j<=group.size(); ++j)
		{
			float relationshipAccount = 0;			
			float totalRelationship = 0;

			groupMember = group[j];

			if(find(groupMember.begin(), groupMember.end(), i) == groupMember.end())            
			{
				for(unsigned int k=0; k<groupMember.size(); ++k)
				{
					int temp = groupMember[k];
					if(friendsGraph[i][temp] == 1)
					{
						relationshipAccount++;
					}
				}
				totalRelationship = groupMember.size();
				float relationshipRate = relationshipAccount/totalRelationship;
		
				if(relationshipRate >= THRESHOLD1)
				{
					map<int, vector<int> >::iterator iter = addMember.find(j);
	                vector<int>& groupMember = iter->second;
    	            groupMember.push_back(i);
				}
			}
		}
	}

	//向组里添加好友
	for(unsigned int i=1; i<=group.size(); ++i)
	{
		map<int, vector<int> >::iterator iter = group.find(i);
		vector<int>& groupMember = iter->second;
		vector<int> adMembers = addMember[i];
		
		groupMember.insert(groupMember.end(), adMembers.begin(), adMembers.end());
	}
}

void AutoGroupingI::deleteFriendsFromGroup(map<int, vector<int> >& group, map<int, vector<int> >& originalGroup, int groupAccount, int **friendsGraph, const float Threshold, const int signal)
{
	//在保证图中连通率的情况下，删除好友
	for(int i=1; i<=groupAccount; ++i)
	{
		if(group.find(i) != group.end())
		{
			map<int, vector<int> >::iterator iter = group.find(i);
	        vector<int>& groupMembers = iter->second;
			
			float totalRelationship = groupMembers.size()*(groupMembers.size() - 1);
			float relationshipAccount = 0;

			for(unsigned int j=0; j<groupMembers.size(); ++j)
			{
				int temp1 = groupMembers[j];
				for(unsigned int k=j+1; k<groupMembers.size(); ++k)
				{
					int temp2 = groupMembers[k];
					
					if(friendsGraph[temp1][temp2] == 1)
					{
						relationshipAccount += 2;
					}
				}
			}
			
			float relationshipRate = relationshipAccount/totalRelationship;	
	
			float Ythreshold = 0;

            if(signal == 0)
            {
                int Xnumber = groupMembers.size();

                if(Xnumber < 4)
                {
                    continue;
                }

                Ythreshold = 0.6*exp(-0.0038*Xnumber);
            }
            else
            {
                Ythreshold = Threshold;
            }

			if(relationshipRate < Ythreshold)
			{
				int minNode = 0;
				int minValue = groupMembers.size();

				vector<int> originalGroupMember = originalGroup[i];
				
				for(unsigned int k1=0; k1<groupMembers.size(); ++k1)
				{
					int temp1 = groupMembers[k1];
					if(find(originalGroupMember.begin(), originalGroupMember.end(), temp1) == originalGroupMember.end())
					{
						int relationshipAccount2 = 0;
						for(unsigned int k2=0; k2<groupMembers.size(); ++k2)
						{
							int temp2 = groupMembers[k2];
							if(friendsGraph[temp1][temp2] == 1)
							{
								relationshipAccount2++;
							}
						}

						if(relationshipAccount2 < minValue)
						{
							minValue = relationshipAccount2;
							minNode = k1;
						}
					}
				}
				groupMembers.erase(groupMembers.begin() + minNode);			
			
				i--;
			}			
		}
	}	
}

void AutoGroupingI::uniteSameGroup(map<int, vector<int> >& group, int groupAccount, int friendsNumber, const float Threshold)
{
	int groupSize = group.size();

    int **friendsGroupGraph = new int *[friendsNumber];                 //做内存处理
    for(int i=0; i<friendsNumber; ++i)
    {
        friendsGroupGraph[i] = new int[groupSize + 1];
    }

    for(int i=0; i<friendsNumber; ++i)
    {
        for(int j=0; j<groupSize+1; ++j)
        {
            friendsGroupGraph[i][j] = 0;
        }
    }
	/*
	int tmp = 0;

    for(int i=1; i<=group.size(); ++i)
    {
        vector<int> tmpMember = group[i];
        for(int j=0; j<tmpMember.size(); ++j)
        {
            tmp = tmpMember[j];
            friendsGroupGraph[tmp][i] = 1;
        }
    }
	*/
	for(map<int, vector<int> >::iterator mIt=group.begin(); mIt!=group.end(); ++mIt)
	{
		for(vector<int>::iterator sIt=mIt->second.begin(); sIt!=mIt->second.end(); ++sIt)
		{
			friendsGroupGraph[*sIt][mIt->first] = 1;	
		}
	}	

    int *enableGroup = new int[groupAccount+1];
    for(int i=0; i<groupAccount+1; ++i)
    {
        enableGroup[i] = 1;
    }

    int **sameElementArray = new int *[groupAccount+1];          
    for(int i=1; i<groupAccount+1; ++i)
    {
        sameElementArray[i] = new int[groupAccount+1];
    }

    for(int i=1; i<groupAccount+1; ++i)
    {
        for(int j=1; j<groupAccount+1; ++j)
        {
            sameElementArray[i][j] = 0;
        }
    }
    bool first = true;

    while(true)
    {
        float sameElementAccount = 0;
        float totalElementNumber = 0;
        float maxElementRate = 0;
        int maxSameGroupX = 0;
        int maxSameGroupY = 0;

	    vector<int> groupMembers1;
        vector<int> groupMembers2;

        for(int i=1; i<=groupAccount; ++i)
        {
			if(!enableGroup[i])
			{
				continue;
			}

			groupMembers1 = group[i];

			for(int j=i+1; j<=groupAccount; ++j)
			{
				if(!enableGroup[j])
				{
					continue;
				}

				sameElementAccount = 0;
				totalElementNumber = 0;
				groupMembers2 = group[j];
			
				if(first)
				{
					for(unsigned int k=0; k<groupMembers2.size(); ++k)                           
                	{
        	    		int temp = groupMembers2[k];
						if(friendsGroupGraph[temp][i] == 1)
						{
							++sameElementAccount;
						}
              		}
					sameElementArray[i][j] = (int)sameElementAccount;
				}
				else
				{
					sameElementAccount = sameElementArray[i][j];					
				}
				//    和两个集合中较小的比较
				totalElementNumber = min(groupMembers1.size(), groupMembers2.size());				

				float sameElementRate = sameElementAccount/totalElementNumber; 
					
				if(sameElementRate > maxElementRate)
				{
					maxElementRate = sameElementRate;
					maxSameGroupX = i;
					maxSameGroupY = j;
				}
			}
        }

		if(maxElementRate > Threshold)     //to-do
		{
			first = false;
			map<int, vector<int> >::iterator iter = group.find(maxSameGroupX);
            vector<int>& groupMember1 = iter->second;
			vector<int> groupMember2 = group[maxSameGroupY];

			for(unsigned int e=0; e<groupMember2.size(); ++e)            //has been done
			{
				int temp = groupMember2[e];
				
				if(friendsGroupGraph[temp][maxSameGroupX] == 0)
				{
					groupMember1.push_back(temp);
					friendsGroupGraph[temp][maxSameGroupX] = 1;
				}
			}

			group.erase(maxSameGroupY);	
			enableGroup[maxSameGroupY] = 0;	

			for(int i=1; i<=groupAccount; ++i)
			{
				if(!enableGroup[i] || i == maxSameGroupX)
				{
					continue;
				}

				sameElementAccount = 0;
				/*
              	for(int k=0; k<groupMember1.size(); ++k)
       	        {
					int tmpMem = groupMember1[k];
                	if(friendsGroupGraph[tmpMem][i])
					{
                   		++sameElementAccount;
					}
            	}
				*/	
				for(vector<int>::iterator iter=groupMember1.begin(); iter!=groupMember1.end(); ++iter)
				{
					if(friendsGroupGraph[*iter][i])
					{
                   		++sameElementAccount;
					}
				}

				sameElementArray[min(i,maxSameGroupX)][max(i,maxSameGroupX)] = (int)sameElementAccount;
			}			
		}
		else
		{
			break;
		}
    }

    for(int i=0; i<friendsNumber; ++i)               //完成内存处理
    {
        delete[] friendsGroupGraph[i];
    }

    delete[] friendsGroupGraph;
    friendsGroupGraph = NULL;

	delete[] enableGroup;

	for(int i=1; i< groupAccount+1; ++i)
    {
        delete[] sameElementArray[i];
    }
    delete[] sameElementArray;
    sameElementArray = NULL;
}

map<int, vector<int> > AutoGroupingI::generateNewGroup(map<int, vector<int> >& group, int groupAccount)
{
	int newGroupAccount = 0;
	map<int, vector<int> > newGroup;
	for(int i=1; i<=groupAccount; ++i)
	{
		if(group.find(i) != group.end() && group[i].size() > (size_t)NUMBER)
		{
			newGroupAccount++;
			newGroup.insert(pair<int, vector<int> >(newGroupAccount, group[i]));
		}
	}
	return newGroup;
}

void AutoGroupingI::accountFriends(map<int, vector<int> >& group, int friendsNumber, int& friendsAccount, float& friendsRate, int& ungroupingNumber, Group& ungroupingMember, vector<int>& userFriend)
{
	//下面为计算已分组好友占总数的比重
	vector<int> accountMembers;

    for(unsigned int i=1; i<=group.size(); ++i)    //has been done
    {
        vector<int> groupMember = group[i];

       	for(unsigned int j=0; j<groupMember.size(); ++j)
        {
        	int member = groupMember[j];

        	if(find(accountMembers.begin(), accountMembers.end(), member) == accountMembers.end())
          	{
            	accountMembers.push_back(member);
            }
       	}
    }

	friendsAccount = accountMembers.size();
	
	friendsRate = (float)friendsAccount/friendsNumber;

	ungroupingFriends(accountMembers, friendsNumber, ungroupingNumber, ungroupingMember, userFriend);
}

void AutoGroupingI::ungroupingFriends(const vector<int>& accountMembers, int friendsNumber, int& ungroupingNumber, Group& ungroupingMember, vector<int>& userFriend)
{
    for(int i=0; i<friendsNumber; ++i)
    {
        if(find(accountMembers.begin(), accountMembers.end(), i) == accountMembers.end())
        {
        	ungroupingMember.friendsId.push_back(userFriend[i]);
        }
    }

	ungroupingNumber = ungroupingMember.friendsId.size();

	ungroupingMember.groupName = "您未分组的成员为:";	
}

void AutoGroupingI::outputFriends(map<int, vector<int> >& group, const int userId, vector<int>& tempUngrouping, vector<int>& userFriend, int& ungroupingNumber, int& friendsAccount, float& friendsRate, TotalGroup& totalgroup, int **friendsGraph)
{
	int friendsNumber = userFriend.size();	

	map<int, vector<int> > finalNewGroup;

    for(unsigned int i=1; i<=group.size(); ++i)
    {
        vector<int> finalNewGroupMember;
        vector<int> newGroupMember = group[i];

        for(unsigned int j=0; j<newGroupMember.size(); ++j)
        {
           	int member = newGroupMember[j];
           	finalNewGroupMember.push_back(userFriend[member]);
       	}

       	finalNewGroup.insert(pair<int, vector<int> >(i, finalNewGroupMember));
    }
	
	int newGroupAccount = finalNewGroup.size();
	
	//cout<<"开始取标签数据"<<endl;
    map<int, vector<StringInt> > groupLabel;

    for(unsigned int i=1; i<=finalNewGroup.size(); ++i)
    {
        vector<StringInt> tag;
        GroupLabel::Label(finalNewGroup[i], userId, tag);

        groupLabel.insert(pair<int, vector<StringInt> >(i, tag));
    }

    //按标签合并分组
    //newUniteGroupLabel(finalNewGroup, groupLabel);
	GroupFriendByInfo::newUniteGroupLabel(finalNewGroup, groupLabel);

	//输出好友前的排序
	//newSortAfterUniteGroup(finalNewGroup, groupLabel, newGroupAccount, friendsGraph, userFriend);
	GenericSortMember::newSortAfterUniteGroup(finalNewGroup, groupLabel, newGroupAccount, friendsGraph, userFriend);

	//根据好友资料向已有组中添加好友
	//addMembersByInfo(finalNewGroup, groupLabel, tempUngrouping, userId);
	GroupFriendByInfo::addMembersByInfo(finalNewGroup, groupLabel, tempUngrouping, userId);

	//已分组，未分组好友分别计数
	finalAccountFriends(finalNewGroup, friendsNumber, friendsAccount, friendsRate, ungroupingNumber, tempUngrouping, userFriend);

	//根据组数判断是否需要重新添加
	finalAddMemberLoop(finalNewGroup, groupLabel, userId, friendsNumber, friendsAccount, friendsRate, ungroupingNumber, tempUngrouping, userFriend);

	//输出组按组中成员的多少排序输出
	//sortGroupBySize(finalNewGroup, groupLabel);
	GenericSortMember::sortGroupBySize(finalNewGroup, groupLabel);

	//返回排好序的结果
	for(unsigned int i=1; i<=finalNewGroup.size(); ++i)
	{
		Group friendsGroup;
		vector<int> finalNewGroupMember = finalNewGroup[i];

		for(unsigned int j=0; j<finalNewGroupMember.size(); ++j)
		{
			int tempMember = finalNewGroupMember[j];
			friendsGroup.friendsId.push_back(tempMember);
		}

		vector<StringInt> tmpTag = groupLabel[i];
		
		for(unsigned int k=0; k<tmpTag.size(); ++k)
		{
			friendsGroup.groupName += tmpTag.at(k).name;
			friendsGroup.groupName += " ";
		}
		totalgroup.push_back(friendsGroup);
	}
}

int AutoGroupingI::assumeNumeber(int friendsNumber)
{
    if(friendsNumber <= 500)
    {
        return 10;
    }
    else if(500 < friendsNumber <= 800)
    {
        return 12;
    }
    else
    {
        return 14;
    }
}

void AutoGroupingI::finalAccountFriends(map<int, vector<int> >& group, int friendsNumber, int& friendsAccount, float& friendsRate, int& ungroupingNumber, vector<int>& tempUngrouping, vector<int>& userFriend)
{
    //下面为计算已分组好友占总数的比重
    vector<int> accountMembers;

    for(unsigned int i=1; i<=group.size(); ++i)
    {
        vector<int> groupMember = group[i];

        for(unsigned int j=0; j<groupMember.size(); ++j)          //has been done
        {
            int member = groupMember[j];

            if(find(accountMembers.begin(), accountMembers.end(), member) == accountMembers.end())
            {
                accountMembers.push_back(member);
            }
        }
    }

	friendsAccount = accountMembers.size();

	friendsRate = (float)friendsAccount/friendsNumber;

	finalUngroupingFriends(accountMembers, ungroupingNumber, tempUngrouping, userFriend);
}

void AutoGroupingI::finalUngroupingFriends(const vector<int>& accountMembers, int& ungroupingNumber, vector<int>& tempUngrouping, vector<int>& userFriend)
{
	int friendsNumber = userFriend.size();
    tempUngrouping.clear();

    for(int i=0; i<friendsNumber; ++i)     //has been done
    {
        if(find(accountMembers.begin(), accountMembers.end(), userFriend[i]) == accountMembers.end())
        {
			tempUngrouping.push_back(userFriend[i]);
        }
    }
	
	ungroupingNumber = tempUngrouping.size();
}

void AutoGroupingI::finalAddMemberLoop(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, const int userId, int friendsNumber, int& friendsAccount, float& friendsRate, int& ungroupingNumber, vector<int>& tempUngrouping, vector<int>& userFriend)
{
	size_t finalNumber = finalGroupNumber(friendsNumber);

	if(finalNewGroup.size() <= finalNumber)
	{
		return;
	}
	else
	{
		addMembersByGroupSize(finalNewGroup, groupLabel, finalNumber, userId, friendsNumber,  friendsAccount, friendsRate, ungroupingNumber, tempUngrouping, userFriend);
	}
}

int AutoGroupingI::finalGroupNumber(int friendsNumber)
{
	int number = 0;

	if(friendsNumber < 500)
	{
		number = 8;		
	}
	else if(500 <= friendsNumber && friendsNumber < 800)
	{
		number = 9;
	}
	else
	{
		number = 10;
	}

	return number;
}

void AutoGroupingI::addMembersByGroupSize(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, int groupSize, const int userId, int friendsNumber, int& friendsAccount, float& friendsRate, int& ungroupingNumber, vector<int>& tempUngrouping, vector<int>& userFriend)
{
	int originalNumber = finalNewGroup.size();
	for(int i=groupSize+1; i<=originalNumber; ++i)
	{
		finalNewGroup.erase(i);
		groupLabel.erase(i);
	}
	finalAccountFriends(finalNewGroup, friendsNumber, friendsAccount, friendsRate, ungroupingNumber, tempUngrouping, userFriend);

	//addMembersByInfo(finalNewGroup, groupLabel, tempUngrouping, userId);
	GroupFriendByInfo::addMembersByInfo(finalNewGroup, groupLabel, tempUngrouping, userId);	
	
	finalAccountFriends(finalNewGroup, friendsNumber, friendsAccount, friendsRate, ungroupingNumber, tempUngrouping, userFriend);
}

