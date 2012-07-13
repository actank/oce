#include "check_and_update_critical_info.h"
#define NDEBUG
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "LogWrapper.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"
#include "ConnectionHolder.h"
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "OceCxxAdapter/src/SearchCacheAdapter.h"

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
const string DB_SEARCH2_USERNAME = "user_names";
const string DB_SEARCH2_USERURL = "user_url";
const string DB_SEARCH2_USERBASIC = "search2_userbasic";


const int UPDATE_BATCH_SIZE = 800;  

const string USER_STATUS_FIELD = " id, status "; 
const string USER_TIME_FIELD = " id, lastlogintime ";
const string USER_HEAD_URL_FIELD = "id, headurl ";
const string USER_NAME_FIELD = " id, name ";
const string NETWORK_HISTORY_FIELD = " userid, network_id, network_name, stage ";



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
  }

  return true;
}

BatchNetworkHistoryResultHandlerI::BatchNetworkHistoryResultHandlerI(set<int>& valid_ids_set, map<int, vector<Network> >& id_2_network_history) :
  valid_ids_set_(valid_ids_set), id_2_network_history_(id_2_network_history){
  }

bool BatchNetworkHistoryResultHandlerI::handle(mysqlpp::Row& row) const {
  int user_id = (int) row["userid"];

  //cout << "in BatchNetworkHistoryResultHandlerI user_id= " << user_id << endl;
  if (valid_ids_set_.find(user_id) != valid_ids_set_.end()) {

    const char* network_id = "network_id";                                              
    const char* stage = "stage";                                                        
    const char* network_name = "network_name";                                          

    Network network;                                                                      
    network.id = (int) row[network_id];                                                   
    network.stage = (int) row[stage];                                                     
#ifndef NEWARCH                                                                         
    network.name = row[network_name].get_string();                                        
#else                                                                                   
    network.name = row[network_name].c_str();                                             
#endif                                         
    id_2_network_history_[user_id].push_back(network);
    //cout << "user_id= " << user_id << ", network_id=" << network.id <<", stage=" << network.stage << ", name=" << network.name << endl;
  }

  return true;
}


BatchHeadUrlResultHandlerI::BatchHeadUrlResultHandlerI(set<int>& valid_ids_set, map<int, string>& id_2_head_url) :
  valid_ids_set_(valid_ids_set), id_2_head_url_(id_2_head_url){
  }

bool BatchHeadUrlResultHandlerI::handle(mysqlpp::Row& row) const {
  int user_id = (int) row["id"];
  if (valid_ids_set_.find(user_id) != valid_ids_set_.end()) {
#ifndef NEWARCH
    id_2_head_url_[user_id] = row["headurl"].get_string();
#else
    id_2_head_url_[user_id] = row["headurl"].c_str();
#endif
  }

  return true;
}

BatchUserNameResultHandlerI::BatchUserNameResultHandlerI(set<int>& valid_ids_set, map<int, string>& id_2_name) :
  valid_ids_set_(valid_ids_set), id_2_name_(id_2_name){
  }

