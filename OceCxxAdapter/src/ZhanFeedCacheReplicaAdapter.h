#ifndef ZHAN_FEED_CACHE_ADAPTER_H_
#define ZHAN_FEED_CACHE_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedCache.h"

namespace xce{
namespace feed{

  using namespace MyUtil;

  class ZhanFeedCacheAdapter: public MyUtil::ReplicatedClusterAdapterI<FeedCachePrx>,
  public MyUtil::Singleton<ZhanFeedCacheAdapter> {
    public:
      ZhanFeedCacheAdapter(): MyUtil::ReplicatedClusterAdapterI <FeedCachePrx> ("ControllerZhanFeedCacheR", 120, 260, new ZhanFeedChannel())
    {
    }
      void		put(const FeedItem& feed, const Int2IntMap& userWeight);
      FeedItemSeq   get(int userId);
      void		remove(int userId, Ice::Long feedId);
      void		removeAll(int userId); 
    protected:
      virtual size_t cluster() {
        return 10; 
      }
      FeedCachePrx getManager(int id, int timeout=260);
      FeedCachePrx getManagerOneway(int id);
  };

};
};


#endif /*RESTMEDIATORADAPTER_H_*/
