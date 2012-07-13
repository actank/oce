#ifndef FEED_CACHE_N_H_
#define FEED_CACHE_N_H_

#include <sys/time.h>
#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include "IceLogger.h"
#include "FeedCache.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "Tasks.h"

namespace xce{
  namespace feed{

    using namespace boost::multi_index;

    class StatFunc {
      string _name;
      timeval m_tpstart, m_tpend;
      public:
      StatFunc(string name) :
        _name(name) {
          gettimeofday(&m_tpstart, NULL);
        }
      ~StatFunc() {
        gettimeofday(&m_tpend, NULL);
        float timeuse=1000000*(m_tpend.tv_sec-m_tpstart.tv_sec)+m_tpend.tv_usec
          -m_tpstart.tv_usec;
        timeuse/=1000;
        MCE_INFO(_name << "  time:" << timeuse << " ms");
      }
      ;
    };


    class UserFeed{
      public:
        static const int FEED_COUNT = 250;
        UserFeed(int userId){
          _end = 0;
          _size = 0;
          _deletedCount = 0;
          _userId = userId;
          memset(_feeds, 0, sizeof(FeedItem)*FEED_COUNT);
        }
        bool 		put(const FeedItem& item);
        void    modify(const FeedItem& item);
        FeedItemSeq 	getMore(Ice::Long maxFeed = -1);
        void 		remove(Ice::Long feedId);
        void removeAll();
        MyUtil::LongSeq    removeByMerge(Ice::Long merge, int stype); 
        MyUtil::LongSeq    removeMergeByFeedId(Ice::Long feedId);
        void 		reject(const vector<Ice::Long> & rejectedIds);

        int userId() const{
          return _userId;
        }
        int size(){
          return _size;
        }
      public:
        int 		_userId;
        FeedItem 	_feeds[FEED_COUNT];
        int 		_end;
        int 		_size;
        int		_deletedCount;
    };

    typedef UserFeed* UserFeedPtr;

    typedef boost::multi_index_container<
      UserFeedPtr,
      indexed_by<
        sequenced<> ,
      hashed_unique<
        BOOST_MULTI_INDEX_CONST_MEM_FUN(UserFeed, int, userId)
        > 
        >
        > UserFeedContainer;
    typedef UserFeedContainer::nth_index<1>::type UserIdIndex;
    typedef UserFeedContainer::nth_index<0>::type SequenceIndex;

    class UserFeedPool{
      public:
        FeedItemSeq 	getMore(int userId, Ice::Long maxFeed = -1);
        void    modify(int userId, const FeedItem &  item);
        //void    get(int userId, const FeedItem & item);
        MyUtil::IntSeq getIds();
        void	 	put(const FeedItem& item, const MyUtil::Int2IntMap& usersWeight);
        void	 	putByUserId(int userId, const FeedItemSeq& items);
        void		remove(int userId, Ice::Long feedId);
        void		removeAll(int userId);
        void		removeUser(int userId);
        MyUtil::LongSeq     removeByMerge(int userId, Ice::Long merge, int stype); 
        MyUtil::LongSeq     removeMergeByFeedId(int userId, Ice::Long feedId);
        //void    removeUserFeeds(int userId); //测试用
        int		  getUserFeedSize(int userId);
        pair<int, int>	size();
				void reject(int uid, const vector<Ice::Long> & rejectedIds);
      private:
        FeedItemSeq 	getFromCache(int userId, Ice::Long maxFeed = -1);
        MyUtil::LongSeq removeByMergeFromCache(int userId, Ice::Long merge, int stype); 
        MyUtil::LongSeq removeMergeByFidFromCache(int userId, Ice::Long feedId);
        void loadUser(int userId);
      private:
        UserFeedContainer	_users;
        IceUtil::Mutex		_mutex;
    };

    class FeedCacheI : public FeedCacheN,
    public MyUtil::Singleton<FeedCacheI>{
      public:
        static const int CACHE_SIZE = 3000000;
        //static const int CACHE_SIZE =   1500000;
        FeedCacheI(){
          _cfg = new FeedCacheConfig;
          _cfg->cacheSize = CACHE_SIZE;
          _cfg->deletedCount = 20; 
          _cfg->loadFrom = "FeedCacheLogic";
          _cfg->enableLogicReject = false;
          _cfg->enableCacheReject = true;
          _cfg->enableCache = true;
          _cfg->feedDBCfgs["FeedDB"] = new FeedDBConfig;
          _cfg->feedDBCfgs["FeedDB"]->syncPut = false;
          _cfg->feedDBCfgs["FeedDB"]->syncRemove = true; 
          _cfg->feedDBCfgs["FeedDB"]->syncRemoveAll = true;
          _cfg->feedDBCfgs["SSDFeedDB"] = new FeedDBConfig;
          _cfg->feedDBCfgs["SSDFeedDB"]->syncPut = false;
          _cfg->feedDBCfgs["SSDFeedDB"]->syncRemove = true; 
          _cfg->feedDBCfgs["SSDFeedDB"]->syncRemoveAll = true; 

        }
        static const int POOL_SIZE = 10000;
        FeedItemSeq 	get(int userId, const Ice::Current& = Ice::Current());
        FeedItemSeq 	getMore(int userId, Ice::Long maxFeed, const Ice::Current& = Ice::Current());
        PoolUserIdPtr getPoolUserIds(int poolIndex, const Ice::Current& = Ice::Current());
        //FeedItemSeq   getSection(int userId, int begin, int limit, const Ice::Current& = Ice::Current());
        void    modify(int userId, const FeedItem &  item, const Ice::Current& = Ice::Current());
        int		  getCount(int userId, const Ice::Current& = Ice::Current());
        void	 	put(const FeedItem& item, const MyUtil::Int2IntMap& usersWeight, const Ice::Current& = Ice::Current());
        void	 	putByUserId(int userId, const FeedItemSeq& items, const Ice::Current& = Ice::Current());
        void		remove(int userId, Ice::Long feedId, const Ice::Current& = Ice::Current());
        void		removeAll(int userId, const Ice::Current& = Ice::Current()); 
        void		removeUser(int userId, const Ice::Current& = Ice::Current()); 
        MyUtil::LongSeq     removeByMerge(int userId, Ice::Long merge, int stype, const Ice::Current& = Ice::Current()); 
        MyUtil::LongSeq     removeMergeByFeedId(int userId, Ice::Long feedId, const Ice::Current& = Ice::Current()); 
        //void    removeUserFeeds(int userId, const Ice::Current& = Ice::Current()); //测试用
        void		updateConfig(const FeedCacheConfigPtr& cfg, const Ice::Current& = Ice::Current()); 
        FeedCacheConfigPtr    getConfig(const Ice::Current& = Ice::Current()); 
        bool setConfigItem(ConfigItem ci, int itemValue, const Ice::Current& = Ice::Current());

        void	  test(const FeedItem& item, const MyUtil::Int2IntMap& usersWeight, const Ice::Current& = Ice::Current());
        void	  testLogic(const FeedItem& item, const MyUtil::Int2IntMap& usersWeight);

        pair<Ice::Long, Ice::Long>	queryCacheCount();

				void reject(int userId, const vector<Ice::Long> & rejectedIds, const Ice::Current& = Ice::Current());
				void printThreadPoolInfoByLevel(int level = MyUtil::TASK_LEVEL_NORMAL);
      private:
        UserFeedPool		_pool[POOL_SIZE];
        FeedCacheConfigPtr 	_cfg;
        IceUtil::Mutex		_config_mutex;
    };

  }
}
#endif