bool BatchUserNameResultHandlerI::handle(mysqlpp::Row& row) const {
  int user_id = (int) row["id"];
  if (valid_ids_set_.find(user_id) != valid_ids_set_.end()) {
#ifndef NEWARCH
    id_2_name_[user_id] = row["name"].get_string();
#else
    id_2_name_[user_id] = row["name"].c_str();
#endif
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
  MyUtil::init_logger("Mce", "oce_log", "INFO");
  //MyUtil::init_logger("Mce", "update_critical_info.log", "INFO");
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
    map<int, string> id_2_name;
    map<int, string> id_2_head_url;
    map<int, std::vector<Network> > id_2_network_history;

    map<int, vector<int> > mod_2_ids;
    //获取有效的用户id
    {
      Statement status_sql;
      status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
        << begin_id << " LIMIT " << UPDATE_BATCH_SIZE;
      BasicStatusResultHandler2I status_handler(max_id, valid_ids_set);
      QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
    }
    //cout << "valid_ids_set.size()=" << valid_ids_set.size() << endl;
    //assert(valid_ids_set.size() != 0);
    /////获取上次登录时间
    ///{
    ///  Statement sql;
    ///  sql << "SELECT " << USER_TIME_FIELD << " FROM user_time WHERE id > "
    ///    << begin_id << " AND id <= " << max_id
    ///    <<" LIMIT " << UPDATE_BATCH_SIZE;
    ///  BatchLastLoginTimeResultHandlerI last_login_time_handler(valid_ids_set, id_2_last_login_time);
    ///  QueryRunner(DB_SEARCH2_USERTIME, CDbRServer).query(sql, last_login_time_handler);
    ///}
    //获取用户姓名信息
    {
      Statement sql; 
      sql << "SELECT " << USER_NAME_FIELD << " FROM user_names WHERE id > "
        << begin_id << " AND id <= " << max_id;
        //<<" LIMIT " << UPDATE_BATCH_SIZE;
      BatchUserNameResultHandlerI handler(valid_ids_set, id_2_name);
      QueryRunner(DB_SEARCH2_USERNAME, CDbRServer).query(sql, handler);
    } 
    //assert(id_2_name.size() != 0);
    //cout << " id_2_name.size()=" << id_2_name.size() << endl;
    //获取用户的小头像url
    {
      Statement sql;
      sql << "SELECT " << USER_HEAD_URL_FIELD << " FROM user_url WHERE id > "
        << begin_id << " AND id <= " << max_id;
        //<<" LIMIT " << UPDATE_BATCH_SIZE;
      BatchHeadUrlResultHandlerI handler(valid_ids_set, id_2_head_url);
      QueryRunner(DB_SEARCH2_USERURL, CDbRServer).query(sql, handler);
    } 
    assert(id_2_head_url.size() != 0);
    //cout << "id_2_head_url.size()=" << id_2_head_url.size() << endl;

    //获取用户的网络
    {
      Statement sql;
      sql
        << "SELECT " << NETWORK_HISTORY_FIELD << " FROM network_history WHERE status <=0 AND userid > "
        << begin_id << " AND userid <= " << max_id;
        //<<" LIMIT " << UPDATE_BATCH_SIZE;
      BatchNetworkHistoryResultHandlerI handler(valid_ids_set, id_2_network_history);                            
      QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);                  
    }
    assert(id_2_network_history.size() != 0);
    //cout << "id_2_network_history.size()=" << id_2_network_history.size() << endl;

    processed_id_total_number += valid_ids_set.size();
    for (set<int>::const_iterator iter=valid_ids_set.begin(); iter!=valid_ids_set.end(); ++iter) {
      mod_2_ids[*iter % kCluster].push_back(*iter);
    }

    //改为多线程的形式，提高速度
    for (int i=0; i<kCluster; ++i) {
      vector<SearchCacheDataPtr> read_data_ptr_vector;
      //vector<SearchCacheData> write_data_vector;
      boost::shared_ptr<IDbCache> db_handler = DbHandler::instance().getDbHandler(i);    
      if (db_handler) {
#ifdef DEBUG 
        for (vector<int>::const_iterator iter=mod_2_ids[i].begin(); iter!=mod_2_ids[i].end(); ++iter) {
          cout << *iter << endl;
        }
#endif
        db_handler->Query(mod_2_ids[i], &read_data_ptr_vector);

#ifdef DEBUG 
        cout << "After Query, mod_2_ids[ " << i << "].size()=" << mod_2_ids[i].size() << ", read_data_ptr_vector.size()=" << read_data_ptr_vector.size() << endl;
        for (int i=0; i<read_data_ptr_vector.size(); ++i) {
          cout << " after Query: uid is " << (read_data_ptr_vector.at(i))->_id << endl;;
        }
#endif
        for (vector<SearchCacheDataPtr>::iterator iter=read_data_ptr_vector.begin(); iter!=read_data_ptr_vector.end(); ++iter) {
          int user_id = (*iter)->_id;
          bool changed_flag = false;
          //std::cout << "[TMEPLE] " << user_id  << ": last_login_time in sql is " << id_2_last_login_time[user_id] << ", last_login_time in tt is " << (*iter)->_lastlogintime << std::endl;

          //////比较last_login_time
          ////if (0 != (((*iter)->_lastlogintime).compare(id_2_last_login_time[user_id]))) {
          ////  changed_flag = true;
          ////  //std::cout << "[TMEPLE] changed " << user_id  << ": last_login_time in sql is " << id_2_last_login_time[user_id] << ", last_login_time in tt is " << (*iter)->_lastlogintime << std::endl;
          ////  (*iter)->_lastlogintime = id_2_last_login_time[user_id];
          ////}

          // 比较user name 并更新
          if (0 != ((*iter)->_name).compare(id_2_name[user_id])) {
            std::cout << user_id  << ": user_name in sql is " << id_2_name[user_id] << ", in tt is " << (*iter)->_name << std::endl;
            changed_flag = true;
            //(*iter)->_name = id_2_name[user_id];
            map<string, string> field_key_2_value;
            field_key_2_value["NAME"] = id_2_name[user_id];
            try {
              SearchCacheAdapter::instance().update(user_id, "user_names", field_key_2_value);
            } catch (IceUtil::Exception& e) {
                MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"user_names\", field_key_2_value) " << e.what());
              } catch (...) {
                MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"user_names\", field_key_2_value");
              }
          }

          // 比较 head url 并更新
          if (0 != ((*iter)->_headurl).compare(id_2_head_url[user_id])) {
            std::cout << user_id  << ": user_head_url in sql is " << id_2_head_url[user_id] << ", in tt is " << (*iter)->_headurl << std::endl;
            changed_flag = true;
            //(*iter)->_head_url = id_2_head_url[user_id];
            map<string, string> field_key_2_value;
            field_key_2_value["HEADURL"] = id_2_head_url[user_id];
            try {
              SearchCacheAdapter::instance().update(user_id, "user_url", field_key_2_value);
            } catch (IceUtil::Exception& e) {
              MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"user_url\", field_key_2_value) " << e.what());
            } catch (...) {
              MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"user_url\", field_key_2_value");
            }
          }

          // 比较 network
          {
            SearchCacheData& data = *(*iter);
            map<int, vector<Network> >::const_iterator iter = id_2_network_history.find(user_id);
            if (iter == id_2_network_history.end() && 0 != data._networks.size()) { 
              cout << "user_id=" << user_id << ", iter == id_2_network_history.end() && data._networks.size()=" << data._networks.size()<< endl;
              changed_flag = true;
              map<string, string> field_key_2_value;
              try {
              SearchCacheAdapter::instance().update(user_id, "network_history", field_key_2_value);
              } catch (IceUtil::Exception& e) {
                MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"network_history\", field_key_2_value) " << e.what());
              } catch (...) {
                MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"network_history\", field_key_2_value");
              }

            } else if (iter != id_2_network_history.end() && data._networks.size() == (iter->second).size() ) {
              for (int i=0; i<data._networks.size(); ++i) {
                if((data._networks[i].id != (iter->second)[i].id) || (data._networks[i].stage !=  (iter->second)[i].stage) 
                    || 0 != data._networks[i].name.compare(iter->second[i].name)) {

                  std::cout << user_id  << ": network_id sql is " <<  (iter->second)[i].id << ", in tt is " << data._networks[i].id<< std::endl;
                  std::cout << user_id  << ": network_stage sql is " <<  (iter->second)[i].stage << ", in tt is " << data._networks[i].stage<< std::endl;
                  std::cout << user_id  << ": network_name sql is " <<  (iter->second)[i].name << ", in tt is " << data._networks[i].name<< std::endl;
                  changed_flag = true;
                  map<string, string> field_key_2_value;
                  try {
                    SearchCacheAdapter::instance().update(user_id, "network_history", field_key_2_value);
                  } catch (IceUtil::Exception& e) {
                    MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"network_history\", field_key_2_value) " << e.what());
                  } catch (...) {
                    MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"network_history\", field_key_2_value");
                  }
                  break;
                }
              }
            } else if (iter != id_2_network_history.end() && data._networks.size() != (iter->second).size()) {
              //size 不同
              cout << "uid=" << user_id << ", tt.size()=" << data._networks.size() <<" sql.size()="<< (iter->second).size() << endl;
              changed_flag = true;
              map<string, string> field_key_2_value;
              try {
                SearchCacheAdapter::instance().update(user_id, "network_history", field_key_2_value);
              } catch (IceUtil::Exception& e) {
                MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"network_history\", field_key_2_value) " << e.what());
              } catch (...) {
                MCE_WARN("userid=" << user_id <<" Exception: SearchCacheAdapter::instance().update(user_id, \"network_history\", field_key_2_value");
              }
            } 
          } 

          //如果有一个关键字段不一致，就回写
          if (changed_flag) {
            ++changed_num;
            // if (!db_handler->Insert(user_id, *(*iter).get())) {
            //   cout << "db_handler->Insert " << user_id << " Failed" <<endl;
            // }
          }

        } // end for 
      } else {
        MCE_ERROR("DB hanlder failed!");
      }
    }

    if (0 == ++cycles % 100) {
      callender_time=time(NULL);
      broken_time = localtime(&callender_time);

      printf ( "[%02d:%02d:%02d] cycles=%d, total_id_nums=%d, max user_id=%d, changed_num=%d.\n", broken_time->tm_hour, broken_time->tm_min, broken_time->tm_sec, cycles, processed_id_total_number, max_id, changed_num);
      //sleep(1);
    }
  }while (begin_id < max_id);
  MCE_INFO("UpdateLastLoginTimeOnceTask done");

  return 0;
}
