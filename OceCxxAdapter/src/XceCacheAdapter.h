/*
 * XceCacheAdapter.h
 *
 *  Created on: 2009-12-10
 *      Author: bbc
 */

#ifndef XCE_CACHE_ADAPTER_H_
#define XCE_CACHE_ADAPTER_H_

#include <boost/lexical_cast.hpp>

#include "TopicI.h"
#include "AdapterI.h"
#include "TaskManager.h"
#include <IceUtil/RWRecMutex.h>
#include "XceCache.h"

namespace xce {
typedef std::map<int, std::string> Int2StrMap;

namespace searchcache{
  class IDbCache;
}

class CommonCacheChannel : public ::MyUtil::Channel {
public:
	CommonCacheChannel();
	virtual ~CommonCacheChannel();
};

class XceCacheAdapter : public MyUtil::ReplicatedAdapterI, //public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<CommonCacheChannel, XceCacheAdapter> {
public:
	XceCacheAdapter();
  virtual ~XceCacheAdapter();

  //保留int类型, 因为server要用int值取模做hash
  //void DbInitialize(const vector<string> & dsn);

//	void Load(int id);  //由客户端调用proxy的load, 访问data source, 获得数据
//	void Loads(const IntSequence& ids);
	
	bool Put(int uid, const std::string & value); // 由客户端直接设定数据
  void Puts(const IntSequence & uids, const StringList & values);

	bool Get(int userid, std::string* value);
	Int2StrMap Gets(const IntSequence& ids);
	
	bool Remove(int userid);
	void Removes(const IntSequence& ids);
	
  bool State(int cluster_id, CacheState* cs);
  bool SetState(int cluster_id, const CacheState & cs);

	bool IsValid(int cluster_id);
	bool SetValid(int cluster_id, bool);

protected:
	bool isValid(const Ice::ObjectPrx& proxy);
	
  //void doLoads(const IntSequence& ids);

	vector<CachePrx> _cache_proxies;
//  vector< boost::shared_ptr<searchcache::IDbCache> > _dbcaches;

	virtual string name() {
		return "cache";
	}
	virtual string endpoints() {
		return "@SearchCache";
	}
	virtual size_t cluster() {
		return 4;
	}	

	/*class DbLoadTask: public MyUtil::Task {
  	public:
  		DbLoadTask(MyUtil::IntSeq& ids):Task(10) {_ids.swap(ids);}
  		virtual void handle()
  		{
	      XceCacheAdapter::instance().doLoads(_ids);
	    }
  	private:
  		MyUtil::IntSeq _ids;
	};
	
	class DbLoader: virtual public IceUtil::Thread {
	public:
  	void Load(int userId) {
  		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  		_id_cache.push_back(userId);
  		if (_id_cache.size() >= 200)
      {
  			_mutex.notify();
  		}
  	}
  	void Loads(const MyUtil::IntSeq& userIds) {
  		if (userIds.empty()) {
  			return;
  		}
  		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  		_id_cache.insert(_id_cache.end(), userIds.begin(), userIds.end());
  		if (_id_cache.size() >= 200) {
  			_mutex.notify();
  		}
  	}
	protected:
		virtual void run() {
      //vector<string> dbs;
      //dbs.push_back("tokyotryant://10.3.17.70:1981");
      //dbs.push_back("tokyotryant://10.3.17.71:1981");
      //dbs.push_back("tokyotryant://10.3.17.72:1981");
      //dbs.push_back("tokyotryant://10.3.17.73:1981");	 
      //XceCacheAdapter::instance().DbInitialize(dbs);

			while (true) {
				try {
					IntSequence batch;
					{
						IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
						if (_id_cache.size() < 200) //why 200?
						{
							_mutex.timedWait(IceUtil::Time::seconds(1));
							//continue;
						}
						batch.swap(_id_cache);
					}
					
					MyUtil::TaskManager::instance().execute(new DbLoadTask(batch));
					
			    //XceCacheAdapter::instance().doLoad(batch);
          //sleep(1);
				} catch (...) {
					MCE_WARN("InvokeReload main loop exception");
				}
			}
		}
	private:
		MyUtil::IntSeq _id_cache;
		IceUtil::Monitor<IceUtil::Mutex> _mutex;
	};

	DbLoader _db_loader;*/
};

}

#endif /* XCE_CACHE_ADAPTER_H_ */ 
