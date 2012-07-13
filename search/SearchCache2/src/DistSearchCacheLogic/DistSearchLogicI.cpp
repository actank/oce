/*
 * =====================================================================================
 *
 *       Filename:  DistSearchLogicI.cpp
 *
 *    Description:  Logic for updating DistSearchCache
 *
 *        Version:  1.0
 *        Created:  2010年12月04日 17时15分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "DistSearchLogicI.h"
#include "execinfo.h"
#include "XceCache.h"

using namespace xce::serverstate;
namespace MyUtil{
  void initialize() {
    ServiceI& service = ServiceI::instance();

    int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("DistSearchCacheLogic.Mod", 0);
    int tt_index_cluster = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("DistSearchCacheLogic.IndexClusterForTT", 0);
    service.getAdapter()->add(&xce::searchcache::SearchLogicManagerI::instance(),service.createIdentity(xce::searchcache::SEARCH_CACHE_LOGIC_MANAGER, ""));

    ServerStateSubscriber::instance().initialize("ControllerDistSearchLogic", &xce::searchcache::SearchLogicManagerI::instance(), mod);

    xce::searchcache::SearchLogicManagerI::instance().setValid(true);

    TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
    TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL_BASIC, ThreadPoolConfig(1, 8));
    TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL_CONFIG, ThreadPoolConfig(1, 8));
    TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL_USERS, ThreadPoolConfig(1, 8));
    TaskManager::instance().config(xce::searchcache::TASK_LEVEL_UPDATE_DBCACHE, ThreadPoolConfig(1, 8));
    vector<string> dsn_name;
    for (int i=0; i<tt_index_cluster; ++i) {
      ostringstream dsn;
      dsn << "DistSearchCacheLogic.InitDsn" << i;
      string strDSN = service.getCommunicator()->getProperties()->getProperty(dsn.str());
      dsn_name.push_back(strDSN);
    }
    xce::searchcache::SearchLogicManagerI::instance().bootstrap(mod, tt_index_cluster, dsn_name);
    service.registerXceObserver(new xce::searchcache::ServiceSchemaI);
  }
}
//***************************************************************************

//全局函数将更新时需要查询mysql的多个语句模块化
namespace xce{
  namespace searchcache{
    using namespace MyUtil;
    using namespace IceUtil;
    using namespace com::xiaonei::xce;
    using std::queue;
    using namespace std;
    using namespace xce::dist::searchcache;
    using namespace xce::searchcache::dboperation;
    const int kMessageQueueThreadNum = 4;

    SearchCacheResult SearchLogicManagerI::getSearchCache(::Ice::Int userId, const Ice::Current& ic) {
      SearchCacheResult result;
      return result;
    }

    Int2SearchCacheResultMap SearchLogicManagerI::getSearchCacheList(const ::MyUtil::IntSeq& userIds, const Ice::Current& ic) {
      Int2SearchCacheResultMap temp;
      return temp;
    }

    Int2ByteSeq SearchLogicManagerI::getSearchCacheList2(const ::MyUtil::IntSeq& userIds,
        const ::Ice::Current& ic) {
      Int2ByteSeq temp;
      return temp;
    }

    SearchLogicManagerI::SearchLogicManagerI() : _valid(false) {
    }

    void SearchLogicManagerI::bootstrap(int mod, int tt_index_cluster, vector<string>& dsn_name) {
      _mod = mod;
      _tt_index_cluster = tt_index_cluster;
      int i = 0;
      for(vector<string>::iterator iter = dsn_name.begin(); iter != dsn_name.end(); iter++)  {
        _db_cache_map[i] = DatabaseCacheFactory::instance().getDbCache(*iter);
        MCE_INFO("mod = "<<i<<" tt="<<*iter);
        if (_db_cache_map[i]->Initialize() == false) {
          MCE_WARN("Db" << i << " initialize failed");
        }
        i++;
      }

      DistSearchCacheAdapter::instance().initialize();
      for (int i=0; i<kMessageQueueThreadNum; ++i) {
        _vecMessageQueueThread.push_back(new MessageQueueThread(i));
      }

      for (MessageQueueThreadVec::const_iterator iter=_vecMessageQueueThread.begin(); 
          iter!=_vecMessageQueueThread.end(); ++iter) {
        (*iter)->start(128 * 1024).detach();
      }
    }

    void SearchLogicManagerI::load(const ::MyUtil::IntSeq& userIds, const Ice::Current&)  {
      return;
    }

    void SearchLogicManagerI::update(const UpdateUnitSeq& updateUnits, const Ice::Current& ic) {
      const Ice::ConnectionPtr con = ic.con;
      if (con.get() != NULL) {
        MCE_DEBUG("SearchLogicManagerI::update" << updateUnits.size() << " objects. invoker is :" << con->toString()  << ", facet=" << ic.facet << ", operation=" << ic.operation << ", requestId=" << ic.requestId << ", id.name=" << ic.id.name << ", id.category=" << ic.id.category << ", adapter.name=" << ic.adapter->getName());
      }
      if (updateUnits.empty())
        return;
      for(UpdateUnitSeq::const_iterator iter=updateUnits.begin(); iter!=updateUnits.end(); ++iter) {
        int user_id = iter->id;
        if (user_id >= 0) {
          (_vecMessageQueueThread.at(user_id % kMessageQueueThreadNum))->invoke(*iter);
        }
      }
    }

    bool SearchLogicManagerI::puts(const Int2ByteSeq& id2Map, const Ice::Current&) {
      MCE_DEBUG("SearchLogicManagerI::puts " << id2Map.size() << " objects. " );
      return true;
    }

    bool SearchLogicManagerI::removes(const MyUtil::LongSeq& userIds, const Ice::Current&) {
      MCE_DEBUG("SearchLogicManagerI::removes " << userIds.size() << " objects. " );
      return true;
    }

    bool SearchLogicManagerI::isValid(const Ice::Current&) {
      RWRecMutex::RLock lock(_validMutex);
      return _valid;
    }

    void SearchLogicManagerI::setValid(bool valid, const Ice::Current&) {
      MCE_DEBUG("SearchLogicManagerI::setValid -> "<<valid);
      IceUtil::RWRecMutex::WLock lock(_validMutex);
      _valid = valid;
      if (_valid) {
        ServerStateSubscriber::instance().setStatus(1);
      } else {
        ServerStateSubscriber::instance().setStatus(0);
      }
    }

    //***************************************************************************

    void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
      TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
    }

  }
}

