#ifndef __RecentlyOnlineBuddyI_h__
#define __RecentlyOnlineBuddyI_h__

#include <RecentlyOnlineBuddy.h>

#include "ObjectCacheI.h"
#include "TaskManager.h"

#include "OnlineTimeCache.h"

namespace xce {

namespace recentlyonlinebuddy {

static const int CATEGORY_DATA = 0;

static const int TASK_LEVEL_BUDDY_CREATE = 1;
static const int TASK_LEVEL_BUDDY_ADD = 2;
static const int TASK_LEVEL_BUDDY_REMOVE = 3;
static const int TASK_LEVEL_BUDDY_SO = 4;
static const int TASK_LEVEL_BUDDY_NOTIFY_SO=5;

class RecentlyOnlineBuddyData: virtual public Ice::Object,
		virtual public IceUtil::Mutex {
public:
	MyUtil::IntSeq get_most_recent(int limit);
	Ice::Int get_count();
	void add_old_online(const MyUtil::IntSeq& sorted_buddies);
	void add_recent_online(int userId);
	void remove_online(int userId);
private:
	std::deque<int> _buddies;
};
typedef IceUtil::Handle<RecentlyOnlineBuddyData> RecentlyOnlineBuddyDataPtr;

class RecentlyOnlineBuddyManagerI: virtual public RecentlyOnlineBuddyManager {
public:

	virtual ::MyUtil::IntSeq get(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual ::Ice::Int getCount(::Ice::Int, const Ice::Current&);

	virtual void add(::Ice::Int,::Ice::Int,const Ice::Current&);
	virtual void remove(::Ice::Int,::Ice::Int,const Ice::Current&);
	virtual void rebuild(::Ice::Int, const Ice::Current&);

	virtual void somebodyOnline(::Ice::Int theOnlineOne, const Ice::Current&);
	virtual void notifySomebodyOnline(const ::MyUtil::IntSeq& buddies,
			::Ice::Int theOnlineOne, const Ice::Current& current);
private:
	RecentlyOnlineBuddyDataPtr locateRecentlyOnlineBuddyData(::Ice::Int);
};

class RecentlyOnlineBuddyDataFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class CreateTask: public MyUtil::Task{
public:
	CreateTask(Ice::Int id,int level=TASK_LEVEL_BUDDY_CREATE) :
		Task(level), _id(id){
		}
	virtual void handle();
private:
        Ice::Int _id;
};

class AddTask: virtual public MyUtil::Task {
public:
	AddTask(Ice::Int userId,Ice::Int friendId,int level=TASK_LEVEL_BUDDY_ADD) :
		Task(level), _userId(userId), _friendId(friendId) {
	}
	virtual void handle();
private:
	Ice::Int _userId;
	Ice::Int _friendId;
};

class RemoveTask: virtual public MyUtil::Task {
public:
	RemoveTask(Ice::Int userId,Ice::Int friendId,int level=TASK_LEVEL_BUDDY_REMOVE) :
		Task(level), _userId(userId), _friendId(friendId) {
	}
	virtual void handle();
private:
	Ice::Int _userId;
	Ice::Int _friendId;
};

class SomebodyOnlineTask: virtual public MyUtil::Task {
public:
	SomebodyOnlineTask(::Ice::Int theOnlineOne) :
		MyUtil::Task(TASK_LEVEL_BUDDY_SO), _theOnlineOne(theOnlineOne) {
	}
	virtual void handle();
private:
	::Ice::Int _theOnlineOne;
};

class NotifySomebodyOnlineTask: virtual public MyUtil::Task {
public:
	NotifySomebodyOnlineTask(const ::MyUtil::IntSeq& buddies,
			::Ice::Int theOnlineOne) :
		MyUtil::Task(TASK_LEVEL_BUDDY_NOTIFY_SO), _buddies(buddies), _theOnlineOne(theOnlineOne) {
	}
	virtual void handle();
private:
	::MyUtil::IntSeq _buddies;
	::Ice::Int _theOnlineOne;
};

}
}

#endif
