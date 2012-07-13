#include "SearchPersistentCacheI.h"
#include "execinfo.h"
#include "OceCxxAdapter/src/SearchCacheAdapter.h"
#include "OceCxxAdapter/src/SearchMemCacheAdapter.h"
//#include "OceCxxAdapter/src/TTManagerAdapter.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"
#include "XceCache.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"



using namespace com::xiaonei::xce;

using namespace xce::serverstate;

//#include <pthread.h>
boost::shared_ptr<xce::searchcache::IDbCache> xce::searchcache::UpdateLogTTTask::_db_cache;
  namespace MyUtil{
void initialize() {
	ServiceI& service = ServiceI::instance();

	int cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.Cluster");
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.Mod");
	string dsn  = service.getCommunicator()->getProperties()->getPropertyWithDefault("SearchCache.Dsn", TOKYOTYRANT_DSN_PREFIX"127.0.0.1:1982");//1981原来
	MCE_DEBUG("SearchPersistentCache data source name: " << dsn );
  //initialize=1，读取全部数据库，初始化PersistentCache
  //注意，在配置search_searchpersistentcache.xml时，只能指定一个服务进行初始化工作 
  int initialize = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.Initialize");

  service.registerObjectCache(xce::searchcache::SEARCH_CACHE, "SC", new xce::searchcache::SearchCacheFactoryI());

	service.getAdapter()->add(&xce::searchcache::SearchPersistentCacheManagerI::instance(),service.createIdentity(xce::searchcache::SEARCH_PERSISTENT_CACHE_MANAGER, ""));

  ServerStateSubscriber::instance().initialize("ControllerSearchPersistentCache", &xce::searchcache::SearchPersistentCacheManagerI::instance(), mod);

  xce::searchcache::SearchPersistentCacheManagerI::instance().setValid(true);


	TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL_BASIC, ThreadPoolConfig(1, 64));
	TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL_CONFIG, ThreadPoolConfig(1, 8));
	TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL_USERS, ThreadPoolConfig(1, 8));
	TaskManager::instance().config(xce::searchcache::TASK_LEVEL_UPDATE_DBCACHE, ThreadPoolConfig(32, 64));


  xce::searchcache::SearchPersistentCacheManagerI::instance().bootstrap(cluster, mod, dsn, initialize);

	service.registerXceObserver(new xce::searchcache::ServiceSchemaI);

  //配置日志TT
  ostringstream update_dsn("SearchCache.LogTTDsn");
  string strDsn = service.getCommunicator()->getProperties()->getProperty(update_dsn.str());
  MCE_INFO("SearchPersistentCache " << update_dsn.str()  << " data source name: " << strDsn);
  xce::searchcache::UpdateLogTTTask::_db_cache = xce::searchcache::DatabaseCacheFactory::instance().getDbCache(strDsn);
  if (xce::searchcache::UpdateLogTTTask::_db_cache->Initialize() == false) {
    MCE_WARN("LogTT initialize failed");
  }



}
}

//***************************************************************************

//全局函数将更新时需要查询mysql的多个语句模块化
namespace xce{
    namespace searchcache{
bool UpdateSqlBatch(MyUtil::LongSeq& uids);
//bool showSearchCacheData(SearchCacheDataPtr& obj);
using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using std::queue;


const int kMessageQueueThreadNum = 1000;

SearchCacheResult SearchPersistentCacheManagerI::getSearchCache(::Ice::Int userId, const Ice::Current& ic) {
  SearchCacheResult result;
  return result;
}

Int2SearchCacheResultMap SearchPersistentCacheManagerI::getSearchCacheList(const ::MyUtil::IntSeq& userIds, const Ice::Current& ic) {
  Int2SearchCacheResultMap temp;
  return temp;
}
Int2ByteSeq SearchPersistentCacheManagerI::getSearchCacheList2(const ::MyUtil::IntSeq& userIds, 
    const ::Ice::Current& ic) {
  Int2ByteSeq temp;
  return temp;
}
/*void SearchPersistentCacheManagerI::setValid(bool valid, const Ice::Current&) {
  return ;
}*/


SearchPersistentCacheManagerI::SearchPersistentCacheManagerI() : _valid(false) {
}

void SearchPersistentCacheManagerI::bootstrap(int cluster, int mod, string dsn, int initialize) {
  _cluster = cluster;
  _mod = mod;
  _dsn = dsn;

  ServiceI& service = ServiceI::instance();

  for (int i=0; i<_cluster; ++i) {
    ostringstream dsn; 
    dsn << "SearchCache.InitDsn" << i;
    //获得需要初始化的数据源
    //数据源格式：SearchPersistentCache.InitDsn0=tokyotyrant://127.0.0.1:1981
    //有多少个cluster就有多少个数据源，数据源的其实id=0
    string strDSN = service.getCommunicator()->getProperties()->getProperty(dsn.str());
    MCE_INFO("MODIFY NAME: "<< strDSN);
    MCE_DEBUG("SearchPersistentCache " << dsn.str()  << " data source name: " << strDSN);
    db_cache_map_[i] = DatabaseCacheFactory::instance().getDbCache(strDSN);

    if (db_cache_map_[i]->Initialize() == false) {
      MCE_WARN("Db" << i << " initialize failed");
    }     
  }

  if (initialize == 1)
    TaskManager::instance().execute(new FillTask());

  //TaskManager::instance().execute(new UpdateViewCountTask());
  for (int i=0; i<kMessageQueueThreadNum; ++i) {
    _vecMessageQueueThread.push_back(new MessageQueueThread(i));
    //_vecMessageQueueThread.at(i)->start(128*1024).detach();
  }

  for (MessageQueueThreadVec::const_iterator iter=_vecMessageQueueThread.begin(); 
      iter!=_vecMessageQueueThread.end(); ++iter) {
    (*iter)->start(128 * 1024).detach();
  }
}

void SearchPersistentCacheManagerI::load(const ::MyUtil::IntSeq& ids, const Ice::Current& ic) {
  const Ice::ConnectionPtr con = ic.con;
  if (con.get() != NULL) {
    MCE_INFO("SearchPersistentCacheManagerI::load " << ids.size() << " objects. invoker is :" << con->toString() );
  }
 for (MyUtil::IntSeq::const_iterator it=ids.begin(); it!=ids.end(); ++it) {
   std::map<string, string> fieldPairMap;
   //TODO(zhigang.wu@opi-corp.com):增加update多数据接口, 减少函数调用次数
   SearchCacheAdapter::instance().update(*it, "", fieldPairMap);
 }

}

void SearchPersistentCacheManagerI::update(const UpdateUnitSeq& updateUnits, const Ice::Current& ic) {
  const Ice::ConnectionPtr con = ic.con;
  if (con.get() != NULL) {
    //MCE_INFO("SearchPersistentCacheManagerI::update" << updateUnits.size() << " objects. invoker is :" << con->toString() );
    MCE_INFO("SearchPersistentCacheManagerI::update" << updateUnits.size() << " objects. invoker is :" << con->toString()  << ", facet=" << ic.facet << ", operation=" << ic.operation << ", requestId=" << ic.requestId << ", id.name=" << ic.id.name << ", id.category=" << ic.id.category << ", adapter.name=" << ic.adapter->getName());
   // for (map<string, string>::const_iterator iter=ic.ctx.begin(); iter!=ic.ctx.end(); ++iter) {
   //   MCE_INFO("ic.context.key=" << iter->first << ", ic.context.value=" << iter->second);
   // }
  }


  if (updateUnits.empty())
    return;
  ///#define DEBUG
  ///#ifdef DEBUG
  ///    MCE_INFO("[TEMPLE] update2load...");
  ///    MyUtil::IntSeq ids;
  ///    for(UpdateUnitSeq::const_iterator iter=updateUnits.begin(); iter!=updateUnits.end(); ++iter)
  ///    {
  ///      ids.push_back(iter->id);
  ///    }
  ///    SearchCacheAdapter::instance().load(ids);
  ///    return;
  ///#endif

  for(UpdateUnitSeq::const_iterator iter=updateUnits.begin(); iter!=updateUnits.end(); ++iter) {
    int user_id = iter->id;
    if (user_id >= 0) {
      (_vecMessageQueueThread.at(user_id % kMessageQueueThreadNum))->invoke(*iter);
    }
  }
}

bool SearchPersistentCacheManagerI::puts(const Int2ByteSeq& id2Map, const Ice::Current&) {
  MCE_DEBUG("SearchPersistentCacheManagerI::puts " << id2Map.size() << " objects. " );
  return true;
}

bool SearchPersistentCacheManagerI::removes(const MyUtil::LongSeq& userIds, const Ice::Current&) {
  MCE_DEBUG("SearchPersistentCacheManagerI::removes " << userIds.size() << " objects. " );
  return true;
}


bool SearchPersistentCacheManagerI::isValid(const Ice::Current&) {
  RWRecMutex::RLock lock(_validMutex);
  return _valid;
}

void SearchPersistentCacheManagerI::setValid(bool valid, const Ice::Current&) {
  MCE_INFO("SearchPersistentCacheManagerI::setValid -> "<<valid);
  IceUtil::RWRecMutex::WLock lock(_validMutex);
  _valid = valid;
  if (_valid) {
    ServerStateSubscriber::instance().setStatus(1);
  } else {
    ServerStateSubscriber::instance().setStatus(0);
  }
}


//***************************************************************************
LoadUpdateDataToDbCacheTask::LoadUpdateDataToDbCacheTask(MyUtil::LongSeq uids) : 
  MyUtil::Task(TASK_LEVEL_UPDATE_DBCACHE), uids_(uids) {
  }

void LoadUpdateDataToDbCacheTask::handle() {
  MyUtil::LongSeq& ids = uids_;

  sort(ids.begin(), ids.end());
  MyUtil::LongSeq::iterator end = unique(ids.begin(), ids.end());
  ids.resize(end - ids.begin());

  MCE_DEBUG("Loading " << ids.size() << " objects");
  if (ids.empty()) {
    return;
  }

  UpdateSqlBatch(ids);

  for (MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    long userId = (*it);
    SearchCacheDataPtr obj = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,userId); 
    if (NULL != obj.get()) {
      boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(userId); 
      if (db_handler) {
        if (obj->_status < 3) {
          if (!db_handler->Insert(userId, *obj.get())) {
            MCE_WARN("db_handler->Insert("<<userId<<") Failed");
          } else {
            //过渡代码，向另一个机群的TT写数据
            try {
             // TTManagerAdapter::instance().Insert(userId, *obj.get());
            } catch (IceUtil::Exception& e) {
              MCE_WARN("TTManagerAdapter::instance().Insert Exception " << e.what());
            } catch (...) {
              MCE_WARN("TTManagerAdapter::instance().Insert unknown Exception ");
            }
          }

          //MCE_DEBUG("[TEMPLE] in LoadUpdateDataToDbCacheTask, before obj is: ");
          //showSearchCacheData(obj);
        } else {
          MCE_INFO("the user "<<userId<<" status is "<<obj->_status);
          if (!db_handler->Remove(userId)) {
            MCE_WARN("remove the user "<<userId<<" from dbcache Failed!");
          } else {
            try {
              //TTManagerAdapter::instance().Remove(static_cast<int>(userId));
            } catch (IceUtil::Exception& e) {
              MCE_WARN("TTManagerAdapter::instance().Remove Exception " << e.what());
            } catch (...) {
              MCE_WARN("TTManagerAdapter::instance().Remove Unknown Exception ");
            }
          }
        }

        ServiceI::instance().getObjectCache()->removeObject(SEARCH_CACHE, userId);
      } else {
        MCE_WARN("DB handle " << userId << " is error");
      }
    } else {
      MCE_DEBUG("cannot find Id = " << userId << " in temple Object Cache");
    }
  }

