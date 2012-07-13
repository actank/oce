#ifndef __FEED_CACHE_N_REPLICA_ADAPTER_H__
#define __FEED_CACHE_N_REPLICA_ADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedCache.h"

namespace xce{
namespace feed{

  using namespace MyUtil;

  class FeedCacheNReplicaAdapter: public MyUtil::ReplicatedClusterAdapterI<FeedCacheNPrx>,
  public MyUtil::Singleton<FeedCacheNReplicaAdapter> {
    public:
      FeedCacheNReplicaAdapter(): MyUtil::ReplicatedClusterAdapterI <FeedCacheNPrx> ("ControllerFeedCacheNR", 120, 300, new XceFeedControllerChannel()) {}

      void		put(const FeedItem& feed, const Int2IntMap& userWeight);
      FeedItemSeq   get(int userId);
      FeedItemSeq   getMore(int userId, Ice::Long maxFeed);
      void modify(int userId, const FeedItem & item);
      void		remove(int userId, Ice::Long feedId);
      void		removeAll(int userId); 
      MyUtil::LongSeq removeByMerge(int userId, Ice::Long merge, int stype);
      MyUtil::LongSeq removeMergeByFeedId(int userId, Ice::Long feedId);
      void		test(const FeedItem& feed, const Int2IntMap& userWeight);
    protected:

      FeedCacheNPrx getManager(int id, int timeout=260);
      FeedCacheNPrx getManagerOneway(int id);



  };

};
};


#endif
