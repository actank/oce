#ifndef FEED_CACHE_LOGIC_H_
#define FEED_CACHE_LOGIC_H_

#include <RFeed.ice>
#include <FeedCache.ice>
module xce{
module feed{

  class FeedCacheLogic {
    MyUtil::LongSeq  resortFeedItems(int uid, FeedItemSeq seq);
    //MyUtil::LongSeq  resortFeedItems2(int uid, FeedItemSeq seq); // 测试用

    FeedItemSeq get(int userId, int begin, int limit);
    //int   getCount(int userId);
    void  put(FeedItem item, MyUtil::Int2IntMap usersWeight, bool syncFeedDB);
    void  remove(int userId, long feedid, bool syncFeedDB);
    void  removeAll(int userId, bool syncFeedDB);
    void  removeByMerge(int userId, long merge, int stype, bool syncFeedDB);
    void  removeMergeByFeedId(int userId, long feedId, bool syncFeedDB);
    void  test(FeedItem item, MyUtil::Int2IntMap usersWeight, bool syncFeedDB);

    void  updateConfig(FeedCacheConfig cfg);
    FeedCacheConfig  getConfig();

    //void setSeq(MyUtil::LongSeq seq);

  };

};
};

#endif
