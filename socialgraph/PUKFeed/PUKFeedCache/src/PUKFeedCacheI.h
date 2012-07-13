#ifndef _PUKFEED_CACHEI_H_
#define _PUKFEED_CACHEI_H_

#include <PUKFeedCache.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"
#include <ObjectCacheI.h>
#include <QueryRunner.h>

#include "PUKFeedDBHelper.h"

namespace xce {
namespace socialgraph {

const static int PUK_DATA = 1;

//-----------------------------------------------------------------------------

class PUKFeedData : virtual public Ice::Object, virtual IceUtil::Mutex {
public:
	PUKFeedData(::Ice::Int ownerId, const PUKFeedSeq& feedList) :
		_ownerId(ownerId), _feedList(feedList) {};
	PUKFeedSeq Get(int limit = -1);
	void Remove(const PUKFeedSeq& feedList);

private:
	::Ice::Int _ownerId;
	PUKFeedSeq _feedList;
};
typedef IceUtil::Handle<PUKFeedData> PUKFeedDataPtr;

//-----------------------------------------------------------------------------

class PUKFeedCacheI : virtual public PUKFeedCache,
		virtual public MyUtil::Singleton<PUKFeedCacheI> {
public:
    virtual void Set(::Ice::Int, const PUKFeedSeq&, const Ice::Current&);
    virtual PUKFeedSeq Get(::Ice::Int, ::Ice::Int, const Ice::Current&);
    virtual PUKFeedSeq GetAndRemove(::Ice::Int, ::Ice::Int, const Ice::Current&);
    virtual void Remove(::Ice::Int, const PUKFeedSeq&, const Ice::Current&);	

private:
	PUKFeedDataPtr FindPUKFeedData(::Ice::Int id);
	friend class MyUtil::Singleton<PUKFeedCacheI>;
};

//-----------------------------------------------------------------------------

class PUKFeedFactory : virtual public MyUtil::ServantFactory {
};

//-----------------------------------------------------------------------------

class RemoveTask : virtual public MyUtil::Task {
public:
	RemoveTask(Ice::Int userId, const PUKFeedSeq& feedList):
		_userId(userId), _feedList(feedList) {}
	virtual void handle();

private:
	Ice::Int _userId;
	PUKFeedSeq _feedList;
};

}
}

#endif
