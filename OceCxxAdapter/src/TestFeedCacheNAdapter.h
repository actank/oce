#ifndef TEST_FEED_CACHE_N_ADAPTER_H_
#define TEST_FEED_CACHE_N_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedCache.h"

namespace xce{
namespace feed{

  using namespace MyUtil;

class TestFeedCacheNAdapter : public AdapterI,
     public AdapterISingleton<MyUtil::XceFeedChannel, TestFeedCacheNAdapter>{

public:
    TestFeedCacheNAdapter() {
      _managers.resize(cluster() > 0 ? cluster() : 1);
      _managersOneway.resize(cluster() > 0 ? cluster() : 1);
    }

      void		put(const FeedItem& feed, const Int2IntMap& userWeight);
      FeedItemSeq   get(int userId);
      FeedItemSeq   getMore(int userId, Ice::Long maxFeed);
      void modify(int userId, const FeedItem & item);
      void		remove(int userId, Ice::Long feedId);
      void		removeAll(int userId); 
      MyUtil::LongSeq removeByMerge(int userId, Ice::Long merge, int stype);
      MyUtil::LongSeq removeMergeByFeedId(int userId, Ice::Long feedId);
      //void    removeUserFeeds(int userId); //测试用

      void		test(const FeedItem& feed, const Int2IntMap& userWeight);
    protected:
      virtual string name() {
        return "FeedCacheN58";
      }
      virtual string endpoints() {
        return "@FeedCacheN58";
      }
      virtual size_t cluster() {
        return 1;
      }

      FeedCacheNPrx getManager(int id, int timeout=260);
      FeedCacheNPrx getManagerOneway(int id);

      vector<FeedCacheNPrx> _managersOneway;
      vector<FeedCacheNPrx> _managers;

  };

};
};


#endif /*RESTMEDIATORADAPTER_H_*/
