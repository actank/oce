#ifndef __SEARCHCACHEI_H__ 
#define __SEARCHCACHEI_H__ 
#include <SearchCache.h> 
#include "../Util/SearchMemCacheData.h" 
#include "../Util/BinString.h"
#include <ServiceI.h> 
#include "util/cpp/TimeCost.h"
#include <IceUtil/IceUtil.h> 
#include <QueryRunner.h> 
using namespace MyUtil; 
using namespace IceUtil; 
using namespace com::xiaonei::xce; 
using namespace xce::searchcache; 
namespace xce { 
  namespace searchcache {

const Ice::Int SEARCH_CACHE = 0;
const Ice::Int BUCKET = 10;

const std::string SEARCH_CACHE_MANAGER = "M";

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_UPDATE_DBCACHE = 9;

const string USER_STATUS_FIELD = " id, status ";
const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";
const int IDS_BATCH_SIZE = 2000;
const int kInvokeLoadThreadNum = 4;
const int NULL_NUM = 5; 
const int LIMIT = 50000000;
//***************************************************************************

class SearchCacheFromDBFactoryI: public MyUtil::ServantFactory {
public:
	SearchCacheFromDBFactoryI(int cached);
	virtual Ice::ObjectPtr create(Ice::Long id);
	virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids);
private:
	int _cached;
};

//***************************************************************************

class ObjectCacheHelper {
    private:
        int bucket;
        int _mod;
        int cluster;
        static ObjectCacheHelper* __instance;
        static IceUtil::Mutex __mutex;

        ObjectCacheHelper() : bucket(10), _mod(4){}//4
        ObjectCacheHelper(const ObjectCacheHelper& rhs);
        ObjectCacheHelper& operator=(const ObjectCacheHelper& rhs);

    public:
        static ObjectCacheHelper& instance() {
            if (!__instance) {
                IceUtil::Mutex::Lock lock(__mutex);
                if (!__instance) {
                    __instance = new ObjectCacheHelper();
                }
            }
            return *__instance;
        }

        void registerObjectCache(int cached) {
            for (int i=0; i<bucket; i++) {
                ostringstream key;
                key << "SC" << i;
                ServiceI::instance().registerObjectCache(i, key.str(), new SearchCacheFromDBFactoryI(cached),false);
            }
        }

        void bootstrap(int bt, int mod) {
            bucket = bt;
            cluster = mod;
            _mod = mod*bucket;
        }

        template<typename T>
        inline T findObject(int uid) {
            return ServiceI::instance().findObject<T>((uid%_mod)/cluster,uid);
        }

        inline void addObject(int id, const Ice::ObjectPtr& o) {
            ServiceI::instance().getObjectCache()->addObject((id%_mod)/cluster, id, o);
        }

        

        inline void removeObject(int id) {
			ServiceI::instance().getObjectCache()->removeObject(((id)%_mod)/cluster, id);
        }

        inline void removeObject(MyUtil::LongSeq uids) {
            for (MyUtil::LongSeq::iterator it = uids.begin(); it!=uids.end(); ++it) {
                ServiceI::instance().getObjectCache()->removeObject(((*it)%_mod)/cluster, *it);
            }
        }

        inline void locateObject(MyUtil::LongSeq uids) {
            for (MyUtil::LongSeq::iterator it = uids.begin(); it!=uids.end(); ++it) {
                ServiceI::instance().getObjectCache()->locateObject(((*it)%_mod)/cluster, *it);
            }
        }
};


class BasicStatusResultHandlerI: public com::xiaonei::xce::ResultHandler {
  public:
    BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<int>& thread);
    virtual bool handle(mysqlpp::Row& row) const;
  private:
    Ice::Int& beginId_;
    std::vector<int>& thread_;
};
class LoadCache: virtual public IceUtil::Thread {
  public:
    LoadCache(boost::shared_ptr<IDbCache> db_cache):_db_cache(db_cache),_shouldStop(0){
    }
    void invoke(const MyUtil::IntSeq& userIds) {
      MCE_INFO("invoke... userIds.size is "<< userIds.size());
      if (userIds.empty()) {
        return;
      }
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
      _cache.insert(_cache.end(), userIds.begin(), userIds.end());

      if ((int)_cache.size() >= IDS_BATCH_SIZE) {
        _mutex.notify();
      }
    }
    void insert(const MyUtil::IntSeq& userIds){
      if(userIds.empty())
        return;
      int total=0;
      while(total < userIds.size()){
        vector<int>::const_iterator itA = userIds.begin()+total;
        vector<int>::const_iterator itB = total+IDS_BATCH_SIZE > userIds.size() ? userIds.end():itA+IDS_BATCH_SIZE;
        total += (itB - itA);
        vector<int> ids(itA,itB);
        std::map<int, ByteSeq> data_map;

        _db_cache->Query(ids, data_map);
        for(std::map<int, ByteSeq>::iterator iter = data_map.begin(); iter!= data_map.end(); iter++)  {
           BinStringPtr binObj = new BinString((char *)((iter->second).data()),(iter->second).size());
           MCE_DEBUG("loading userid=" << iter->first << "\t" << binObj->getBufferLen() << "\t" << binObj->getBufferCapacity());
           ObjectCacheHelper::instance().addObject(iter->first, binObj); 
        }

        MCE_INFO("ids size is "<<ids.size()<<" DATA_MAP SIZE IS "<<data_map.size());
        //insert into objectacache
      }
    }
    void stop(){
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
      _shouldStop = 1;
    }