  return;
}

UpdateViewCountTask::UpdateViewCountTask() :
  MyUtil::Task(TASK_LEVEL_UPDATE_DBCACHE) {
  }
void UpdateViewCountTask::handle() {
  //MCE_INFO("[TEMPLE] step in UpdateViewCountTask::handle");
  ServiceI& service = ServiceI::instance();
  ostringstream time_interval;
  //默认从当天的1点20开始执行更新view数据的线程
  string string_start_time =  service.getCommunicator()->getProperties()->getPropertyWithDefault("SearchCache.UpdateViewCountStartTime","2350");
  MCE_INFO("UpdateViewCountStartTime is "<<string_start_time);
  time_t start_time, current_time;

  time_t temp_calender_time = time(NULL);
  struct tm temp_broken_down_time;
  if (!localtime_r(&temp_calender_time, &temp_broken_down_time)) {
    MCE_FATAL("ERROR!localtime_r");
  }

  stringstream ss;
  // ss << string_start_time.substr(0,4);
  // ss >> temp_broken_down_time.tm_year;
  // temp_broken_down_time.tm_year -= 1900;

  // ss.str("");
  // ss.clear();
  // ss << string_start_time.substr(4,2); 
  // ss >> temp_broken_down_time.tm_mon;
  // temp_broken_down_time.tm_mon -= 1;

  // ss.str("");
  // ss.clear();
  // ss << string_start_time.substr(6,2); 
  // ss >> temp_broken_down_time.tm_mday;
  ss << string_start_time.substr(0,2); 
  ss >> temp_broken_down_time.tm_hour;

  ss.str("");
  ss.clear();
  ss << string_start_time.substr(2,4);
  ss >> temp_broken_down_time.tm_min;

  temp_broken_down_time.tm_sec=0;
  start_time = mktime(&temp_broken_down_time);

  //MCE_INFO("UpdateViewCountStartTime: " << start_time);

  time_interval<< "SearchCache.UpdateViewCountTimeInterval";
  //默认时间间隔为168=7*24 小时
  int dwTimeInterval =  service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(time_interval.str(), 168);
  MCE_INFO("UpdateViewCountTimeInterval: " << dwTimeInterval<<" hours");

  while(true) {

    current_time = time(NULL);
    MCE_INFO("[TEMPLE] current_time="<<current_time<<", start_time="<<start_time);
    if (current_time >= start_time) {
      MCE_INFO("[TEMPLE] current_time>=start_time");
      break;
    }
    MCE_INFO("[TEMPLE] current_time<start_time, so sleep 60 sec");
    sleep(60);
  }

  while(true) {
    //MCE_INFO("[TEMPLE] begin execute UpdateViewCountOnceTask()");
    //TaskManager::instance().execute(new UpdateViewCountOnceTask());
    UpdateViewCountOnceTask update_view_count_task;
    update_view_count_task.handle();
    //MCE_INFO("[TEMPLE] after execute UpdateViewCountOnceTask(), beginning sleep long time ...");
    sleep(dwTimeInterval*60*60);
  }

}

UpdateViewCountOnceTask::UpdateViewCountOnceTask() :
  MyUtil::Task(TASK_LEVEL_UPDATE_DBCACHE) {
  }
void UpdateViewCountOnceTask::handle() {
  MCE_INFO("[TEMPLE] begin execute UpdateViewCountOnceTask");
  ServiceI& service = ServiceI::instance();
  int beginId = 0;
  int maxId = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.MaxId");
  MCE_INFO("begin user_id is "<<maxId);
  int cycles = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.Cycles");
  MCE_INFO("begin cycles is "<<cycles);

  int idx=cycles;
  do {
    beginId = maxId;
    set<int> valid_ids_set;
    {
      Statement status_sql;
      status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
        << beginId << " LIMIT " << UPDATE_VIEW_COUNT_BATCH_SIZE;
      BasicStatusResultHandler2I status_handler(maxId, valid_ids_set);
      QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
      //           ServiceI::instance().getObjectCache()->addObjects(SEARCH_CACHE, results->data);
    }
    {
      Statement sql;
      sql << "SELECT " << USER_VIEWCOUNT_FIELD << " FROM view_count WHERE id > "
        << beginId << " AND id <= " << maxId 
        <<" LIMIT " << UPDATE_VIEW_COUNT_BATCH_SIZE;
      BatchViewCountResultHandlerI view_count_handler(valid_ids_set);
      QueryRunner(DB_VIEW_COUNT, CDbRServer).query(sql, view_count_handler);
    }
    sleep(1);
    if (0 == ++idx % 100) {
      MCE_INFO("UpdateViewCount has processed "<<maxId<<" user ids in "<< idx << " cycles");
    }
  } while (beginId < maxId);
  MCE_INFO("UpdateViewCountOnceTask done");
}

FillTask::FillTask() :
  MyUtil::Task(TASK_LEVEL_FILL) {
  }

void FillTask::handle() {
  ServiceI& service = ServiceI::instance();
  int beginId = 0;
  //int maxId = 0;
  int maxId = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.MaxId");
  int count = 0;
  int startId = 0;

  try{

    do {
      beginId = maxId;
      //		ObjectResultPtr results = new ObjectResult;
      {

        Statement status_sql;
        status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
          << beginId << " LIMIT " << FILL_BATCH_SIZE;
        BatchBasicStatusResultHandlerI status_handler(maxId);
        QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
        //           ServiceI::instance().getObjectCache()->addObjects(SEARCH_CACHE, results->data);
      }
      {

        Statement sql1;

        sql1 << "SELECT " << USER_BORN_FIELD << " FROM user_born WHERE id > "
          << beginId << " AND id <= " << maxId <<" LIMIT " 
          << FILL_BATCH_SIZE;

        TaskManager::instance().execute(
            new BatchTask("id", DB_SEARCH2_USERBORN, 
              &SearchCacheData::setUserBornProperties, sql1, 
              TASK_LEVEL_FILL_BASIC));

      }

      {
        Statement sql1;
        sql1 << "SELECT " << USER_URL_FIELD << " FROM user_url WHERE id > "
          << beginId << " AND id <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("id", DB_SEARCH2_USERURL, 
              &SearchCacheData::setUserUrlProperties, sql1, 
              TASK_LEVEL_FILL_BASIC));
      }

      {
        Statement sql1;
        sql1 << "SELECT " << USER_STATE_FIELD << " FROM user_state WHERE id > "
          << beginId << " AND id <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("id", DB_SEARCH2_USERSTATE, 
              &SearchCacheData::setUserStateProperties, sql1, 
              TASK_LEVEL_FILL_CONFIG));
      }

      {
        Statement sql1;
        sql1 << "SELECT " << USER_CONFIG_FIELD << " FROM user_config WHERE id > "
          << beginId << " AND id <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("id", DB_SEARCH2_USERCONFIG, 
              &SearchCacheData::setConfigProperties, sql1, 
              TASK_LEVEL_FILL_CONFIG));
      }

      {
        Statement sql1;
        sql1 << "SELECT " << USER_AUTH_FIELD << " FROM user_right WHERE id > "
          << beginId << " AND id <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("id", DB_USER_RIGHT, 
              &SearchCacheData::setUserAuthProperties, sql1, 
              TASK_LEVEL_FILL_CONFIG));

      }

      {
        Statement sql1;
        sql1 << "SELECT " << USER_VIEWCOUNT_FIELD << " FROM view_count WHERE id > "
          << beginId << " AND id <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("id", DB_VIEW_COUNT, 
              &SearchCacheData::setViewCountProperties, sql1, 
              TASK_LEVEL_FILL_CONFIG));
      }

      {
        Statement sql1;
        sql1 << "SELECT " << USER_NAME_FIELD << " FROM user_names WHERE id > "
          << beginId << " AND id <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("id", DB_SEARCH2_USERNAME, 
              &SearchCacheData::setUserNameProperties, sql1, 
              TASK_LEVEL_FILL_USERS));
      }

      {
        Statement sql1;
        sql1 << "SELECT " << USER_STAGE_FIELD << " FROM user_stage WHERE id > "
          << beginId << " AND id <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("id", DB_SEARCH2_USERSTAGE, 
              &SearchCacheData::setUserStageProperties, sql1, 
              TASK_LEVEL_FILL_USERS));
      }


      {
        Statement sql2;
        sql2 << "SELECT " << NETWORK_HISTORY_FIELD << " FROM network_history WHERE status<=0 AND userid  > "
          << beginId << " AND userid <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("userid", DB_SEARCH2_USERBASIC, 
              &SearchCacheData::setNetworkProperties, sql2, 
              TASK_LEVEL_FILL_BASIC));
      }

      {
        Statement sql3;
        sql3 << "SELECT " << ELEMENTARY_SCHOOL_INFO_FIELD << " FROM elementary_school_info WHERE userid  > "
          << beginId << " AND userid <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("userid", DB_SEARCH2_USERBASIC, 
              &SearchCacheData::setElementarySchoolProperties, sql3, 
              TASK_LEVEL_FILL_BASIC));
      }

      {
        Statement sql4;
        sql4 << "SELECT " << JUNIOR_HIGH_SCHOOL_INFO_FIELD << " FROM junior_high_school_info WHERE userid  > "
          << beginId << " AND userid <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("userid", DB_SEARCH2_USERBASIC, 
              &SearchCacheData::setJuniorSchoolProperties, sql4, 
              TASK_LEVEL_FILL_BASIC));
      }

      {
        Statement sql5;
        sql5 << "SELECT " << HIGH_SCHOOL_INFO_FIELD << " FROM high_school_info WHERE userid  > "
          << beginId << " AND userid <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("userid", DB_SEARCH2_USERBASIC, 
              &SearchCacheData::setHighSchoolProperties, sql5, 
              TASK_LEVEL_FILL_BASIC));
      }

      {
        Statement sql6;
        sql6 << "SELECT " << COLLEGE_INFO_FIELD << " FROM college_info WHERE userid  > "
          << beginId << " AND userid <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("userid", DB_SEARCH2_USERBASIC, 
              &SearchCacheData::setCollegeProperties, sql6, 
              TASK_LEVEL_FILL_BASIC));
      }

      {
        Statement sql7;
        sql7 << "SELECT " << UNIVERSITY_INFO_FIELD << " FROM university_info WHERE userid  > "
          << beginId << " AND userid <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("userid", DB_SEARCH2_USERBASIC, 
              &SearchCacheData::setUniversityProperties, sql7, 
              TASK_LEVEL_FILL_BASIC));
      }

      {
        Statement sql8;
        sql8 << "SELECT " << WORKPLACE_INFO_FIELD << " FROM workplace_info WHERE userid  > "
          << beginId << " AND userid <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("userid", DB_SEARCH2_USERBASIC, 
              &SearchCacheData::setWorkplaceProperties, sql8, 
              TASK_LEVEL_FILL_BASIC));
      }

      {
        Statement sql9;
        sql9 << "SELECT " << REGION_INFO_FIELD << " FROM region_info WHERE userid  > "
          << beginId << " AND userid <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("userid", DB_SEARCH2_USERBASIC, 
              &SearchCacheData::setRegionProperties, sql9, 
              TASK_LEVEL_FILL_BASIC));
      }
      {
        for(int i=0; i<100; i++) {
          ostringstream table_name;
          table_name << "user_fond_" << i;
          Statement sql;
          sql  << "SELECT " << USER_FOND_FIELD << " FROM " << table_name.str() << " WHERE user_id > "
            << beginId << " AND user_id <= " << maxId << " LIMIT "
            << FILL_BATCH_SIZE;
          TaskManager::instance().execute(
              new BatchTask("user_id", DB_SEARCH2_USERFOND, 
                &SearchCacheData::setUserFondProperties, sql, 
                TASK_LEVEL_FILL_BASIC));

        }
      }
      {
        Statement sql;
        sql  << "SELECT " << USER_TIME_FIELD << " FROM user_time WHERE id > "
          << beginId << " AND id <= " << maxId << " LIMIT "
          << FILL_BATCH_SIZE;
        TaskManager::instance().execute(
            new BatchTask("id", DB_SEARCH2_USERTIME, 
              &SearchCacheData::setLastlogintimeProperties, sql, 
              TASK_LEVEL_FILL_BASIC));
      }

      count += (maxId - beginId);

      //每10万存储一次
      if (count > 100000) {
        count = 0;
       
        for(int userId = startId; userId<=maxId; ++userId) {
          SearchCacheDataPtr obj = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,userId); 
          if (NULL != obj.get()) {
            boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(userId); 
            if (db_handler) {
              if (obj->_status < 3) {
                if (!db_handler->Insert(userId, *obj.get())) {
                  MCE_WARN("db_handler->Insert("<<userId<<") Failed");
                } else {
                  try {
                    //TTManagerAdapter::instance().Insert(userId, *obj.get());
                  } catch (IceUtil::Exception& e) {
                    MCE_WARN("TTManagerAdapter::instance().Insert Exception " << e.what());
                  } catch (...) {
                    MCE_WARN("TTManagerAdapter::instance().Insert unknown Exception ");
                  }
                }
              } else {
                if (!db_handler->Remove(userId)) {
                  MCE_WARN("remove the user "<<userId<<" from dbcache Failed!");
                } else {
                  try {
                    //TTManagerAdapter::instance().Remove(static_cast<int>(userId));
                  } catch (IceUtil::Exception& e) {
                    MCE_WARN("TTManagerAdapter::instance().Remove Exception " << e.what());
                  } catch (...) {
                    MCE_WARN("TTManagerAdapter::instance().Remove Unknown Exception ");
                  }
                }
              }

              ServiceI::instance().getObjectCache()->removeObject(SEARCH_CACHE, userId);
            } else {
              MCE_WARN("DB handle " << userId << " is error");
            }
          } else {
            MCE_DEBUG("cannot find Id = " << userId << " in temple Object Cache");
          }
        }
        startId = maxId;
      }
    } while (beginId < maxId);

    for(int userId = startId; userId<=maxId; ++userId) {
      SearchCacheDataPtr obj = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,userId); 
      if (NULL != obj.get()) {
        boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(userId); 
        if (db_handler) {
          if (obj->_status < 3) {
            if (!db_handler->Insert(userId, *obj.get())) {
              MCE_WARN("db_handler->Insert("<<userId<<") Failed");
            } else {
              //过渡代码
              try {
                //TTManagerAdapter::instance().Insert(userId, *obj.get());
              } catch (IceUtil::Exception& e) {
                MCE_WARN("TTManagerAdapter::instance().Insert Exception " << e.what());
              } catch (...) {
                MCE_WARN("TTManagerAdapter::instance().Insert unknown Exception ");
              }
            }
          } else {
            if (!db_handler->Remove(userId)) {
              MCE_WARN("remove the user "<<userId<<" from dbcache Failed!");
            } else {
              MCE_INFO(userId<<" status is "<<obj->_status<<", remove form db");
              try {
                //TODO:非常ugly，彻底解决long 和 int的问题
                //TTManagerAdapter::instance().Remove(static_cast<int>(userId));
              } catch (IceUtil::Exception& e) {
                MCE_WARN("TTManagerAdapter::instance().Remove Exception " << e.what());
              } catch (...) {
                MCE_WARN("TTManagerAdapter::instance().Remove Unknown Exception ");
              }
            }
          }

          ServiceI::instance().getObjectCache()->removeObject(SEARCH_CACHE, userId);
        } else {
          MCE_WARN("DB handle " << userId << " is error");
        }
      } else {
        MCE_DEBUG("cannot find Id = " << userId << " in temple Object Cache");
      }
    }
  } catch(...) {
    MCE_WARN("FillTask done with exception");
  }
  //SearchPersistentCacheManagerI::instance().setValid(true);
  MCE_INFO("FillTask done.");
}
//***************************************************************************

