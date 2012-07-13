#ifndef _FRIENDFINDER_BYACTIVEUSER_H_
#define _FRIENDFINDER_BYACTIVEUSER_H_

#include "FriendFinderByActiveUser.h"

#include <bitset>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"
#include <ObjectCacheI.h>
#include <QueryRunner.h>
#include <BuddyByIdReplicaAdapter.h>
#include <FriendFinderInfoCacheReplicaAdapter.h>
#include <FriendRankCacheAdapter.h>
#include <HotFeedRecommendCacheAdapter.h>
#include "RelationBatchHelper.h"

namespace xce {
namespace friendfinder {
const static int USER_LOGIN_BACKEND = 3;
const static int USER_INDEX_BACKEND = 3; 

const static int USER_LOGIN = 1;
const static int USER_INDEX = 1; 

const static int NEGTIVEDAYS = 15;         //15内登录0次或1次
const static int ACTIVEDAYS = 4;          //一个月内有4天登录

const static int RECOMMEND_DATA = 2;

const static int rebuildInterval = 24*3600;

const static int REBUILD_THRESHOLD = 10;

const std::string REMOVE_WITH_BLOCK = "REMOVE_WITH_BLOCK";

const int LEVEL_REBUILD = 1;
const int LEVEL_CREATE = 2;
const int LEVEL_REMOVE = 3;

//**********************************************************************************

class RecommendData : virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	RecommendData(::Ice::Int ownerId, const MyUtil::Int2IntSeqMap& recommendMap, const MyUtil::Int2IntMap& friendrank) : 
		_ownerId(ownerId), _recommendMap(recommendMap), _friendrank(friendrank), _timeStamp(time(NULL)) { };

	MyUtil::Int2IntSeqMap get(int limit = -1);
	MyUtil::Int2IntSeqMap getRandom(int limit = -1);
	MyUtil::Int2IntSeqMap getRandomByFriendRank(int limit = -1);
	MyUtil::IntSeq getWriteRecommendRandom(int limit = -1);
	MyUtil::IntSeq getFriendsRecommendRandom(int limit = -1);
	MyUtil::IntSeq getRecommendFriendList(int friendId, int begin, int limit = -1);	

	void removeBatch(const MyUtil::IntSeq& idSeq);
	void checkTimeout();	

private:
	::Ice::Int _ownerId;
	MyUtil::Int2IntSeqMap _recommendMap;
	MyUtil::Int2IntMap _friendrank;
	time_t _timeStamp;
};
typedef IceUtil::Handle<RecommendData> RecommendDataPtr;

//**********************************************************************************

class FriendFinderByActiveUserI : virtual public FriendFinderByActiveUser,
		virtual public MyUtil::Singleton<FriendFinderByActiveUserI> {
public:
	virtual void setBitMap(const MyUtil::Int2IntMap&, const Ice::Current&);
    virtual void setNegtiveList(const MyUtil::IntSeq&, const Ice::Current&);
	virtual void clearUserLoginCache(const Ice::Current&);

	virtual MyUtil::Int2IntSeqMap getRecommend(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual MyUtil::Int2IntSeqMap getRecommendRandom(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual MyUtil::IntSeq getWriteRecommendRandom(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual MyUtil::IntSeq getFriendsRecommendRandom(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual MyUtil::IntSeq getRecommendFriendList(::Ice::Int, ::Ice::Int, ::Ice::Int, ::Ice::Int, const Ice::Current&);

	virtual void removeRecommendFriends(::Ice::Int, ::Ice::Int, const MyUtil::IntSeq&, const Ice::Current&); 

	virtual MyUtil::IntSeq getActiveUser(::Ice::Int, const MyUtil::IntSeq&, const Ice::Current&);

protected:
	RecommendDataPtr locateRecommendData(::Ice::Int id);

	friend class MyUtil::Singleton<FriendFinderByActiveUserI>;
};

//**********************************************************************************

class UserLoginFactory : virtual public MyUtil::ServantFactory {
};

//**********************************************************************************

class RecommendFriendsFactory : virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

//**********************************************************************************

class UserLoginData : virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	void setLoginMap(const MyUtil::Int2IntMap& loginMap);
	void setNegtiveUserList(const MyUtil::IntSeq& userList);
	MyUtil::Int2IntSeqMap getRecommendFriends(int userId);
	MyUtil::IntSeq getActiveUser(const MyUtil::IntSeq& userList);
	~UserLoginData();

private:
	void AppendUserFriends(MyUtil::Int2IntSeqMap& result, const MyUtil::IntSeq& userFriends, const MyUtil::Int2IntSeqMap& negtiveFriendsMap);
	void AppendSingleRelationFriend(int userId, MyUtil::Int2IntSeqMap& result);

	IceUtil::RWRecMutex _init_flag_mutex;

	typedef std::map< int, std::bitset<32> > UserLoginMapType;
	UserLoginMapType _userLoginMap;

	typedef std::set<int> NegtiveUserListType;
	NegtiveUserListType _negtiveUserList;	
};
typedef IceUtil::Handle<UserLoginData> UserLoginDataPtr;

//**********************************************************************************

class CreateTask: virtual public MyUtil::Task {
public:
	CreateTask(Ice::Int userId, Ice::Int level=LEVEL_CREATE): 	
		Task(level), _userId(userId) { };
	virtual void handle();

private:
	Ice::Int _userId;
};

//**********************************************************************************

class BuildTask: virtual public MyUtil::Task {
public: 
	BuildTask(Ice::Int userId, Ice::Int level=LEVEL_REBUILD): 
		Task(level), _userId(userId) { };
	virtual void handle();

private:
	Ice::Int _userId;
};

//**********************************************************************************

class RemoveTask: virtual public MyUtil::Task {
public: 
	RemoveTask(Ice::Int userId, Ice::Int friendId, const MyUtil::IntSeq& blockIds, bool block, Ice::Int level=LEVEL_REMOVE): 
		Task(level), _userId(userId), _friendId(friendId), _blockIds(blockIds) { };
	virtual void handle();

private:
	Ice::Int _userId;
	Ice::Int _friendId;
	MyUtil::IntSeq _blockIds;
	bool _block;
};

}
}

#endif 
