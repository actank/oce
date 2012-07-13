#ifndef _SOCIALFEEDCACHEI_H_
#define _SOCIALFEEDCACHEI_H_

#include <SocialFeedCache.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"
#include <ObjectCacheI.h>
#include <QueryRunner.h>
#include "SocialFeedHelper.h"

namespace xce {
namespace socialgraph {

const static int FEED_DATA = 1;

//***************************************************************************************

class FeedData : virtual public Ice::Object, virtual IceUtil::Mutex {
public:
	FeedData(::Ice::Int ownerId, const FeedSeq& feedList) : 
		_ownerId(ownerId), _feedList(feedList) { };
	FeedSeq get(int limit = -1);
	void remove(const Feed& feed);

private:
	::Ice::Int _ownerId;
	FeedSeq _feedList;	
};
typedef IceUtil::Handle<FeedData> FeedDataPtr;

//***************************************************************************************

class SocialFeedCacheI : virtual public SocialFeedCache,
		virtual public MyUtil::Singleton<SocialFeedCacheI> {
public:
	virtual void set(::Ice::Int, const FeedSeq&, const Ice::Current&);
	virtual void remove(::Ice::Int, const MyUtil::IntSeq&, const MyUtil::IntSeq&, const Ice::Current&);
	virtual FeedSeq get(::Ice::Int, ::Ice::Int, const Ice::Current&);

private:
	FeedDataPtr locateFeedData(::Ice::Int id);
	friend class MyUtil::Singleton<SocialFeedCacheI>;
};

//***************************************************************************************

class SocialFeedFactory : virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

//***************************************************************************************

class RemoveTask : virtual public MyUtil::Task {
public:
	RemoveTask(Ice::Int ownerId, const Feed& feed):
		_ownerId(ownerId), _feed(feed) { }; 
	virtual void handle();

private:
	Ice::Int _ownerId;
	Feed _feed;
};

}
}

#endif