SearchCacheFactoryI::SearchCacheFactoryI() {
}

Ice::ObjectPtr SearchCacheFactoryI::create(Ice::Long id) {
  MCE_DEBUG("XXXX Loading " << id);
  return NULL;
}

MyUtil::ObjectResultPtr SearchCacheFactoryI::create(const MyUtil::LongSeq& ids) {

  MCE_DEBUG("XXXX Loading " << ids.size() << " objects");
  ObjectResultPtr results = new ObjectResult;
  return results;
}

SingleResultHandlerI::SingleResultHandlerI(SearchCacheDataPtr& result, void (SearchCacheData::*p)(mysqlpp::Row& row)) :
  _result(result), _pFunc(p) {
  }

bool SingleResultHandlerI::handle(mysqlpp::Row& row) const {
  ((_result.get())->*_pFunc)(row);
  return false;
}
//***************************************************************************
BatchResultHandlerI::BatchResultHandlerI(std::string id, void (SearchCacheData::*p)(mysqlpp::Row& row)):
  _id(id), _pFunc(p) {
  }

bool BatchResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row[_id.c_str()];
  SearchCacheDataPtr data=ServiceI::instance().findObject<SearchCacheDataPtr> (SEARCH_CACHE,id);
  if (NULL != data.get()) {
    ((data.get())->*_pFunc)(row);
  }
  return true;
}

//***************************************************************************
BatchVectorResultHandlerI::BatchVectorResultHandlerI(std::string idField, map<long, vector<mysqlpp::Row> > &id2RowsMap):
  _idField(idField), _id2RowsMap(id2RowsMap) {
  }

bool BatchVectorResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row[_idField.c_str()];
  _id2RowsMap[id].push_back(row);

  return true;
}
//***************************************************************************
BatchBasicStatusResultHandlerI::BatchBasicStatusResultHandlerI(Ice::Int& maxId) :
  _maxId(maxId){
  }

bool BatchBasicStatusResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  _maxId = id > _maxId ? id : _maxId;

  SearchCacheDataPtr obj = new (std::nothrow) SearchCacheData; 
  if (NULL != obj.get()) {
    obj->_id = id;
    obj->setUserStatusProperties(row);
    ServiceI::instance().getObjectCache()->addObject(SEARCH_CACHE, id, obj);
  }

  return true;
}

//***************************************************************************
BasicStatusResultHandler2I::BasicStatusResultHandler2I(Ice::Int& maxId, set<int>& valid_ids_set) :
  _maxId(maxId), valid_ids_set_(valid_ids_set) {
  }

bool BasicStatusResultHandler2I::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  _maxId = id > _maxId ? id : _maxId;
  int status = (int) row["status"];
  if (status < 3) {
    valid_ids_set_.insert(id);
  }

  return true;
}
BatchViewCountResultHandlerI::BatchViewCountResultHandlerI(set<int>& valid_ids_set) :
  valid_ids_set_(valid_ids_set){
  }

bool BatchViewCountResultHandlerI::handle(mysqlpp::Row& row) const {
  int user_id = (int) row["id"];
  if (valid_ids_set_.find(user_id) != valid_ids_set_.end()) {
    int view_count = (int) row["viewcount"];
    //--boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(user_id);    
    //--if (db_handler) {
    //--  SearchCacheDataPtr temp_obj = new SearchCacheData;
    //--  if (db_handler->Query(user_id, temp_obj.get())) {
    //--    //如果viewcount字段值没有变化，就不再需要再写了
    //--    if (temp_obj->_viewcount == view_count) {
    //--      return true;
    //--    }
    //--  }
    //--} 

    //--//其它情况均需要写
    map<string, string> id_2_viewcount;
    id_2_viewcount["VIEWCOUNT"] = Int2String(view_count);
    SearchCacheAdapter::instance().update(user_id, "view_count", id_2_viewcount);
  }

  return true;
}

string BatchViewCountResultHandlerI::Int2String(int view_count) const {
  stringstream  ss;
  ss<<view_count;
  string string_value;
  ss>>string_value;
  return string_value;
}


