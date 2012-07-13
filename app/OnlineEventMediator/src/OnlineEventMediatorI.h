#ifndef __OnlineEventMediatorI_h__
#define __OnlineEventMediatorI_h__

#include <OnlineEventMediator.h>
#include "IceUtil/IceUtil.h"
#include "ServiceI.h"

#include "UserCache.h"

namespace xce
{

namespace mediator
{

namespace onlineevent
{

const int TASK_LEVEL_USER_CACHE = 10;
const int TASK_LEVEL_ONLINESTATE_FILTER = 10;

const int TASK_LEVEL_DIST_USER_CACHE = 11;

class Invoker : public IceUtil::Thread{
public:
	void invoke(int userId, int online_type);
	void invoke(const MyUtil::Int2IntMap& batch_online);
	virtual void run();
private:
    MyUtil::Int2IntMap _cache;
	xce::usercache::OnlineTypeSeq _onlineVec;
    IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

class OnlineEventMediatorManagerI : virtual public OnlineEventMediatorManager, public MyUtil::Singleton<OnlineEventMediatorManagerI>
{
public:
	OnlineEventMediatorManagerI();
    virtual void sendOnlineEventSingle(::Ice::Int,
                                       ::Ice::Int,
                                       const Ice::Current&);

    virtual void sendOnlineEventBatch(const ::MyUtil::Int2IntMap&,
                                      const Ice::Current&);
private:
	Invoker _invoker;
};

class DistUserCacheNotifyTask : public MyUtil::Task {
public:
	DistUserCacheNotifyTask(const xce::usercache::OnlineTypeSeq& batch) : MyUtil::Task(TASK_LEVEL_DIST_USER_CACHE), batch_(batch) {}
	virtual void handle();
private:
	xce::usercache::OnlineTypeSeq batch_;
};

class UserCacheNotifyTask : public MyUtil::Task {
public:
	UserCacheNotifyTask(const MyUtil::Int2IntMap& batch) : MyUtil::Task(TASK_LEVEL_USER_CACHE), batch_(batch) {}
	virtual void handle();
private:
	MyUtil::Int2IntMap batch_;
};

class OnlineStateFilterNotifyTask : public MyUtil::Task {
public:
	OnlineStateFilterNotifyTask(const MyUtil::Int2IntMap& batch) : MyUtil::Task(TASK_LEVEL_ONLINESTATE_FILTER), batch_(batch) {}
	virtual void handle();
private:
	MyUtil::Int2IntMap batch_;
};

}

}

}

#endif
