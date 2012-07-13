#ifndef _HOTFEED_RECOMMENDCACHEI_H_
#define _HOTFEED_RECOMMENDCACHEI_H_

#include <HotFeedRecommendCache.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"
#include <ObjectCacheI.h>
#include <QueryRunner.h>

#include "HotFeedDBHelper.h"

namespace xce {
namespace socialgraph {

const int HOT_FEED_DATA = 1;
const int INTERVAL = 7 * 24 * 3600;    //数据7天后过期

//*******************************************************************************

class HotFeedData : virtual public Ice::Object, virtual IceUtil::Mutex {
public:
	HotFeedData(::Ice::Int ownerId, const HotFeedSeq& hotFeedList):
		_ownerId(ownerId), _hotFeedList(hotFeedList), _timestamp(time(NULL)) {};
	~HotFeedData() {}

	HotFeedSeq Get(::Ice::Int ownerId, int limit = 1);
	void Remove(int ownerId, const HotFeedSeq& hotFeedList);

private:
	bool CheckTimeout();

	::Ice::Int _ownerId;
	HotFeedSeq _hotFeedList;	
	time_t _timestamp;
};
typedef IceUtil::Handle<HotFeedData> HotFeedDataPtr;

//*******************************************************************************

class HotFeedRecommendCacheI : virtual public HotFeedRecommendCache,
		virtual public MyUtil::Singleton<HotFeedRecommendCacheI> {
public:
	virtual void Set(::Ice::Int, const HotFeedSeq&, const Ice::Current&);
	virtual HotFeedSeq GetAndRemove(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual HotFeedSeq Get(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual void Remove(::Ice::Int, const HotFeedSeq&, const Ice::Current&);

private:
	HotFeedDataPtr FindHotFeedData(::Ice::Int id);
	friend class MyUtil::Singleton<HotFeedRecommendCacheI>;
};

//*******************************************************************************

class HotFeedFactory : virtual public MyUtil::ServantFactory {
};

//*******************************************************************************

class RemoveTask : virtual public MyUtil::Task {
public:
	RemoveTask(Ice::Int ownerId, const HotFeedSeq& hotFeedList):
		_ownerId(ownerId), _hotFeedList(hotFeedList) { };
	virtual void handle();

private:
	Ice::Int _ownerId;
	HotFeedSeq _hotFeedList;
};

}
}

#endif