//***************************************************************************
BatchTask::BatchTask(std::string id, std::string db, void (SearchCacheData::*p)(mysqlpp::Row& row), 
    const Statement& sql,int level):Task(level),_sql(sql), _db(db), _id(id), _pFunc(p){
}

void BatchTask::handle(){
  BatchResultHandlerI handler(_id, _pFunc);
  QueryRunner(_db, CDbRServer).query(_sql, handler);
}

//***************************************************************************

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
}

UpdateTask::UpdateTask() {
  }

void UpdateTask::handle(queue<boost::shared_ptr<UpdateUnit> >& update_units_ptr_queue) {
  //google::InstallFailureSignalHandler();

  try {

  MyUtil::LongSeq updateIds;
  UpdateUnitSeq updateTables, updateFieldMap;
  set<string>fieldTable, vectorTable;

  //以下表名信息都是vector类型的，即在数据库中，对应同一个用户id，可能有多条记录
  //例如 在多个学校上过大学, 故赵宁那边只能给到形如 id:table 的更改信息 
  vectorTable.insert("network_history"); 
  vectorTable.insert("elementary_school_info"); 
  vectorTable.insert("junior_high_school_info"); 
  vectorTable.insert("high_school_info"); 
  vectorTable.insert("college_info"); 
  vectorTable.insert("university_info"); 
  vectorTable.insert("workplace_info"); 
  vectorTable.insert("region_info"); 

  //以下表中更改的信息 赵宁将给出形如 id:table:字段名:字段值 的形式
  fieldTable.insert("user_passport"); 
  fieldTable.insert("user_names");
  fieldTable.insert("user_born");
  fieldTable.insert("user_time");
  fieldTable.insert("user_url");
  fieldTable.insert("user_fond");
  fieldTable.insert("user_state");
  fieldTable.insert("user_stage");
  fieldTable.insert("user_config");
  fieldTable.insert("view_count");
  fieldTable.insert("user_right");

  while (!update_units_ptr_queue.empty()) {
    boost::shared_ptr<UpdateUnit> ptr = update_units_ptr_queue.front();
    update_units_ptr_queue.pop();
    if (ptr->table.compare("") == 0) {
      updateIds.push_back(ptr->id);
    } else if (vectorTable.find(ptr->table)!=vectorTable.end()) {
      updateTables.push_back(*ptr);
    } else if (fieldTable.find(ptr->table)!=fieldTable.end()) {
      updateFieldMap.push_back(*ptr);
    }
  }

  MCE_INFO("Ids size: " << updateIds.size());
  MCE_INFO("table size: " << updateTables.size());
  MCE_INFO("field map size " << updateFieldMap.size());

///  #define DEBUG
///  #ifdef DEBUG
///      MCE_DEBUG("[TEMPLE] input updateUnits is:");
///      for(UpdateUnitSeq::const_iterator iter=updateUnits.begin(); iter!=updateUnits.end(); iter++)
///      {    
///        MCE_DEBUG("[TEMPLE] id:"<<iter->id);
///        MCE_DEBUG("    [TEMPLE] table:"<<iter->table);
///        for(map<string, string>::const_iterator fieldIter=iter->fieldPairMap.begin(); fieldIter!=iter->fieldPairMap.end(); fieldIter++)
///        {    
///          MCE_DEBUG("       [TEMPLE] field key:"<<fieldIter->first);
///          MCE_DEBUG("       [TEMPLE] field value:"<<fieldIter->second);
///        }    
///      }    
///  #endif
  
  MyUtil::LongSeq queryFailedIds;
  MyUtil::LongSeq updateLogTTIds, deleteIds;
  Int2ByteSeq id2Map;

  UpdatePCAccordingIdAndTableName(updateTables, queryFailedIds, updateLogTTIds, deleteIds, id2Map);
  //MCE_DEBUG("[TEMPLE] after table, updateIds.size is"<<updateIds.size()<<", queryFailedIds.size is "<<queryFailedIds.size());
  UpdatePCAccordingFieldMap(updateFieldMap, queryFailedIds, updateLogTTIds, deleteIds, id2Map);
  //MCE_DEBUG("[TEMPLE] before merge, updateIds.size is"<<updateIds.size()<<", queryFailedIds.size is "<<queryFailedIds.size());
  updateIds.insert(updateIds.end(), queryFailedIds.begin(), queryFailedIds.end());
  UpdatePCAccordingIds(updateIds, updateLogTTIds, deleteIds, id2Map);

  TaskManager::instance().execute(new UpdateLogTTTask(deleteIds, updateLogTTIds));
  //MCE_DEBUG("[TEMPLE] doUpdateMemCacheData...");
  SearchCacheAdapter::instance().doUpdateMemCacheData(id2Map);
  SearchMemCacheAdapter::instance().doUpdateMemCacheData(id2Map);
///  {
///    IntSequence userIds;
///    StringList string_list;
///    for (Int2ByteSeq::const_iterator iter = id2Map.begin(); iter!=id2Map.end(); ++iter){
///      userIds.push_back(iter->first);
///      string byte_seq_string((iter->second).begin(), (iter->second).end());
///      string_list.push_back(byte_seq_string);
///    }
///    try {
///      //xce::XceCacheAdapter::instance().Puts(userIds, string_list);
///    } catch (...) {
///      MCE_WARN("ERROR!xce::XceCacheAdapter::instance().Puts");
///    }
///  }
  //MCE_DEBUG("[TEMPLE] doRemoveMemCacheData...");
  SearchCacheAdapter::instance().doRemoveMemCacheData(deleteIds);
  SearchMemCacheAdapter::instance().doRemoveMemCacheData(deleteIds);
///  {
///    MyUtil::IntSeq userIds;
///    for (MyUtil::LongSeq::const_iterator iter=deleteIds.begin(); iter!=deleteIds.end(); ++iter) {
///      userIds.push_back(*iter);
///    }
///    try {
///      //xce::XceCacheAdapter::instance().Removes(userIds);
///    } catch (...) {
///      MCE_WARN("ERROR!xce::XceCacheAdapter::instance().Removes");
///    }
///  }

  } catch (IceUtil::Exception& e) {
    MCE_ERROR("UpdateTask::handle Exception: " << e.what());
  } catch (std::exception& e) {
    MCE_ERROR("UpdateTask::handle Exception: " << e.what());
  } catch (...) {
    MCE_ERROR("UpdateTask::handle unknown Exception ");
  }
}

UpdateLogTTTask::UpdateLogTTTask(MyUtil::LongSeq& deleteLogTTIds, MyUtil::LongSeq& updateLogTTIds) : 
  MyUtil::Task(TASK_LEVEL_UPDATE_DBCACHE), _deleteLogTTIds(deleteLogTTIds), _updateLogTTIds(updateLogTTIds) {
  }

void UpdateLogTTTask::handle() {
  DoUpdateLogTT(_deleteLogTTIds, DELETE_PREFIX);
  DoUpdateLogTT(_updateLogTTIds, UPDATE_PREFIX);
}

void UpdateLogTTTask::DoUpdateLogTT(MyUtil::LongSeq& ids, const string PREFIX) {
  MCE_DEBUG("UpdateLogTT, ids.size()=" << ids.size());

  if (ids.empty()) {
    return;
  }

  try {
    if (_db_cache) {

      sort(ids.begin(), ids.end());
      MyUtil::LongSeq::iterator end = unique(ids.begin(), ids.end());
      ids.resize(end - ids.begin());

      char valuebuf[IO_BUFFER_SIZE];
      char *pBuf = valuebuf;

      if (ids.size()*sizeof(int) > IO_BUFFER_SIZE) {
        pBuf = new char [ids.size()*sizeof(int)];
      } 

      char* value_buf = pBuf;
      size_t value_size = 0;

      //将所有的userid按顺序存储到buf中
      for (MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
        int id = (*it);
        memcpy(pBuf, &id, sizeof(int));
        pBuf += sizeof(int);
        value_size += sizeof(int);
      }

      //获得当前时间
      std::ostringstream strDate;
      time_t now = time(NULL);
      struct tm result;
      if (!localtime_r(&now, &result)) {
        MCE_FATAL("不能获得正确的时间" << LongSeq2String(ids));
        goto _exit;
      }

      char date[200];
      if (strftime(date, sizeof(date), "%Y%m%d%H%M", &result) == 0) {
        MCE_FATAL("格式化时间错误" << LongSeq2String(ids));
        goto _exit;
      }

      strDate << PREFIX << date;

      if (!(_db_cache->Concatenate(strDate.str().c_str(), strDate.str().length(), value_buf, value_size))) {
        MCE_FATAL("insert LogTT failed" << LongSeq2String(ids));
      } else {
        MCE_DEBUG("[DEBUG]insert LogTT strDate is "<<strDate.str()<<"(" << LongSeq2String(ids)<<")");
      }

_exit:
      if (value_size> IO_BUFFER_SIZE) {
        delete pBuf;
      }

    }
  } catch (...) {
    MCE_WARN("UpdateLogTTTask::DoUpdateLogTT unknown exception");
  }

  return;
}

string UpdateLogTTTask::LongSeq2String(const MyUtil::LongSeq& uids) {
  std::ostringstream str;
  str << " ";
  for (MyUtil::LongSeq::const_iterator it = uids.begin(); it != uids.end(); ++it) {
    str << boost::lexical_cast<string>(*it) << " ";
  }   
  return str.str();
}


void UpdateTask::UpdatePCAccordingIds(MyUtil::LongSeq& uids, MyUtil::LongSeq& updateLogTTIds, MyUtil::LongSeq& deleteIds, Int2ByteSeq& id2BytesMap) {
  MCE_DEBUG("[TEMPLE] step in UpdatePCAndMCAccordingIds, size is "<<uids.size());
  if (uids.empty()) {
    return;
  }

  MyUtil::LongSeq& ids = uids;

  sort(ids.begin(), ids.end());
  MyUtil::LongSeq::iterator end = unique(ids.begin(), ids.end());
  ids.resize(end - ids.begin());

  try{

    //查询多个数据库
    UpdateSqlBatch(ids);

    {
      for (MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
        long userId = (*it);
        SearchCacheDataPtr obj = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,userId); 
        if (NULL != obj.get()) {
          boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(userId); 
          if (db_handler) {
            //MCE_DEBUG("[TEMPLE] in UpdateAccordingIds, before writeback, obj is: ");
            //showSearchCacheData(obj);
            Write2TTAndMemCacheBuffer(userId, obj, db_handler, deleteIds, updateLogTTIds, id2BytesMap);
          } else {
            MCE_WARN("DB getHandler ERROR!");
          }
        } else {
          MCE_DEBUG("cannot find Id = " << userId << " in temple Object Cache");
        }
      }

    }
    //SearchCacheAdapter::instance().doUpdateMemCacheData(id2BytesMap);
  } catch(Ice::Exception& e) {
    MCE_FATAL("ERROR! "<<e);
  } catch (...) {
    MCE_FATAL("UpdateAccordingIds() run ERROR!");
  }

  MCE_DEBUG("[TEMPLE] step out UpdatePCAndMCAccordingIds");
}


