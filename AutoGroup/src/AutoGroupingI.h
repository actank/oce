#ifndef AUTOGROUPINGI_H_
#define AUTOGROUPINGI_H_
#include <Ice/Ice.h>
#include <AutoGrouping.h>
using namespace com::xiaonei::socialgraph;

#include <iostream.h>
#include <vector.h>
#include <map.h>
#include <stdio.h>
#include <BuddyCacheByIdAdapter.h>
using namespace xce::buddy;

#include <UserCacheAdapter.h>
using namespace xce::usercache;

#include <math.h>
#include "GroupFriendByInfo.h"
#include "GenericSortMember.h"

const int AUTOGROUPING_DATA = 0;

class AutoGroupingI : virtual public MyUtil::Singleton<AutoGroupingI>, virtual public AutoGrouping
{
public:
	virtual GroupResultPtr getGroup(int userId, const Ice::Current &);
	GroupResultPtr grouping(int userId);

private:
	AutoGroupingI();
	friend class MyUtil::Singleton<AutoGroupingI>;
	const float THRESHOLD1;  //好友归属阈值 0.66
	const float THRESHOLD2; //连通率 0.8
	const float THRESHOLD3; //连通图之间的相似度  0.5
	const float THRESHOLD4; //最终的连通图的连通率，用来删除坏点 0.55 
    
	const int NUMBER;   //控制输出阈值  2
	//int k;
    int** getFriends(const int userId, vector<int>& userFriend, int **friendsGraph);
    map<int, vector<int> > getPureGroup(int friendsNumber, int **friendsGraph);
	void ungroupingFriends(const vector<int>& accountMembers, int friendsNumber, int& ungroupingNumber, Group& ungroupingMember, vector<int>& userFriend);
    void addFriendsToGroup(map<int, vector<int> >& group, int **friendsGraph, int friendsNumber);
    void deleteFriendsFromGroup(map<int, vector<int> >& group, map<int, vector<int> >& originalGroup, int groupAccount, int **friendsGraph, const float Threshold, const int signal);
    void uniteSameGroup(map<int, vector<int> >& group, int groupAccount, int friendsNumber, const float Threshold);
    map<int, vector<int> > generateNewGroup(map<int, vector<int> >& group, int groupAccount);
    void accountFriends(map<int, vector<int> >& group, int friendsNumber, int& friendsAccount, float& friendsRate, int& ungroupingNumber, Group& ungroupingMember, vector<int>& userFriend);
    void outputFriends(map<int, vector<int> >& group, const int userId, vector<int>& tempUngrouping, vector<int>& userFriend, int& ungroupingNumber, int& friendsAccount, float& friendsRate, TotalGroup& totalgroup, int **friendsGraph);
	void newUniteGroupLabel(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel);
	int assumeNumeber(int friendsNumber);
	void newSortAfterUniteGroup(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, int newGroupAccount, int **friendsGraph, vector<int>& userFriend);
	void finalAccountFriends(map<int, vector<int> >& group, int friendsNumber, int& friendsAccount, float& friendsRate, int& ungroupingNumber, vector<int>& tempUngrouping, vector<int>& userFriend);
	void finalUngroupingFriends(const vector<int>& accountMembers, int& ungroupingNumber, vector<int>& tempUngrouping, vector<int>& userFriend);
	void finalAddMemberLoop(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, const int userId, int friendsNumber, int& friendsAccount, float& friendsRate, int& ungroupingNumber, vector<int>& tempUngrouping, vector<int>& userFriend);
	int finalGroupNumber(int friendsNumber);
	void addMembersByGroupSize(map<int, vector<int> >& finalNewGroup, map<int, vector<StringInt> >& groupLabel, int groupSize, const int userId, int friendsNumber, int& friendsAccount, float& friendsRate, int& ungroupingNumber, vector<int>& tempUngrouping, vector<int>& userFriend);
};
 
class AutoGroupingFactoryI: public MyUtil::ServantFactory
{
public:
    virtual Ice::ObjectPtr create(Ice::Int id);
};

class AutoGroupingFactoryCreatTask : public MyUtil::Task
{
public:
    AutoGroupingFactoryCreatTask(int userId) :_userId(userId) { };
    virtual void handle();
private:
    int _userId;
};
#endif
