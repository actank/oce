#ifndef FEED_CACHE_ADAPTER_H_
#define FEED_CACHE_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedCache.h"

namespace xce{
namespace feed{

  using namespace MyUtil;

  class FeedCacheAdapter: public MyUtil::ReplicatedClusterAdapterI<FeedCachePrx>,
  public MyUtil::Singleton<FeedCacheAdapter> {
    public:
      FeedCacheAdapter(): MyUtil::ReplicatedClusterAdapterI <FeedCachePrx> ("ControllerFeedCacheR", 120, 260, new XceFeedChannel())
    {
      //_managers.resize(cluster() > 0 ? cluster() : 1);
      //_managersOneway.resize(cluster() > 0 ? cluster() : 1);
    }
      void		put(const FeedItem& feed, const Int2IntMap& userWeight);
      FeedItemSeq   get(int userId);
      void		remove(int userId, Ice::Long feedId);
      void		removeAll(int userId); 
    protected:
      virtual string name() {
        return "FeedCache";
      }
      virtual string endpoints() {
        return "@FeedCache";
      }
      virtual size_t cluster() {
        return 10;
      }

      FeedCachePrx getManager(int id, int timeout=260);
      FeedCachePrx getManagerOneway(int id);

      //vector<FeedCachePrx> _managersOneway;
      //vector<FeedCachePrx> _managers;

  };

};
};


#endif /*RESTMEDIATORADAPTER_H_*/