  protected:
    virtual void run() {
      while (true) {
        try {
          //MCE_INFO("THREAD RUN...");
          MyUtil::IntSeq batch;
          {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
            if(_cache.empty()){
              if(_shouldStop == 1){
                //MCE_INFO("STOP AND EMPTY");
                break;
              }
            }
            if (_cache.size() < IDS_BATCH_SIZE) {
              _mutex.timedWait(IceUtil::Time::seconds(1));
            }
            batch.swap(_cache);
          }
          insert(batch);
        } catch (...) {
          MCE_WARN("InvokeReload main loop exception");
        }
      }
    }

  private:
    MyUtil::IntSeq _cache;
    boost::shared_ptr<IDbCache> _db_cache;
    int _shouldStop;
    IceUtil::Monitor<IceUtil::Mutex> _mutex;
};
class LoadUpdateDataCacheTask: virtual public MyUtil::Task {
private:
    MyUtil::LongSeq uids_;
public:
	LoadUpdateDataCacheTask(MyUtil::LongSeq uids);
	virtual void handle();
};

class SearchCacheManagerI: virtual public SearchCacheManager,
		public MyUtil::Singleton<SearchCacheManagerI> {
public:
	SearchCacheManagerI();
	virtual SearchCacheResult getSearchCache(::Ice::Int, const Ice::Current&);

	virtual Int2SearchCacheResultMap getSearchCacheList(const ::MyUtil::IntSeq&,
			const Ice::Current&);
    virtual Int2ByteSeq getSearchCacheList2(const ::MyUtil::IntSeq&, 
            const ::Ice::Current&);

	virtual void load(const ::MyUtil::IntSeq&, const Ice::Current&);
    void update(const UpdateUnitSeq& updateUnits, const Ice::Current&);
    bool puts(const Int2ByteSeq& id2Map, const Ice::Current&);
    bool removes(const MyUtil::LongSeq& userIds, const Ice::Current&);
	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

	void bootstrap(int cluster, int mod, int cached, string& dsn);
	bool inline belongsMe(int id){
		if (_cluster==0) return true;
		return (_mod == (id % _cluster));
	}
    string getDSN() { return _dsn;}

private:
	class InvokeLoad: virtual public IceUtil::Thread {
		public:
			void invoke(const MyUtil::LongSeq& userIds) {
				if (userIds.empty()) {
					return;
				}
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				_cache.insert(_cache.end(), userIds.begin(), userIds.end());

				if (_cache.size() >= 200) {
					_mutex.notify();
				}
			}
		protected:
			virtual void run() {
				while (true) {
					try {
						MyUtil::LongSeq batch;
						{
							IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
							if (_cache.size() < 200) {
								_mutex.timedWait(IceUtil::Time::seconds(1));
							}
							batch.swap(_cache);
						}

                        ::MyUtil::TaskManager::instance().execute(new LoadUpdateDataCacheTask(batch));
					} catch (...) {
						MCE_WARN("InvokeReload main loop exception");
					}
				}
			}
		private:
			MyUtil::LongSeq _cache;
			IceUtil::Monitor<IceUtil::Mutex> _mutex;
		};

	InvokeLoad _invokeLoad;

	SearchMemCacheDataPtr getSearchCacheData(int userId);
	//MyUtil::ObjectResultPtr getSearchCacheData(const MyUtil::IntSeq& userIds);

	bool _valid;
	IceUtil::RWRecMutex _validMutex;

	int _cluster;
	int _mod;
	int _cached;
    string _dsn;
};

class FillFromDbCacheTask: virtual public MyUtil::Task {
public:
	FillFromDbCacheTask();
	virtual void handle();
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

//***************************************************************************

}
}
#endif