void UpdateTask::UpdatePCAccordingIdAndTableName(UpdateUnitSeq& updateTables, MyUtil::LongSeq& queryFailedIds, MyUtil::LongSeq& updateLogTTIds, MyUtil::LongSeq& deleteIds, Int2ByteSeq& id2BytesMap) {
  MCE_DEBUG("[TEMPLE] step in UpdatePCAndMCAccordingIdAndTableName, size=" << updateTables.size());
  if (updateTables.empty())
    return;

  UpdateUnitSeq::const_iterator iter;
  MyUtil::LongSeq network_history_ids, elementary_school_info_ids, junior_high_school_info_ids, 
    high_school_info_ids, college_info_ids, university_info_ids, workplace_info_ids, region_info_ids;

  for(iter=updateTables.begin(); iter!=updateTables.end(); iter++) {
    if (0 == iter->id) {
      continue;
    }
    if (0 == iter->table.compare("network_history")) {
      network_history_ids.push_back(iter->id);
    } else if (0 == iter->table.compare("elementary_school_info")) {
      elementary_school_info_ids.push_back(iter->id);
    } else if (0 == iter->table.compare("junior_high_school_info")) {
      junior_high_school_info_ids.push_back(iter->id);
    } else if (0 == iter->table.compare("high_school_info")) {
      high_school_info_ids.push_back(iter->id);
    } else if (0 == iter->table.compare("college_info")) {
      college_info_ids.push_back(iter->id);
    } else if (0 == iter->table.compare("university_info")) {
      university_info_ids.push_back(iter->id);
    } else if (0 == iter->table.compare("workplace_info")) {
      workplace_info_ids.push_back(iter->id);
    } else if (0 == iter->table.compare("region_info")) {
      region_info_ids.push_back(iter->id);
    }
  }

  {
    Statement sql;
    sql << "SELECT " << NETWORK_HISTORY_FIELD << " FROM network_history WHERE status <=0 AND userid IN ";
    UpdateSpecialTable(network_history_ids, sql, &SearchCacheData::setNetworkPropertiesAccordingVector, queryFailedIds, updateLogTTIds, deleteIds, id2BytesMap);
  }
  {
    Statement sql;
    sql << "SELECT " << ELEMENTARY_SCHOOL_INFO_FIELD << " FROM elementary_school_info WHERE userid IN ";
    UpdateSpecialTable(elementary_school_info_ids, sql, &SearchCacheData::setElementarySchoolPropertiesAccordingVector,  queryFailedIds, updateLogTTIds, deleteIds, id2BytesMap);
  }
  {
    Statement sql;
    sql << "SELECT " << JUNIOR_HIGH_SCHOOL_INFO_FIELD << " FROM junior_high_school_info WHERE userid IN ";
    UpdateSpecialTable(junior_high_school_info_ids, sql, &SearchCacheData::setJuniorSchoolPropertiesAccordingVector, queryFailedIds, updateLogTTIds, deleteIds, id2BytesMap);
  }
  {
    Statement sql;
    sql << "SELECT " << HIGH_SCHOOL_INFO_FIELD << " FROM high_school_info WHERE userid IN ";
    UpdateSpecialTable(high_school_info_ids, sql, &SearchCacheData::setHighSchoolPropertiesAccordingVector, queryFailedIds, updateLogTTIds, deleteIds, id2BytesMap);
  }
  {
    Statement sql;
    sql << "SELECT " << COLLEGE_INFO_FIELD << " FROM college_info WHERE userid IN ";
    UpdateSpecialTable(college_info_ids, sql, &SearchCacheData::setCollegePropertiesAccordingVector, queryFailedIds, updateLogTTIds, deleteIds, id2BytesMap);
  }
  {
    Statement sql;
    sql << "SELECT " << UNIVERSITY_INFO_FIELD << " FROM university_info WHERE userid IN ";
    UpdateSpecialTable(university_info_ids, sql, &SearchCacheData::setUniversityPropertiesAccordingVector, queryFailedIds, updateLogTTIds, deleteIds, id2BytesMap);
  }
  {
    Statement sql;
    sql << "SELECT " << WORKPLACE_INFO_FIELD << " FROM workplace_info WHERE userid IN ";
    UpdateSpecialTable(workplace_info_ids, sql, &SearchCacheData::setWorkplacePropertiesAccordingVector, queryFailedIds, updateLogTTIds, deleteIds, id2BytesMap);
  }
  {
    Statement sql;
    sql << "SELECT " << REGION_INFO_FIELD << " FROM region_info WHERE userid IN ";
    UpdateSpecialTable(region_info_ids, sql, &SearchCacheData::setRegionPropertiesAccordingVector, queryFailedIds, updateLogTTIds, deleteIds, id2BytesMap);
  }

 /// //用于统计哪个表更新的最频繁
 /// if (!network_history_ids.empty()) {
 ///   MCE_INFO("STAT TABLE network_history " << network_history_ids.size());
 /// }
 /// if (!elementary_school_info_ids.empty()) {
 ///   MCE_INFO("STAT TABLE elementary_school_info " << elementary_school_info_ids.size());
 /// }
 /// if (!high_school_info_ids.empty()) {
 ///   MCE_INFO("STAT TABLE high_school_info " << high_school_info_ids.size());
 /// }
 /// if (!junior_high_school_info_ids.empty()) {
 ///   MCE_INFO("STAT TABLE junior_high_school_info " << junior_high_school_info_ids.size());
 /// }
 /// if (!college_info_ids.empty()) {
 ///   MCE_INFO("STAT TABLE college_info " << college_info_ids.size());
 /// }
 /// if (!university_info_ids.empty()) {
 ///   MCE_INFO("STAT TABLE university_info " << university_info_ids.size());
 /// }
 /// if (!workplace_info_ids.empty()) {
 ///   MCE_INFO("STAT TABLE workplace_info " << workplace_info_ids.size());
 /// }
 /// if (!region_info_ids.empty()) {
 ///   MCE_INFO("STAT TABLE region_info " << region_info_ids.size());
 /// }

  MCE_DEBUG("[TEMPLE] step out UpdatePCAndMCAccordingIdAndTableName");
}

