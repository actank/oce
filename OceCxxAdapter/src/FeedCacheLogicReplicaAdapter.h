#ifndef __FEED_CACHE_LOGIC_REPLICA_ADAPTER_H__
#define  __FEED_CACHE_LOGIC_REPLICA_ADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"
#include "FeedCacheLogic.h"

namespace xce {
namespace feed {

using namespace MyUtil;


class FeedCacheLogicReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<FeedCacheLogicPrx>,
  public MyUtil::Singleton<FeedCacheLogicReplicaAdapter>{

public:

  FeedCacheLogicReplicaAdapter() : MyUtil::ReplicatedClusterAdapterI<FeedCacheLogicPrx> ("ControllerFeedCacheLogicR",120,300,new XceFeedControllerChannel){
  }

  MyUtil::LongSeq resortFeedItems(int userId, const ::xce::feed::FeedItemSeq & feedItemSeq);
  //MyUtil::LongSeq resortFeedItems2(int userId, const ::xce::feed::FeedItemSeq & feedItemSeq); // 测试用

  FeedItemSeq   get(int userId, int begin, int limit);
  void    put(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncFeedDB = true);
  void    remove(int userId, Ice::Long feedId, bool syncFeedDB = true);
  void    removeByMerge(int userId, Ice::Long mergeId, int stype, bool syncFeedDB = true); 
  void    removeMergeByFeedId(int userId, Ice::Long feedId, bool syncFeedDB = true); 
  void    removeAll(int userId, bool syncFeedDB = true); 

  void    test(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncDB = true);


protected:


	FeedCacheLogicPrx getManager(long id);
	FeedCacheLogicPrx getManagerOneway(int id);


};

}
;
}
;
#endif /*__FEED_CACHE_LOGIC_REPLICA_ADAPTER_H__ */
