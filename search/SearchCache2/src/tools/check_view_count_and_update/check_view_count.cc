#include "check_view_count.h"
#include <stdlib.h>
#include "LogWrapper.h"
#include "../../Util/DatabaseCacheFactory.h"
#include "ConnectionHolder.h"

using namespace xce;
using namespace xce::searchcache;
using namespace com::xiaonei::xce;

DbHandler* DbHandler::instance_ = 0;
IceUtil::Mutex DbHandler::mutex_;

typedef boost::shared_ptr<IDbCache> IDbCachePtr;

const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";
const string DB_VIEW_COUNT = "user_view_count"; 
const int UPDATE_VIEW_COUNT_BATCH_SIZE = 1600;  
const string USER_STATUS_FIELD = " id, status "; 
const string USER_VIEWCOUNT_FIELD = " id, viewcount ";
const int kCluster = 4;
using std::cout;
using std::endl;

bool DbHandler::Initialize() {
  cluster_ = 4;
  string tt_name; 
  IDbCachePtr db_cache;

  tt_name = TOKYOTYRANT_DSN_PREFIX"s17:1981";
  db_cache = DatabaseCacheFactory::instance().getDbCache(tt_name);
  if (db_cache->Initialize() == false) {
    MCE_FATAL("Db" << tt_name << " initialize failed");
    goto __error;
  }    
  db_handler_map_[0] = db_cache;

  tt_name = TOKYOTYRANT_DSN_PREFIX"s18:1981";
  db_cache = DatabaseCacheFactory::instance().getDbCache(tt_name);
  if (db_cache->Initialize() == false) {
    MCE_FATAL("Db" << tt_name << " initialize failed");
    goto __error;
  }    
  db_handler_map_[1] = db_cache;

  tt_name = TOKYOTYRANT_DSN_PREFIX"s19:1981";
  db_cache = DatabaseCacheFactory::instance().getDbCache(tt_name);
  if (db_cache->Initialize() == false) {
    MCE_FATAL("Db" << tt_name << " initialize failed");
    goto __error;
  }    
  db_handler_map_[2] = db_cache;

  tt_name = TOKYOTYRANT_DSN_PREFIX"s20:1981";
  db_cache = DatabaseCacheFactory::instance().getDbCache(tt_name);
  if (db_cache->Initialize() == false) {
    MCE_FATAL("Db" << tt_name << " initialize failed");
    goto __error;
  }    
  db_handler_map_[3] = db_cache;
  return true;

__error:
  return false;
}

DbHandler::~DbHandler() {
  for (int i=0; i<cluster_; ++i) {
    if (db_handler_map_[i])
      db_handler_map_[i]->Close();
  }
}
boost::shared_ptr<IDbCache> DbHandler::getDbHandler(int user_id) {
  return db_handler_map_[user_id % cluster_];
}

BatchViewCountResultHandlerI::BatchViewCountResultHandlerI(set<int>& valid_ids_set, map<int, int>& id_2_view_count) :
  valid_ids_set_(valid_ids_set), id_2_view_count_(id_2_view_count){
  }

bool BatchViewCountResultHandlerI::handle(mysqlpp::Row& row) const {
  int user_id = (int) row["id"];
  if (valid_ids_set_.find(user_id) != valid_ids_set_.end()) {
    int view_count = (int) row["viewcount"];
    id_2_view_count_[user_id] = view_count;
    //boost::shared_ptr<IDbCache> db_handler = DbHandler::instance().getDbHandler(user_id);    
    //if (db_handler) {
    //  SearchCacheDataPtr temp_obj = new SearchCacheData;
    //  if (db_handler->Query(user_id, temp_obj.get())) {
    //    //如果viewcount字段值没有变化，就不再需要再写了
    //      std::cout << "[TMEPLE] view_count in sql is " << view_count << ", view_count in tt is " << temp_obj->_viewcount << std::endl;
    //    if (abs(temp_obj->_viewcount - view_count) > 10000) {
    //      std::cout << "[TMEPLE] view_count in sql is " << view_count << ", view_count in tt is " << temp_obj->_viewcount << std::endl;
    //      temp_obj->_viewcount = view_count;
    //      db_handler->Insert(user_id, *temp_obj.get());
    //      return true;
    //    }
    //  }
    //} 
  }

  return true;
}