void UpdateTask::UpdatePCAccordingFieldMap(UpdateUnitSeq& updateFieldMap, MyUtil::LongSeq& queryFailedIds, MyUtil::LongSeq& updateLogTTIds, MyUtil::LongSeq& deleteIds, Int2ByteSeq& id2BytesMap) {
  MCE_DEBUG("[TEMPLE] step in UpdatePCAndMCAccordingFieldMap, size is "<<updateFieldMap.size());
  if (updateFieldMap.empty())
    return;

  map<string, int> field2count;
  try{
    UpdateUnitSeq::const_iterator updateUnitIter;

    map<string, int*> mapString2IntField;
    map<string, string*> mapString2StringField;

    //用于统计计数, 分析哪个字段更新的最频繁
    field2count["user_passport:STATUS"] = 0;
    field2count["user_names:NAME"] = 0;
    field2count["user_born:GENDER"] = 0;
    field2count["user_names:NICKNAME"] = 0;
    field2count["user_time:LASTLOGINTIME"] = 0;
    field2count["user_url:TINYURL"] = 0;
    field2count["user_url:HEADURL"] = 0;
    field2count["user_born:ASTROLOGY"] = 0;
    field2count["user_born:HOME_CITY"] = 0;
    field2count["user_born:HOME_PROVINCE"] = 0;
    field2count["user_fond:INTEREST"] = 0;
    field2count["user_fond:MUSIC"] = 0;
    field2count["user_fond:MOVIE"] = 0;
    field2count["user_fond:GAME"] = 0;
    field2count["user_fond:COMIC"] = 0;
    field2count["user_fond:SPORT"] = 0;
    field2count["user_fond:BOOK"] = 0;
    field2count["user_fond:SOCIETY"] = 0;
    field2count["user_state:STAR"] = 0;
    field2count["user_stage:STAGE"] = 0;
    field2count["user_state:STATE"] = 0;
    field2count["user_born:BIRTHDAY_YEAR"] = 0;
    field2count["user_born:BIRTHDAY_MONTH"] = 0;
    field2count["user_born:BIRTHDAY_DAY"] = 0;
    field2count["user_config:BROWSE_CONFIG"] = 0;
    field2count["user_config:STATUS_CONFIG"] = 0;
    field2count["user_config:BASIC_CONFIG"] = 0;
    field2count["user_right:AUTH"] = 0;
    field2count["view_count:VIEWCOUNT"] = 0;


    //用户自动化修改
    mapString2IntField["user_passport:STATUS"] = NULL;
    mapString2StringField["user_names:NAME"] = NULL;
    mapString2StringField["user_born:GENDER"] = NULL;
    mapString2StringField["user_names:NICKNAME"] = NULL;
    mapString2StringField["user_time:LASTLOGINTIME"] = NULL;
    mapString2StringField["user_url:TINYURL"] = NULL;
    mapString2StringField["user_url:HEADURL"] = NULL;
    mapString2StringField["user_born:ASTROLOGY"] = NULL;
    mapString2StringField["user_born:HOME_CITY"] = NULL;
    mapString2StringField["user_born:HOME_PROVINCE"] = NULL;
    mapString2StringField["user_fond:INTEREST"] = NULL;
    mapString2StringField["user_fond:MUSIC"] = NULL;
    mapString2StringField["user_fond:MOVIE"] = NULL;
    mapString2StringField["user_fond:GAME"] = NULL;
    mapString2StringField["user_fond:COMIC"] = NULL;
    mapString2StringField["user_fond:SPORT"] = NULL;
    mapString2StringField["user_fond:BOOK"] = NULL;
    mapString2StringField["user_fond:SOCIETY"] = NULL;
    
    mapString2StringField["user_config:PROFILE_PRIVACY"] = NULL;
    mapString2IntField["user_state:STAR"] = NULL;
    mapString2IntField["user_stage:STAGE"] = NULL;
    mapString2IntField["user_state:STATE"] = NULL;
    mapString2IntField["user_born:BIRTHDAY_YEAR"] = NULL;
    mapString2IntField["user_born:BIRTHDAY_MONTH"] = NULL;
    mapString2IntField["user_born:BIRTHDAY_DAY"] = NULL;
    mapString2IntField["user_config:BROWSE_CONFIG"] = NULL;
    mapString2IntField["user_config:STATUS_CONFIG"] = NULL;
    mapString2IntField["user_config:BASIC_CONFIG"] = NULL;
    mapString2IntField["user_config:PROFILE_PRIVACY"] = NULL;
    mapString2IntField["view_count:VIEWCOUNT"] = NULL;


    for(updateUnitIter=updateFieldMap.begin(); updateUnitIter!=updateFieldMap.end(); updateUnitIter++) {
      boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(updateUnitIter->id); 

      if (db_handler) {
        //先遍历看是否有关心的字段
        {
          bool has_valid_filed = false;
          string tableName = updateUnitIter->table;
          transform(tableName.begin(), tableName.end(), tableName.begin(), ::tolower);
          MCE_DEBUG("[TEMPLE] in updateAccordingMap, id is "<<updateUnitIter->id<<", tablename is "<<updateUnitIter->table); 
          for(map<string, string>::const_iterator fieldIter=updateUnitIter->fieldPairMap.begin(); fieldIter!=updateUnitIter->fieldPairMap.end(); fieldIter++) {
            //关键字段若为空，则不进行更新
            if (0 == tableName.compare("user_names") && 0 == (fieldIter->first).compare("NAME") && 0 == (fieldIter->second).compare("")) {
              MCE_INFO("NAME is empty, so dump it");
              break;
            }

            string table_name_and_field = tableName + ":" + fieldIter->first;
            map<string, string*>::iterator stringFieldIter = mapString2StringField.find(table_name_and_field);
            map<string, int*>::iterator intFieldIter = mapString2IntField.find(table_name_and_field);

            if (stringFieldIter != mapString2StringField.end()) {
              //计数加一
              ++field2count[table_name_and_field];

              has_valid_filed = true;
              break;
            } else if (intFieldIter != mapString2IntField.end()) {
              ++field2count[table_name_and_field];

              has_valid_filed = true;
              break;
            }
          }

          if (!has_valid_filed) {
            //MCE_DEBUG("[TEMPLE] have no valid field, so go to next unit");
            continue;
          }
        }


        SearchCacheDataPtr obj = new (std::nothrow) SearchCacheData; 
        //MCE_DEBUG("[TEMPLE] read logic...");

        if (!ReadSearchCacheDataFromTT(updateUnitIter->id, obj, db_handler)) {
          queryFailedIds.push_back(updateUnitIter->id);
          continue;
        }

        //MCE_DEBUG("[TEMPLE] change logic...");
        //修改临时ObjectCache
        {
          mapString2IntField["user_passport:STATUS"] = &obj->_status;
          mapString2StringField["user_names:NAME"] = &obj->_name;
          mapString2StringField["user_born:GENDER"] = &obj->_gender;
          mapString2StringField["user_names:NICKNAME"] = &obj->_signature;
          mapString2StringField["user_time:LASTLOGINTIME"] = &obj->_lastlogintime;
          mapString2StringField["user_url:TINYURL"] = &obj->_tinyurl;
          mapString2StringField["user_url:HEADURL"] = &obj->_headurl;
          mapString2StringField["user_born:ASTROLOGY"] = &obj->_astrology;
          mapString2StringField["user_born:HOME_CITY"] = &obj->_home_city;
          mapString2StringField["user_born:HOME_PROVINCE"] = &obj->_home_province;
          mapString2StringField["user_fond:INTEREST"] = &obj->_interest;
          mapString2StringField["user_fond:MUSIC"] = &obj->_music;
          mapString2StringField["user_fond:MOVIE"] = &obj->_movie;
          mapString2StringField["user_fond:GAME"] = &obj->_game;
          mapString2StringField["user_fond:COMIC"] = &obj->_comic;
          mapString2StringField["user_fond:SPORT"] = &obj->_sport;
          mapString2StringField["user_fond:BOOK"] = &obj->_book;
          mapString2StringField["user_fond:SOCIETY"] = &obj->_society;
          mapString2StringField["user_config:PROFILE_PRIVACY"] = &obj->_profile_privacy;
          
          mapString2IntField["user_state:STAR"] = &obj->_star;
          mapString2IntField["user_stage:STAGE"] = &obj->_stage;
          mapString2IntField["user_state:STATE"] = &obj->_state;
          mapString2IntField["user_born:BIRTH_YEAR"] = &obj->_birthday_year;
          mapString2IntField["user_born:BIRTH_MONTH"] = &obj->_birthday_month;
          mapString2IntField["user_born:BIRTH_DAY"] = &obj->_birthday_day;
          mapString2IntField["user_config:BROWSE_CONFIG"] = &obj->_browse_config;
          mapString2IntField["user_config:STATUS_CONFIG"] = &obj->_status_config;
          mapString2IntField["user_config:BASIC_CONFIG"] = &obj->_basic_config;
          mapString2IntField["user_right:AUTH"] = &obj->_auth;
          
          mapString2IntField["view_count:VIEWCOUNT"] = &obj->_viewcount;

          {
            //MCE_DEBUG("***********[TEMPLE] before change template object, old obj is ");
            //showSearchCacheData(obj);
            map<string, string>::const_iterator fieldIter;
            string tableName = updateUnitIter->table;
            for(fieldIter=updateUnitIter->fieldPairMap.begin(); fieldIter!=updateUnitIter->fieldPairMap.end(); fieldIter++) {
              map<string, string*>::iterator stringFieldIter = mapString2StringField.find(tableName+":"+fieldIter->first);
              map<string, int*>::iterator intFieldIter = mapString2IntField.find(tableName+":"+fieldIter->first);
              //MCE_DEBUG("[TEMPLE] table is "<<tableName<<", field key is "<<fieldIter->first<<", field value is "<<fieldIter->second);
              if (0==fieldIter->first.compare("STATUS") && 0==tableName.compare("user_passport")) {
                //MCE_DEBUG("[TEMPLE] change obj->_status to "<<fieldIter->second);
                obj->_status = String2Int(fieldIter->second);
              } else if (stringFieldIter != mapString2StringField.end()) {
                //MCE_DEBUG("[TEMPLE] change "<<fieldIter->first<<" to "<<fieldIter->second);
                  if(0 == fieldIter->first.compare("PROFILE_PRIVACY") && 0 == tableName.compare("user_config"))  {
                    boost::shared_ptr<SearchCacheData> sc_data = boost::shared_ptr<SearchCacheData>(new SearchCacheData());
                    obj->_profile_privacy = sc_data->splitProfilePrivacy(fieldIter->second);
                  }  else {
                    *(stringFieldIter->second) = fieldIter->second;
                  }
              } else if (intFieldIter != mapString2IntField.end()) {
                //MCE_DEBUG("[TEMPLE] change "<<fieldIter->first<<" to "<<fieldIter->second);
                *(intFieldIter->second) = String2Int(fieldIter->second);
              }
            }
            //MCE_DEBUG("***********[TEMPLE] after change template object, obj is ");
            //showSearchCacheData(obj);
          }
        }
        //MCE_DEBUG("[TEMPLE] write back logic...");
        //
        Write2TTAndMemCacheBuffer(updateUnitIter->id, obj, db_handler, deleteIds, updateLogTTIds, id2BytesMap);
      } else {
        MCE_WARN("getDbHandler ERROR!");
      }
    }

  } catch (const Ice::Exception& e) {
    MCE_WARN(" UpdateAccordingFieldMap Ice::Exception:"<<e.what() );
  } catch (std::exception& e) {
    MCE_WARN("UpdateAccordingFieldMap std::exception:" << e.what());
  } catch(...) {
    MCE_WARN(" UpdateAccordingFieldMap unknown exception");
  }

 /// //输出日志, 用于统计哪个字段更新最频繁
 // for (map<string, int>::const_iterator iter=field2count.begin(); iter!=field2count.end(); ++iter) {
 //   if (0 != iter->second) {
 //     MCE_INFO("STAT FIELD " << iter->first << " " << iter->second);
 //   }
 // }
  MCE_DEBUG("[TEMPLE] step out of UpdatePCAndMCAccordingFieldMap");
}

void UpdateTask::UpdateSpecialTable(MyUtil::LongSeq& uids, Statement& sql, void (SearchCacheData::*pFunc)(vector<mysqlpp::Row>& rows), MyUtil::LongSeq& queryFailedIds, MyUtil::LongSeq& updateLogTTIds, MyUtil::LongSeq& deleteIds, Int2ByteSeq& id2BytesMap) {
  MCE_DEBUG("[TEMPLE] step in UpdateSpecialTable, size is "<< uids.size());
  if (uids.empty()) {
    return;
  }

  MyUtil::LongSeq& ids = uids;
  std::set<long> setQueryFailedIds;

  sort(ids.begin(), ids.end());
  MyUtil::LongSeq::const_iterator end = unique(ids.begin(), ids.end());
  ids.resize(end - ids.begin());

  ostringstream oss;
  string strSuffix;
  MyUtil::LongSeq::const_iterator it = ids.begin();
  SearchCacheDataPtr  obj;
  oss << "(" ;

  while (it!=ids.end()) {
    boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(*it); 
    if (db_handler) {
      SearchCacheDataPtr obj = new (std::nothrow) SearchCacheData;
      if (!ReadSearchCacheDataFromTT(*it, obj, db_handler)) {
        setQueryFailedIds.insert(*it);
        ++it;
      } else {
        oss << *it;
        ++it;
        break;
      }
    } else {
      MCE_WARN("getDbHandler Error");
      ++it;
    }
  }

  for (; it != ids.end(); ++it) {
    boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(*it); 
    if (db_handler) {
      SearchCacheDataPtr obj = new (std::nothrow) SearchCacheData;
      if (!ReadSearchCacheDataFromTT(*it, obj, db_handler)) {
        setQueryFailedIds.insert(*it);
        continue;
      } else {
        oss << ", " << *it;
      }
    } else {
      MCE_WARN("DB getDbHandler ERROR");
    }
  }
  oss << ")";

  //for(std::set<long>::const_iterator iter=setQueryFailedIds.begin(); iter!=setQueryFailedIds.end(); ++iter) {
  //  queryFailedIds.push_back(*iter);
  //}
  queryFailedIds.insert(queryFailedIds.end(), setQueryFailedIds.begin(), setQueryFailedIds.end());

  if (0 == oss.str().compare("()")) {
    return;
  }

  strSuffix = oss.str();
  sql << strSuffix;

  {
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    for(MyUtil::LongSeq::const_iterator iter=ids.begin(); iter!=ids.end(); ++iter) {
      if(setQueryFailedIds.find(*iter) == setQueryFailedIds.end()) {
        vector<mysqlpp::Row>  row_vector;
        id2RowsMap[*iter] = row_vector;
      }
    }

    try {
      BatchVectorResultHandlerI handler("userid", id2RowsMap);
      QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
    } catch (mysqlpp::Exception& e) {
      MCE_FATAL("UpdateSpecialTable mysqlpp Exception: "<< e.what());
    } catch (Ice::Exception& e) {
      MCE_FATAL("UpdateSpecialTable Ice::Exception : " << e.what());
    } catch (std::exception& e) {
      MCE_FATAL("UpdateSpecialTable std::exception : " << e.what());
    } catch (...) {
      MCE_WARN("ERROR! in Func  UpdateTask::UpdateSpecialTable, unkonwn exception");
    }

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        if (setQueryFailedIds.find(iter->first)!=setQueryFailedIds.end())
          continue;
        (data.get()->*pFunc)(iter->second);
      }
    }
  }


  for (MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    long userId = (*it);

    boost::shared_ptr<IDbCache> db_handler = SearchPersistentCacheManagerI::instance().getDbHandler(userId); 
    SearchCacheDataPtr obj = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,userId);
    if (db_handler) {
      if (NULL != obj.get())
        Write2TTAndMemCacheBuffer(userId, obj, db_handler, deleteIds, updateLogTTIds, id2BytesMap);
    } else {
      MCE_WARN("DB handle " << userId << " is error");
    }
  }
  MCE_DEBUG("[TEMPLE] step out UpdateSpecialTable, size is "<< uids.size());
}

