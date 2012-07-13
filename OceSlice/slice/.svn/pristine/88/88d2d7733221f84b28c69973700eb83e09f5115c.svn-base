#ifndef FEED_CACHE_H_
#define FEED_CACHE_H_

#include <RFeed.ice>
module xce{
module feed{
	class FeedDBConfig{
		bool syncPut;
		bool syncRemove;
		bool syncRemoveAll;
	};
	dictionary<string, FeedDBConfig> FeedDBCfgsMap;
	class FeedCacheConfig{
		int cacheSize;
		int deletedCount;
		string loadFrom;
		bool	enableCache;
    bool enableLogicReject;
    bool enableCacheReject;
		FeedDBCfgsMap feedDBCfgs;
	};
	//class CacheCount{
	//	long usercount;
	//	long feedcount;
	//};
  enum ConfigItem {LogicReject, CacheReject, CacheSize};
  class FeedCache{
    FeedItemSeq     get(int userid);
		int		getCount(int userId);
    void            put(FeedItem feed, MyUtil::Int2IntMap usersWeight);
    void            remove(int userid, long feedid);
    void            removeAll(int userid);
    //CacheCount  queryCacheCount();
    void    updateConfig(FeedCacheConfig cfg);
    FeedCacheConfig getConfig();
  };


  class PoolUserId{
    int poolIndex;
    MyUtil::IntSeq ids;
  };

	class FeedCacheN{
		FeedItemSeq     getMore(int userId, long maxFeed);
		FeedItemSeq     get(int userId);
    PoolUserId getPoolUserIds(int poolIndex);
		int		getCount(int userId);
    void  modify(int userId, FeedItem item);
    void  put(FeedItem feed, MyUtil::Int2IntMap usersWeight);
    void  putByUserId(int userId, FeedItemSeq items);
   // PoolUserId getPoolUserIds(int poolIndex);
    void  remove(int userId, long feedId);
    void  removeAll(int userId);
    void    removeUser(int userId);
    MyUtil::LongSeq removeByMerge(int userId, long merge, int stype);
    MyUtil::LongSeq removeMergeByFeedId(int userId, long feedId);
    //void  removeUserFeeds(int userId); //测试用
    void  test(FeedItem feed, MyUtil::Int2IntMap usersWeight);
		//CacheCount 	queryCacheCount();
		void  updateConfig(FeedCacheConfig cfg);
		FeedCacheConfig	getConfig();
	};

//sequence<long> LongSeq;
dictionary<int, int> Int2IntMap;
};
};

#endif
