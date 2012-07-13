#ifndef FEED_CACHE_LOGIC_ADAPTER_H_
#define FEED_CACHE_LOGIC_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedCacheLogic.h"

namespace xce{
namespace feed{



using namespace MyUtil;

class FeedCacheLogicAdapter : public AdapterI,
		public AdapterISingleton<MyUtil::XceFeedChannel, FeedCacheLogicAdapter>{
public:
	FeedCacheLogicAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
  MyUtil::LongSeq resortFeedItems(int userId, const ::xce::feed::FeedItemSeq & feedItemSeq);
//  MyUtil::LongSeq resortFeedItems2(int userId, const ::xce::feed::FeedItemSeq & feedItemSeq); // 测试用

  FeedItemSeq   get(int userId, int begin, int limit);
  void    put(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncFeedDB = true);
  void    remove(int userId, Ice::Long feedId, bool syncFeedDB = true);
  void    removeByMerge(int userId, Ice::Long mergeId, int stype, bool syncFeedDB = true);
  void    removeMergeByFeedId(int userId, Ice::Long feedId, bool syncFeedDB = true);
  void    removeAll(int userId, bool syncFeedDB = true);


  void    test(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncDB = true);

protected:
	virtual string name() {
		return "FeedCacheLogic";
	}
	virtual string endpoints() {
		return "@FeedCacheLogic";
	}
	virtual size_t cluster() {
		return 10;
	}

	FeedCacheLogicPrx getManager(int id, int timeout=260);
	FeedCacheLogicPrx getManagerOneway(int id);

	vector<FeedCacheLogicPrx> _managersOneway;
	vector<FeedCacheLogicPrx> _managers;

};

};
};


#endif /*RESTMEDIATORADAPTER_H_*/
