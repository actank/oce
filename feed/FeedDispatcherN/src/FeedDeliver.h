#ifndef __FEED_DELIVER_H__
#define __FEED_DELIVER_H__

#include "RFeed.h"
#include "Singleton.h"
#include "FeedDispatcher.h"
#include "XceLoggerAdapter.h"
namespace xce {
namespace feed {

using namespace MyUtil;
using namespace com::xiaonei::service;
//--------------------------------------------------------------------------------
class FeedDeliver : public Singleton<FeedDeliver> {
	friend class Singleton<FeedDeliver>;
public:
	////FeedDeliver():_logger(""){}
	bool initialize(const Ice::PropertiesPtr& props);
	void deliver(FeedSeedPtr & feed,FeedConfigPtr & config,set<int> & target,map<int,int> & rank,DispatchCost & cost) ;
	 void initLogger();
private:
	FeedDeliver();
	FeedDeliver(const FeedDeliver& rhs);
	FeedDeliver& operator=(const FeedDeliver& rhs);
	virtual ~FeedDeliver();
	void deliverToNews(FeedSeedPtr feed, FeedConfigPtr config,set<int> &target,map<int, int> &rank);
	void deliverToMini(FeedSeedPtr feed,FeedConfigPtr config);
	void deliverToPhotos(FeedSeedPtr feed, FeedConfigPtr config,set<int> & target, map<int, int> & rank);
	 XceLoggerPtr _logger;
};

}
;
}
;

#endif
