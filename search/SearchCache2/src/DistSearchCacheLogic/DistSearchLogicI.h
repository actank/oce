/*
 * =====================================================================================
 *
 *       Filename:  DistSearchLogicI.h
 *
 *    Description:  Logic for updating DistSearchCache
 *
 *        Version:  1.0
 *        Created:  2010年12月04日 17时16分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __DISTSEARCHLOGICI_H__
#define __DISTSEARCHLOGICI_H__
#include <vector>
#include <queue>
#include <boost/shared_ptr.hpp>
#include "SearchCache.h"
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
#include "DistSearchCacheAdapter.h"
#include "search/SearchCache2/src//Util/IDbCache.h"
#include "search/SearchCache2/src//Util/TokyoTyrantCache.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "ZoieUpdater.h"

namespace xce {
  namespace searchcache {
    using namespace com::xiaonei::xce;
    using namespace xce::serverstate;
    using namespace MyUtil;
    using namespace std;
    using namespace xce::dist::searchcache;
    using namespace xce::searchcache::dboperation;    
    const unsigned int kMaxUserIdNumEachProcess = 200;
    const Ice::Int SEARCH_CACHE = 0;
    const std::string SEARCH_CACHE_LOGIC_MANAGER = "S";

    const int TASK_LEVEL_FILL = 1;
    const int TASK_LEVEL_FILL_BASIC = 2;
    const int TASK_LEVEL_FILL_CONFIG = 3;
    const int TASK_LEVEL_FILL_USERS = 8;
    const int TASK_LEVEL_UPDATE_DBCACHE = 9;
    const int FILL_BATCH_SIZE = 3000;

    //***************************************************************************
    class MessageQueueThread;
    class SearchLogicManagerI: virtual public SearchCacheManager,
    public MyUtil::Singleton<SearchLogicManagerI> {
      public:
        SearchLogicManagerI();
        virtual void load(const ::MyUtil::IntSeq& userIds, const Ice::Current&);
        virtual void update(const UpdateUnitSeq& updateUnits, const Ice::Current&);
        virtual bool isValid(const Ice::Current& = Ice::Current());
        virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
        virtual bool removes(const MyUtil::LongSeq& userIds, const Ice::Current&);
        //哑接口，仅对MemCache有效
        virtual bool puts(const Int2ByteSeq& id2Map, const Ice::Current&);
        virtual SearchCacheResult getSearchCache(::Ice::Int, const Ice::Current&);
        virtual Int2SearchCacheResultMap getSearchCacheList(const ::MyUtil::IntSeq&, const Ice::Current&);
        virtual Int2ByteSeq getSearchCacheList2(const ::MyUtil::IntSeq&, const ::Ice::Current&);
      
        inline boost::shared_ptr<IDbCache> getDbHandler(long userId) {
         map<long, boost::shared_ptr<IDbCache> >::iterator it = _db_cache_map.find(userId % _tt_index_cluster); 
         MCE_DEBUG("userId = "<<userId<<" _tt_index_cluster = "<<_tt_index_cluster<<" mod = "<<userId % _tt_index_cluster); 
         if (it == _db_cache_map.end())
           return boost::shared_ptr<IDbCache>((IDbCache*)NULL);
         return (*it).second;
       }        

        void bootstrap(int mod, int tt_index_cluster, vector<string>& dsn_name);

      private:
        typedef vector<MessageQueueThread*> MessageQueueThreadVec;
        MessageQueueThreadVec _vecMessageQueueThread;
        bool _valid;
        IceUtil::RWRecMutex _validMutex;
        int _cluster;
        int _mod;
        string _dsn;
        int _tt_index_cluster;
        map<long, boost::shared_ptr<IDbCache> > _db_cache_map;
    };

    //***************************************************************************

    class ServiceSchemaI: public MyUtil::Descriptor {
      public:
        virtual void configure(const Ice::PropertiesPtr& props);
    };

    //***************************************************************************
    class MessageQueueThread : virtual public IceUtil::Thread {

      public:
        MessageQueueThread(int mod) :_mod(mod) {
        }

        void invoke(const UpdateUnit& unit) {
          {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
            boost::shared_ptr<UpdateUnit> pUnit(new UpdateUnit(unit));
            _cache.push(pUnit);
            if (_cache.size() > kMaxUserIdNumEachProcess) {
              _mutex.notify();
            }
          }
        }

        void run() {
          UpdateTask update_task;
          std::queue<boost::shared_ptr<UpdateUnit> > batch;
          while(true) {
            {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
              if (_cache.size() < kMaxUserIdNumEachProcess) {
                _mutex.timedWait(IceUtil::Time::seconds(1));
              }
              while(batch.size() < kMaxUserIdNumEachProcess && !_cache.empty()) {
                batch.push(_cache.front());
                _cache.pop();
              }
            }
            if (!batch.empty())  {
              update_task.handle(batch);
            }
          }
        }
      private:
        IceUtil::Monitor<IceUtil::Mutex> _mutex;
        std::queue<boost::shared_ptr<UpdateUnit> > _cache;
        int _mod;
    };
  }
}
#endif