BasicStatusResultHandler2I::BasicStatusResultHandler2I(Ice::Int& max_id, set<int>& valid_ids_set) :
  max_id_(max_id), valid_ids_set_(valid_ids_set) {
  }

bool BasicStatusResultHandler2I::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  max_id_ = id > max_id_ ? id : max_id_;
  int status = (int) row["status"];
  if (status < 3) {
    valid_ids_set_.insert(id);
  }

  return true;
}   


static void Usage(const char* cmd) {
  fprintf(stderr, "%s <>\r\n", cmd);
}

int main(int argc, char*argv[]) {
  if (1 != argc) {
    Usage(argv[0]);
    return -1;
  }

  MyUtil::init_logger("test.log", "./test.log", "INFO");
  //初始化TT连接
  DbHandler::instance().Initialize();
  //初始化sql连接池
  ConnectionPoolManager::instance().initialize();

  int begin_id=0;
  int max_id=0;

  int idx=0;
  do {
    begin_id = max_id;
    set<int> valid_ids_set;
    map<int, int> id_2_view_count;
    map<int, vector<long> > mod_2_ids;
    {
      Statement status_sql;
      status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
        << begin_id << " LIMIT " << UPDATE_VIEW_COUNT_BATCH_SIZE;
      BasicStatusResultHandler2I status_handler(max_id, valid_ids_set);
      QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
    }
    {
      Statement sql;
      sql << "SELECT " << USER_VIEWCOUNT_FIELD << " FROM view_count WHERE id > "
        << begin_id << " AND id <= " << max_id
        <<" LIMIT " << UPDATE_VIEW_COUNT_BATCH_SIZE;
      BatchViewCountResultHandlerI view_count_handler(valid_ids_set, id_2_view_count);
      QueryRunner(DB_VIEW_COUNT, CDbRServer).query(sql, view_count_handler);
    }

    for (map<int, int>::const_iterator iter=id_2_view_count.begin(); iter!=id_2_view_count.end(); ++iter) {
      mod_2_ids[iter->first % kCluster].push_back(iter->first);
    }

    for (int i=0; i<kCluster; ++i) {
      vector<SearchCacheDataPtr> read_data_ptr_vector;
      //vector<SearchCacheData> write_data_vector;
      boost::shared_ptr<IDbCache> db_handler = DbHandler::instance().getDbHandler(i);    
      if (db_handler) {
#ifdef DEBUG 
        for (vector<long>::const_iterator iter=mod_2_ids[i].begin(); iter!=mod_2_ids[i].end(); ++iter) {
          cout << *iter << endl;
        }
#endif
        db_handler->Query(mod_2_ids[i], &read_data_ptr_vector);

#ifdef DEBUG 
        for (int i=0; i<read_data_ptr_vector.size(); ++i) {
          cout << " after Query: uid is " << (read_data_ptr_vector.at(i))->_id << endl;;
        }
#endif
        for (vector<SearchCacheDataPtr>::iterator iter=read_data_ptr_vector.begin(); iter!=read_data_ptr_vector.end(); ++iter) {
          int user_id = (*iter)->_id;
          //std::cout << "[TMEPLE] " << user_id  << ": view_count in sql is " << id_2_view_count[user_id] << ", view_count in tt is " << (*iter)->_viewcount << std::endl;
          if (abs((*iter)->_viewcount-id_2_view_count[user_id]) > 10000) {
            (*iter)->_viewcount = id_2_view_count[user_id];
            db_handler->Insert(user_id, *(*iter).get());
            //write_data_vector.push_back(*(*iter).get());
          }
        }

        //if (db_handler->Insert(write_data_vector)) {
          //cout << "ERROR while db_handler->Insert" <<endl;
        //}
      }
    }

    if (0 == ++idx % 100) {
      MCE_INFO("UpdateViewCount has processed "<<max_id<<" user ids in "<< idx << " cycles");
    }
  } while (begin_id < max_id);
  MCE_INFO("UpdateViewCountOnceTask done");

  return 0;
}