int UpdateTask::String2Int(const string& strInt) {
  stringstream ss(strInt);
  int intValue;
  ss>>intValue; 
  return intValue;
}
void UpdateTask::BinStr2ByteSeqMap(long userId, BinString* pBinString, Int2ByteSeq& id2Map) {
  char * pBuffer=NULL;
  int len=0;
  if (NULL == pBinString || 0 >= userId) {
    return;
  }

  pBinString->getBuf(&pBuffer, &len);

  if (NULL == pBuffer || 0 == len) {
    return;
  }

  id2Map[userId].assign(pBuffer, pBuffer+len);
}

//批量从多个数据库里读出数据
bool UpdateSqlBatch(MyUtil::LongSeq& uids) {
  ostringstream oss;
  string strSuffix;
  MyUtil::LongSeq::const_iterator it = uids.begin();

  if (uids.size() == 1 && *it == 0) {
    return true;
  }

  oss << "(" ;
  if (*it != 0) {
    oss << *it;
  }
  else {
    oss <<*(++it);
  }
  ++it;
  for (; it != uids.end(); ++it) {
    oss << ", " << *it;
  }
  oss << ")";


  strSuffix = oss.str();
  {
    Statement status_sql;
    status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id IN "
      << strSuffix;
    int maxId;
    BatchBasicStatusResultHandlerI status_handler(maxId);
    QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
    //    ServiceI::instance().getObjectCache()->addObjects(SEARCH_CACHE, results->data);
  }
  {

    Statement sql;
    sql << "SELECT " << USER_BORN_FIELD << " FROM user_born WHERE id IN "
      << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setUserBornProperties);
    QueryRunner(DB_SEARCH2_USERBORN, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << USER_URL_FIELD << " FROM user_url WHERE id IN "
      << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setUserUrlProperties);
    QueryRunner(DB_SEARCH2_USERURL, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << USER_STATE_FIELD << " FROM user_state WHERE id IN "
      << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setUserStateProperties);
    QueryRunner(DB_SEARCH2_USERSTATE, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << USER_CONFIG_FIELD << " FROM user_config WHERE id IN "
      << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setConfigProperties);
    QueryRunner(DB_SEARCH2_USERCONFIG, CDbRServer).query(sql, handler);
  }

  {
    Statement sql;
    sql << "SELECT " << USER_AUTH_FIELD << " FROM user_right WHERE id IN "
      << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setUserAuthProperties);
    QueryRunner(DB_USER_RIGHT, CDbRServer).query(sql, handler);
  }


  {
    Statement sql;
    sql << "SELECT " << USER_VIEWCOUNT_FIELD << " FROM view_count WHERE id IN "
      << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setViewCountProperties);
    QueryRunner(DB_VIEW_COUNT, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << USER_NAME_FIELD << " FROM user_names WHERE id IN "
      << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setUserNameProperties);
    QueryRunner(DB_SEARCH2_USERNAME, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << USER_STAGE_FIELD<< " FROM user_stage WHERE id IN "
      << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setUserStageProperties);
    QueryRunner(DB_SEARCH2_USERSTAGE, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql
      << "SELECT " << NETWORK_HISTORY_FIELD << " FROM network_history WHERE status <=0 AND userid IN "
      << strSuffix;
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    BatchVectorResultHandlerI handler("userid", id2RowsMap);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        data->setNetworkPropertiesAccordingVector(iter->second);
      }
    }
  }
  {
    Statement sql;
    sql << "SELECT " << ELEMENTARY_SCHOOL_INFO_FIELD << " FROM elementary_school_info WHERE userid IN "<< strSuffix;
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    BatchVectorResultHandlerI handler("userid", id2RowsMap);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        data->setElementarySchoolPropertiesAccordingVector(iter->second);
      }
    }
  }
  {
    Statement sql;
    sql << "SELECT " << JUNIOR_HIGH_SCHOOL_INFO_FIELD << " FROM junior_high_school_info WHERE userid IN "<< strSuffix;
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    BatchVectorResultHandlerI handler("userid", id2RowsMap);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        data->setJuniorSchoolPropertiesAccordingVector(iter->second);
      }
    }
  }
  {
    Statement sql;
    sql << "SELECT " << HIGH_SCHOOL_INFO_FIELD << " FROM high_school_info WHERE userid IN "<< strSuffix;
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    BatchVectorResultHandlerI handler("userid", id2RowsMap);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        data->setHighSchoolPropertiesAccordingVector(iter->second);
      }
    }
  }
  {
    Statement sql;
    sql << "SELECT " << COLLEGE_INFO_FIELD << " FROM college_info WHERE userid IN "<< strSuffix;
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    BatchVectorResultHandlerI handler("userid", id2RowsMap);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        data->setCollegePropertiesAccordingVector(iter->second);
      }
    }
  }
  {
    Statement sql;
    sql << "SELECT " << UNIVERSITY_INFO_FIELD << " FROM university_info WHERE userid IN "<< strSuffix;
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    BatchVectorResultHandlerI handler("userid", id2RowsMap);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        data->setUniversityPropertiesAccordingVector(iter->second);
      }
    }
  }
  {
    Statement sql;
    sql << "SELECT " << WORKPLACE_INFO_FIELD << " FROM workplace_info WHERE userid IN " << strSuffix;
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    BatchVectorResultHandlerI handler("userid", id2RowsMap);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        data->setWorkplacePropertiesAccordingVector(iter->second);
      }
    }
  }
  {
    Statement sql;
    sql << "SELECT " << REGION_INFO_FIELD << " FROM region_info WHERE userid IN " << strSuffix;
    map<long, vector<mysqlpp::Row> > id2RowsMap;
    BatchVectorResultHandlerI handler("userid", id2RowsMap);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
        iter != id2RowsMap.end(); 
        ++iter) {
      SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, iter->first);
      if (NULL != data.get()) {
        data->setRegionPropertiesAccordingVector(iter->second);
      }
    }
  }
  {
    map<int, string> mod_2_suffix;
    string user_fond_suffix;

    MyUtil::LongSeq::const_iterator user_fond_ids_iter = uids.begin();
    //默认uids已经是排过序的了
    if (0 == *user_fond_ids_iter)
      ++user_fond_ids_iter;
    while (user_fond_ids_iter != uids.end()) {
      //用于字符串与数字连接的临时对象
      ostringstream user_fond_temp_oss; 
      int user_id = *user_fond_ids_iter;
      int mod = user_id % 100;
      //若找到，只需填充类似 ", 20005882" 的串即可；
      //若未找到，则为第一个，填充"(20005882"形式的串
      if (mod_2_suffix.end() != mod_2_suffix.find(mod)) {
        user_fond_temp_oss << mod_2_suffix[mod];
        user_fond_temp_oss << ", " << user_id;
      } else {
        user_fond_temp_oss << "(" << user_id;
      }
      mod_2_suffix[mod] = user_fond_temp_oss.str();
      ++user_fond_ids_iter;
    }

    for(int i=0; i<100; i++) {
      ostringstream table_name;
      table_name << "user_fond_" << i;
      if (mod_2_suffix.end() != mod_2_suffix.find(i)) {
        string user_fond_suffix = mod_2_suffix[i] + ")";
        Statement sql;
        sql  << "SELECT " << USER_FOND_FIELD << " FROM "<<table_name.str()<<" WHERE user_id IN "
          << user_fond_suffix;
        BatchResultHandlerI handler("user_id", &SearchCacheData::setUserFondProperties);
        QueryRunner(DB_SEARCH2_USERFOND, CDbRServer).query(sql, handler);
      }
    }
  }
  {
    Statement sql;
    sql  << "SELECT " << USER_TIME_FIELD << " FROM user_time WHERE id IN " << strSuffix;
    BatchResultHandlerI handler("id", &SearchCacheData::setLastlogintimeProperties);
    QueryRunner(DB_SEARCH2_USERTIME, CDbRServer).query(sql, handler);
  }

  return true;
}

void UpdateTask::Write2TTAndMemCacheBuffer(long userId, SearchCacheDataPtr& obj, boost::shared_ptr<IDbCache> &db_handler, MyUtil::LongSeq& deleteIds, MyUtil::LongSeq& updateLogTTIds, Int2ByteSeq& id2BytesMap) {
  if (!obj || !db_handler || userId < 0) {
    return;
  }

  if (obj->_status < 3) {
    if (db_handler->Insert(userId, *obj.get())) {
      try {
        //TTManagerAdapter::instance().Insert(userId, *obj.get());
      } catch (IceUtil::Exception& e) {
        MCE_WARN("TTManagerAdapter::instance().Insert Exception " << e.what());
      } catch (...) {
        MCE_WARN("TTManagerAdapter::instance().Insert unknown Exception ");
      }

      updateLogTTIds.push_back(userId);
      BinStringPtr binStrPtr = obj->Serialize();
      if (binStrPtr) {
        BinStr2ByteSeqMap(userId, binStrPtr.get(), id2BytesMap);
      }
    } else {
      MCE_WARN("db_handler->Insert("<<userId<<") Failed");
    }
  } else {
    MCE_DEBUG("obj->_status is "<<obj->_status<<", so remove it");
    deleteIds.push_back(userId);
    if (!db_handler->Remove(userId)) {
      MCE_WARN("remove the user "<<userId<<" from dbcache Failed!");
    } else {
      try {
        //TTManagerAdapter::instance().Remove(static_cast<int>(userId));
      } catch (IceUtil::Exception& e) {
        MCE_WARN("TTManagerAdapter::instance().Remove Exception " << e.what());
      } catch (...) {
        MCE_WARN("TTManagerAdapter::instance().Remove Unknown Exception ");
      }
    }
  }

  ServiceI::instance().getObjectCache()->removeObject(SEARCH_CACHE, userId);
}

