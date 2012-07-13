#include "check_last_login_time.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "LogWrapper.h"
#include "../../Util/DatabaseCacheFactory.h"
#include "ConnectionHolder.h"

using namespace xce;
using namespace xce::searchcache;
using namespace com::xiaonei::xce;
using std::string;
using std::cout;
using std::endl;

DbHandler* DbHandler::instance_ = 0;
IceUtil::Mutex DbHandler::mutex_;

typedef boost::shared_ptr<IDbCache> IDbCachePtr;

const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";
const string DB_SEARCH2_USERTIME = "search2_usertime";

const int UPDATE_BATCH_SIZE = 400;  
const string USER_STATUS_FIELD = " id, status "; 
const string USER_TIME_FIELD = " id, lastlogintime ";
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

BatchLastLoginTimeResultHandlerI::BatchLastLoginTimeResultHandlerI(set<int>& valid_ids_set, map<int, string>& id_2_last_login_time) :
  valid_ids_set_(valid_ids_set), id_2_last_login_time_(id_2_last_login_time){
  }

bool BatchLastLoginTimeResultHandlerI::handle(mysqlpp::Row& row) const {
  int user_id = (int) row["id"];
  if (valid_ids_set_.find(user_id) != valid_ids_set_.end()) {
#ifndef NEWARCH
    id_2_last_login_time_[user_id] = row["lastlogintime"].get_string();
#else
    id_2_last_login_time_[user_id] = row["lastlogintime"].c_str();
#endif
    //boost::shared_ptr<IDbCache> db_handler = DbHandler::instance().getDbHandler(user_id);    
    //if (db_handler) {
    //  SearchCacheDataPtr temp_obj = new SearchCacheData;
    //  if (db_handler->Query(user_id, temp_obj.get())) {
    //    //如果last_login_time字段值没有变化，就不再需要再写了
    //      std::cout << "[TMEPLE] last_login_time in sql is " << last_login_time << ", last_login_time in tt is " << temp_obj->_last_login_time << std::endl;
    //    if (abs(temp_obj->_last_login_time - last_login_time) > 10000) {
    //      std::cout << "[TMEPLE] last_login_time in sql is " << last_login_time << ", last_login_time in tt is " << temp_obj->_last_login_time << std::endl;
    //      temp_obj->_last_login_time = last_login_time;
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
  fprintf(stderr, "%s <begin_id processed_id_total_number changed_num cycles>\r\n", cmd);
}

int main(int argc, char*argv[]) {
  if (5 != argc) {
    Usage(argv[0]);
    return -1;
  }

  //MyUtil::init_logger("test.log", "./test.log", "INFO");
  //初始化TT连接
  DbHandler::instance().Initialize();
  //初始化sql连接池
  ConnectionPoolManager::instance().initialize();

  int begin_id=0;
  int max_id=atoi(argv[1]);
  int processed_id_total_number = atoi(argv[2]);
  int changed_num = atoi(argv[3]);
  int cycles = atoi(argv[4]);

  time_t callender_time;
  struct tm* broken_time;

  cout << "begin user id is " << max_id << ", processed_id_total_number is " << processed_id_total_number << ", changed num is " << changed_num <<", cycles is "<< cycles<< endl;
  do {
    begin_id = max_id;
    set<int> valid_ids_set;
    map<int, string> id_2_last_login_time;
    map<int, vector<long> > mod_2_ids;
    {
      Statement status_sql;
      status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
        << begin_id << " LIMIT " << UPDATE_BATCH_SIZE;
      BasicStatusResultHandler2I status_handler(max_id, valid_ids_set);
      QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
    }
    {
      Statement sql;
      sql << "SELECT " << USER_TIME_FIELD << " FROM user_time WHERE id > "
        << begin_id << " AND id <= " << max_id
        <<" LIMIT " << UPDATE_BATCH_SIZE;
      BatchLastLoginTimeResultHandlerI last_login_time_handler(valid_ids_set, id_2_last_login_time);
      QueryRunner(DB_SEARCH2_USERTIME, CDbRServer).query(sql, last_login_time_handler);
    }

    processed_id_total_number += valid_ids_set.size();
    for (map<int, string>::const_iterator iter=id_2_last_login_time.begin(); iter!=id_2_last_login_time.end(); ++iter) {
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
          //std::cout << "[TMEPLE] " << user_id  << ": last_login_time in sql is " << id_2_last_login_time[user_id] << ", last_login_time in tt is " << (*iter)->_lastlogintime << std::endl;
          if (0 != (((*iter)->_lastlogintime).compare(id_2_last_login_time[user_id]))) {
            ++changed_num;
            //std::cout << "[TMEPLE] changed " << user_id  << ": last_login_time in sql is " << id_2_last_login_time[user_id] << ", last_login_time in tt is " << (*iter)->_lastlogintime << std::endl;
            (*iter)->_lastlogintime = id_2_last_login_time[user_id];
            if (!db_handler->Insert(user_id, *(*iter).get())) {
              cout << "db_handler->Insert " << user_id << " Failed" <<endl;
            }
          }
        }
      }
    }

    if (0 == ++cycles % 10) {
      callender_time=time(NULL);
      broken_time = localtime(&callender_time);

      printf ( "[%02d:%02d:%02d] cycles=%d, total_id_nums=%d, max user_id=%d, changed_num=%d.\n", broken_time->tm_hour, broken_time->tm_min, broken_time->tm_sec, cycles, processed_id_total_number, max_id, changed_num);
      //cout << " cycles="<< ++cycles << ", total_id_nums=" << processed_id_total_number << ": max user_id="<<max_id<< ", changed_num="<< changed_num << "." << endl;
    }
  } while (begin_id < max_id);
  MCE_INFO("UpdateLastLoginTimeOnceTask done");

  return 0;
}
