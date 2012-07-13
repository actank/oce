#ifndef FEED_CACHE_H_
#define FEED_CACHE_H_

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

/*class FeedCacheConfig : public MyUtil::Singleton<FeedCacheConfig>{
public:
	static const int CACHE_SIZE = 3000000;
	FeedCacheConfig(){
		_deletedCount = 10;
		_cacheSize = CACHE_SIZE;
	}
	int deletedCount(){
		return _deletedCount;
	}
	void deletedCount(int c){
		_deletedCount = c;
	}
	int cacheSize(){
		return _cacheSize;
	}
	void cacheSize(int c){
		_cacheSize = c;
	}
private:
	int 	_deletedCount;
	int 	_cacheSize;
	bool	_load_when_put;
	map<string, FeedDBConfig> feedDBCfgs;
};*/

class UserFeed{
public:
	static const int FEED_COUNT=260;
	UserFeed(int userid){
		_end = 0;
		_size = 0;
		_deletedCount = 0;
		_userid = userid;
		memset(_feeds, 0, sizeof(FeedItem)*FEED_COUNT);
	}
	void 		put(const FeedItem& feed);
	FeedItemSeq 	get();
	void 		remove(Ice::Long feedid);
	int userid() const{
		return _userid;
	}
	int size(){
		return _size;
	}
public:
	int 		_userid;
	FeedItem 	_feeds[FEED_COUNT];
	int 		_end;
	int 		_size;
	int		_deletedCount;
	IceUtil::Mutex    _mutex;
};

typedef UserFeed* UserFeedPtr;

typedef boost::multi_index_container<
	UserFeedPtr,
	indexed_by<
		sequenced<> ,
		hashed_unique<
			BOOST_MULTI_INDEX_CONST_MEM_FUN(UserFeed, int, userid)
		> 
	>
> UserFeedContainer;
typedef UserFeedContainer::nth_index<1>::type UserIdIndex;
typedef UserFeedContainer::nth_index<0>::type SequenceIndex;

class UserFeedPool{
public:
  static const int FEED_COUNT=260;
	//static const unsigned int MAX_USER_COUNT = 10;
	FeedItemSeq 	get(int userid);
	void	 	put(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight);
	void		remove(int userid, Ice::Long feedid);
	void		removeAll(int userid);
	int		getUserFeedSize(int userid);
	pair<int, int>	size();
private:
	UserFeedContainer	_users;
	IceUtil::Mutex		_mutex;
};

class FeedCacheI : public FeedCache,
		public MyUtil::Singleton<FeedCacheI>{
public:
	static const int CACHE_SIZE = 3000000;
	FeedCacheI(){
		_cfg = new FeedCacheConfig;
		_cfg->cacheSize = CACHE_SIZE;
		_cfg->deletedCount = 10;
		_cfg->loadFrom = "FeedDB";
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
	FeedItemSeq 	get(int userid, const Ice::Current& = Ice::Current());
	int		getCount(int userid, const Ice::Current& = Ice::Current());
	void	 	put(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, const Ice::Current& = Ice::Current());
	void		remove(int userid, Ice::Long feedid, const Ice::Current& = Ice::Current());
	void		removeAll(int userid, const Ice::Current& = Ice::Current()); 
	void		updateConfig(const FeedCacheConfigPtr& cfg, const Ice::Current& = Ice::Current()); 
	FeedCacheConfigPtr getConfig(const Ice::Current& = Ice::Current()); 
	
	pair<Ice::Long, Ice::Long>	queryCacheCount();
private:
	UserFeedPool		_pool[POOL_SIZE];
	FeedCacheConfigPtr 	_cfg;
};

}
}
#endif