bool UpdateTask::ReadSearchCacheDataFromTT(long userId, SearchCacheDataPtr& obj,boost::shared_ptr<IDbCache> &db_handler) {
  if (userId < 0 || !db_handler || !obj) {
    return false;
  }

  if (!db_handler->Query(userId, obj.get())) { 
    MCE_DEBUG("db_handler->Query("<<userId<<") Failed");
    return false;
  } else {
    ServiceI::instance().getObjectCache()->addObject(SEARCH_CACHE, userId, obj);
    return true;
  }
}

///bool showSearchCacheData(SearchCacheDataPtr& obj) {
///    const std::string SEARCH_CACHE_TYPE_NETWORK = "network";
///    const std::string SEARCH_CACHE_TYPE_JUNIOR_SCHOOL = "junior_school";
///    const std::string SEARCH_CACHE_TYPE_HIGH_SCHOOL = "high_school";
///    const std::string SEARCH_CACHE_TYPE_ELEMENTARY_SCHOOL = "elementary_school";
///    const std::string SEARCH_CACHE_TYPE_COLLEGE = "college";
///    const std::string SEARCH_CACHE_TYPE_UNIVERSITY = "university";
///    const std::string SEARCH_CACHE_TYPE_WORKPLACE = "workplace";
///    const std::string SEARCH_CACHE_TYPE_REGION = "region";
///
///    const std::string SEARCH_CACHE_ID = "user_basic.id";
///    const std::string SEARCH_CACHE_NAME = "name";
///    const std::string SEARCH_CACHE_SIGNATURE = "signature";
///    const std::string SEARCH_CACHE_STAR = "star";
///    const std::string SEARCH_CACHE_STATE= "state";
///    const std::string SEARCH_CACHE_SELECTED = "selected";
///    const std::string SEARCH_CACHE_GENDER = "gender";
///    const std::string SEARCH_CACHE_BIRTHDAY_YEAR = "birth_year";
///    const std::string SEARCH_CACHE_STATUS = "status";
///    const std::string SEARCH_CACHE_TINYURL = "tinyurl";
///    const std::string SEARCH_CACHE_HEADURL = "headurl";
///    const std::string SEARCH_CACHE_BROWSE_CONFIG = "browse_config";
///    const std::string SEARCH_CACHE_STATUS_CONFIG = "status_config";
///    const std::string SEARCH_CACHE_BASIC_CONFIG  = "basic_config";
///    const std::string SEARCH_CACHE_VIEW_COUNT  = "viewcount";
///    const std::string SEARCH_CACHE_HOME_CITY = "home_city";
///    const std::string SEARCH_CACHE_HOME_PROVINCE = "home_province";
///    const std::string SEARCH_CACHE_NETWORK_ID = "network_id";
///    const std::string SEARCH_CACHE_NETWORK_NAME = "network_name";
///    const std::string SEARCH_CACHE_NETWORK_STAGE = "network_history.stage";
///    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_ID = "elementary_school_info.elementary_school_id";
///    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_YEAR = "elementary_school_info.elementary_school_year";
///    const std::string SEARCH_CACHE_ELEMENTARY_SCHOOL_NAME = "elementary_school_info.elementary_school_name";
///    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_ID = "junior_high_school_info.junior_high_school_id";
///    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_NAME = "junior_high_school_info.junior_high_school_name";
///    const std::string SEARCH_CACHE_JUNIOR_SCHOOL_YEAR = "junior_high_school_info.junior_high_school_year";
///    const std::string SEARCH_CACHE_HIGH_SCHOOL_YEAR = "enroll_year";
///    const std::string SEARCH_CACHE_HIGH_SCHOOL_NAME = "high_school_name";
///    const std::string SEARCH_CACHE_HIGH_SCHOOL_ID = "high_school_id";
///    const std::string SEARCH_CACHE_COLLEGE_ID = "college_info.college_id";
///    const std::string SEARCH_CACHE_COLLEGE_NAME ="college_info.college_name";
///    const std::string SEARCH_CACHE_COLLEGE_DEPARTMENT = "college_info.department";
///    const std::string SEARCH_CACHE_COLLEGE_YEAR = "college_info.enroll_year";
///    const std::string SEARCH_CACHE_UNIVERSITY_ID = "university_id";
///    const std::string SEARCH_CACHE_UNIVERSITY_NAME = "university_name";
///    const std::string SEARCH_CACHE_UNIVERSITY_YEAR = "enroll_year";
///    const std::string SEARCH_CACHE_UNIVERSITY_DEPARTMENT = "department";
///    const std::string SEARCH_CACHE_WORKPLACE_ID = "workplace_id";
///    const std::string SEARCH_CACHE_WORKPLACE_NAME = "workplace_name";
///    const std::string SEARCH_CACHE_REGION_ID = "region_id";
///    const std::string SEARCH_CACHE_REGION_PROVINCE = "province_name";
///    const std::string SEARCH_CACHE_REGION_CITY = "city_name";
///
///    try{
///        MCE_DEBUG("[TRACE] " << SEARCH_CACHE_ID   << " : " << obj->_id);
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_STATUS << " : " << obj->_status);
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_NAME << " : " << obj->_name );
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_SIGNATURE << " : " << obj->_signature);
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_HEADURL << " : " << obj->_headurl );
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_TINYURL << " : " << obj->_tinyurl);
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_STAR << " : " << obj->_star);
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_HOME_CITY   << " : " << obj->_home_city);
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_HOME_PROVINCE << " : " << obj->_home_province );
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_STATUS_CONFIG << " : " << obj->_status_config );
///        MCE_DEBUG("[TRACE] "<< SEARCH_CACHE_BROWSE_CONFIG << " : " << obj->_browse_config);
///        MCE_DEBUG("[TRACE] "<< "birthday_year" << " : " << obj->_birthday_year);
///        MCE_DEBUG("[TRACE] "<< "birthday_month" << " : " << obj->_birthday_month);
///        MCE_DEBUG("[TRACE] "<< "birthday_day" << " : " << obj->_birthday_day);
///        MCE_DEBUG("[TRACE] "<< "state" << " : " << obj->_state);
///        MCE_DEBUG("[TRACE] "<< "stage" << " : " << obj->_stage);
///        MCE_DEBUG("[TRACE] "<< "lastlogintime" << " : " << obj->_lastlogintime);
///        MCE_DEBUG("[TRACE] "<< "astrology" << " : " << obj->_astrology);
///        MCE_DEBUG("[TRACE] "<< "basic_config" << " : " << obj->_basic_config);
///        MCE_DEBUG("[TRACE] "<< "gender" << " : " << obj->_gender);
///        MCE_DEBUG("[TRACE] "<< "interest" << " : " << obj->_interest);
///        MCE_DEBUG("[TRACE] "<< "movie" << " : " << obj->_movie);
///        MCE_DEBUG("[TRACE] "<< "sport" << " : " << obj->_sport);
///        MCE_DEBUG("[TRACE] "<< "music" << " : " << obj->_music);
///        MCE_DEBUG("[TRACE] "<< "game" << " : " << obj->_game);
///        MCE_DEBUG("[TRACE] "<< "comic" << " : " << obj->_comic);
///        MCE_DEBUG("[TRACE] "<< "book" << " : " << obj->_book);
///        MCE_DEBUG("[TRACE] "<< "society" << " : " << obj->_society);
///        MCE_DEBUG("[TRACE] "<< "viewcount" << " : " << obj->_viewcount);
///
///
///        MCE_DEBUG("[TRACE] "<< "Network" );
///        for (int i = 0; i<obj->_networks.size(); ++i) {
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_networks[i].id);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_networks[i].stage);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_networks[i].name);
///        }
///
///        MCE_DEBUG("[TRACE] "<< "Elementary School" );
///
///        for (int i=0; i<obj->_elementarySchools.size(); ++i) {
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_elementarySchools[i].id);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_elementarySchools[i].year);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_elementarySchools[i].name);
///        }
///     
///        MCE_DEBUG("[TRACE] "<< "Junior School" );
///        for (int i = 0; i<obj->_juniorSchools.size(); ++i) {
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_juniorSchools[i].id);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_juniorSchools[i].year);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_juniorSchools[i].name);
///        }
///
///        MCE_DEBUG("[TRACE] "<<"High School" );
///        
///        for (int i = 0; i<obj->_highSchools.size(); ++i) {
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_highSchools[i].id);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_highSchools[i].year);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_highSchools[i].name);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_highSchools[i].class1);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_highSchools[i].class2);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_highSchools[i].class3);
///        }
///
///
///        MCE_DEBUG("[TRACE] "<<"Colege" );
///               for (int i = 0; i<obj->_colleges.size(); ++i) {
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_colleges[i].id);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_colleges[i].year);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_colleges[i].department);
///        }
///
///
///        MCE_DEBUG("[TRACE] "<<"University" );
///              for (int i = 0; i<obj->_universities.size(); ++i) {
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_universities[i].id);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_universities[i].year);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_universities[i].name);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_universities[i].department);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_universities[i].dorm);
///        }
///
///        MCE_DEBUG("[TRACE] "<<"Workplace" );
///             for (int i = 0; i<obj->_workplaces.size(); ++i) {
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_workplaces[i].id);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_workplaces[i].name);
///        }
///
///        MCE_DEBUG("[TRACE] "<<"Region" );
///        for (int i = 0; i<obj->_regions.size(); ++i) {
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_regions[i].id);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_regions[i].city);
///            MCE_DEBUG("[TRACE] "<< "\t" << obj->_regions[i].province);
///        }
///
///    } catch (const Ice::Exception& e) {
///        MCE_DEBUG("[TRACE] "<<e );
///        return false;
///    } catch (const char* msg) {
///        MCE_DEBUG("[TRACE] "<<msg );
///        return false;
///    }
///    return true;
///}
///
}
}
