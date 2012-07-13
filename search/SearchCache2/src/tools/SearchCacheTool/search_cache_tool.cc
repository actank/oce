/** 
 * @file search_cache_tool.cpp
 * @brief 用于测试SearchCache的更新逻辑
 * @author zhigang.wu
 * @date 2010-01-09
 */
//TODO:
//1. 将常用show函数封装成一个类
//2.使测试输入数据可配置化

#include <IceUtil/IceUtil.h>
#include <SearchCache.h>
#include <SearchCacheAdapter.h>
#include <SearchLogicAdapter.h>
#include <DistSearchLogicAdapter.h>
#include <XceCacheAdapter.h>
#include "search/SearchCache2/src/SearchCacheProxy/CacheProxy.h"
#include "search/SearchCache2/src/Util/SearchMemCacheData.h"
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"
#include "search/SearchCache2/src/Util/IDbCache.h"
#include "AdapterI.h"
#include "TopicI.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>//后加
#include "util.h"
#include <sstream>
#include "show.h"
#include "LogWrapper.h"
#include "search/SearchCache2/src/DistSearchCacheLogic/DbOperation.h"
//#include "search/SearchCache2/src/DistSearchCacheLogic/DistSearchCacheAdapter.h"
#include "OceCxxAdapter/src/DistSearchCacheAdapter.h"



using namespace std;
const std::string UPDATE_SUFFIX="sc_date_";
const std::string DELETE_SUFFIX="delete_";

using namespace xce::searchcache;
//using namespace xce::dist::searchcache;
using namespace xce::searchcache::dboperation;
using namespace xce::dist::searchcacheadapter;
void Usage();
bool GetSearchMemCacheDataFromTT(int id);
bool GetUserInfoFromTT(int id, const string& ttserver, SearchCacheData& tt_data);
bool GetTTServer(int user_id, int argc, char *argv[], string& ttserver);
bool GetUserInfoFromDB(int user_id, SearchCacheData& db_data);
bool CompareTTAndDB(const SearchCacheData& tt_data, const SearchCacheData& mysql_data); 

int main(int argc, char *argv[]) {
  MyUtil::init_logger("Mce", "oce_log.log", "DEBUG");
  if (1 == argc || 0 == strcmp(argv[1], "--help")) {
    Usage();
  }  else if (0 == strcmp(argv[1], "show_user_info_in_mem_cache")){
    vector<long> ids;
    for (int i=2; i<argc; ++i) {
      stringstream ss;
      long user_id;
      ss<<argv[i];
      ss>>user_id;
      ids.push_back(user_id);
    }
    xce::dist::searchcacheadapter::DistSearchCacheAdapter::instance().initialize();
    map<long, xce::searchcache::dboperation::SearchCacheMemDataPtr> results = xce::dist::searchcacheadapter::DistSearchCacheAdapter::instance().getData(ids);
    ShowSearchCacheResult(results);
  } else if (0 == strcmp(argv[1], "get_search_mem_cache_data_from_tt")) {
    stringstream ss;
    int user_id;
    ss<<argv[2];
    ss>>user_id;
    GetSearchMemCacheDataFromTT(user_id);
  } else if (0 == strcmp(argv[1], "show_user_info_in_tt")) {
    stringstream ss;
    int user_id;
    ss<<argv[2];
    ss>>user_id;
    cout<<user_id<<endl;

    string ttserver ;
    GetTTServer(user_id, argc, argv, ttserver);

    SearchCacheData tt_data;
    if (GetUserInfoFromTT(user_id, ttserver, tt_data))
      ShowSearchCacheData(tt_data);
  } else if (0 == strcmp(argv[1], "compare_tt_and_mysql")) {
    com::xiaonei::xce::ConnectionPoolManager::instance().initialize();

    stringstream ss;
    int user_id;
    ss << argv[2];
    ss >> user_id;

    string ttserver ;
    GetTTServer(user_id, argc, argv, ttserver);

    SearchCacheData tt_data, db_data;
    if (GetUserInfoFromTT(user_id, ttserver, tt_data) && GetUserInfoFromDB(user_id, db_data)) {
      CompareTTAndDB(tt_data, db_data);
      cout<<"\t##################tt_data:"<<endl;
      ShowSearchCacheData(tt_data);
      cout<<"\t##################mysql_data:"<<endl;
      ShowSearchCacheData(db_data);
    }

  } else if (0 == strcmp(argv[1], "update")) {
    stringstream ss;
    int user_id;
    ss<<argv[2];
    ss>>user_id;
    UpdateUnitSeq updateUnits;
    {
      UpdateUnit updateUnit;
      updateUnit.id = user_id;
      updateUnit.table = "";
      updateUnits.push_back(updateUnit);
    }
    cout << "\t-->updateUnits.size:" << updateUnits.size() << endl;
    for (size_t i = 0; i < updateUnits.size(); i++) {
      cout << "\t-->updateUnits[" << i << "]:" << updateUnits[i].id << "; " << updateUnits[i].table << endl;
    }

    xce::dist::searchcacheadapter::DistSearchCacheAdapter::instance().initialize();
    DistSearchLogicAdapter::instance().doUpdate(updateUnits);
  } else if (0 == strcmp(argv[1], "update_field_map")) {
    stringstream ss;
    string table, field_key, field_value;
    long user_id;
    ss<<argv[2];
    ss>>user_id;
    table = (string) argv[3];
    field_key = (string) argv[4];
    field_value = (string) argv[5];
    map<string, string> field_map;
    field_map[(string)argv[4]]= (string)argv[5];
    std::cout<<"user_id : "<<user_id<<" table: "<<table<<" field_key: "<<field_key<<" field_value: "<<field_value<<std::endl;
    xce::dist::searchcacheadapter::DistSearchCacheAdapter::instance().initialize();
    DistSearchLogicAdapter::instance().update(user_id, table, field_map);
    sleep(5);
  } else if (0 == strcmp(argv[1], "update_batch_from_file")) {
    stringstream ss;
    int user_id;
    ss<<argv[2];
    ifstream id_list_file((ss.str()).c_str());
    string sline;

    map<string, string> field_map;

    xce::dist::searchcacheadapter::DistSearchCacheAdapter::instance().initialize();
    while (getline(id_list_file, sline)) {
      stringstream ss_line(sline);
      ss_line >> user_id;

      cout<<user_id<<endl;
      DistSearchLogicAdapter::instance().update(user_id, "", field_map);
    }
    sleep(5);
  } else if( 0 == strcmp(argv[1], "update_service"))  {
    if(argc !=5)
      Usage();
    stringstream ss;
    int service_mod = 0; 
    int index = 0;
    bool is_not = true;
    ss<<argv[2];
    ss>>service_mod;
    if(strcmp(argv[3], "0")==0)
      index = 0;
    else if(strcmp(argv[3], "1")==0)
      index = 1;
    if(strcmp(argv[4],"false")== 0)
      is_not=false;
    else if(strcmp(argv[4], "true") ==0)
      is_not=true;
    std::vector<SearchCacheManagerPrx> proxies_before, proxies_after;
    DistSearchLogicAdapter::instance().isValidTest(service_mod, proxies_before); 
    for(std::vector<SearchCacheManagerPrx>::iterator iter = proxies_before.begin(); iter != proxies_before.end(); iter++)  {//TODO  改成log
      std::cout<<*iter<<std::endl;
    }

    DistSearchLogicAdapter::instance().setValid(service_mod,index,is_not);

    DistSearchLogicAdapter::instance().isValidTest(service_mod, proxies_after);
    for(std::vector<SearchCacheManagerPrx>::iterator iter = proxies_after.begin(); iter != proxies_after.end(); iter++)  {//TODO  改成log
      std::cout<<*iter<<std::endl;
    }
  } else if(0 == strcmp(argv[1], "show_service"))  {
    if(argc != 3)
      Usage();
    stringstream ss;
    int service_mod;
    ss<<argv[2];
    ss>>service_mod;
    std::vector<SearchCacheManagerPrx> proxies;
    DistSearchLogicAdapter::instance().isValidTest(service_mod, proxies);
    for(std::vector<SearchCacheManagerPrx>::iterator iter = proxies.begin(); iter != proxies.end(); iter++)  {//TODO  改成log
      std::cout<<*iter<<std::endl;
    }
  }  
  return 1;
}

void Usage() {
  cout << "用法: " << endl; 
  cout << "\tcompare_tt_and_mysql user_id (比较tt和mysql中的数据)" << endl; 
  cout << "\tshow_user_info_in_mem_cache user_id search_cache_adapter_name(例如SearchCache08) (展示缓存中的用户信息)" << endl;
  cout << "\tshow_user_info_in_tt user_id (ttserver) (展示tt中用户的信息)" << endl; 
  cout << "\tget_search_mem_cache_data_from_tt user_id " << endl;
  cout << "\tupdate [user_id] ---reload [user_id] information from db to searchcache" << endl;
  cout << "\tupdate_field_map [user_id] [table_name] [field_key] [field_value] ---reload [field_key:field_value] in [table_name] of [user_id]" << endl;
  cout << "\tupdate_batch_from_file [id_list_file]  ---load ids from [id_list_file] to DistSearchCache" << endl;
  cout << "\tshow_service service_mod(显示mod=service_mod的所有可用服务代理)" << endl;
  cout << "\tupdate_service service_mod index_proxy(停止哪个SearchPersistentCache对外提供服务) true(false)>" << endl;
}

bool GetUserInfoFromTT(int id, const string& ttserver, SearchCacheData& tt_data) {
  boost::shared_ptr<IDbCache> db_cache; 
  cout<<"id is:"<<id<<endl;
  db_cache = DatabaseCacheFactory::instance().getDbCache(ttserver);
  cout << " using " << ttserver << endl;
  if (db_cache->Initialize() == false) {
    std::cout<<"Db initialize failed"<<std::endl;
    return false;
  }

  bool result = true;
  if (!db_cache->Query(id, &tt_data)) {
    std::cout << "error userid = " << id << std::endl;
    result = false;
  }

  db_cache->Close();

  return result;
}

bool GetSearchMemCacheDataFromTT(int id) {
  boost::shared_ptr<IDbCache> db_cache; 
  if (0 == id % 4) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.18.72:1981");
    cout << " mod=0, using tokyotryant://10.3.18.72:1981(s21)" << endl;
  } else if (1 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.18.73:1981");
    cout << " mod=1, using tokyotryant://10.3.18.73:1981(s22)" << endl;
  } else if (2 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.18.74:1981");
    cout << " mod=2, using tokyotryant://10.3.18.74:1981(s23)" << endl;
  } else if (3 == id % 4 ) {
    db_cache = DatabaseCacheFactory::instance().getDbCache("tokyotryant://10.3.21.15:1981");
    cout << " mod=3, using tokyotryant://10.3.21.15:1981(s33)" << endl;
  }
  if (db_cache->Initialize() == false) {
    std::cout<<"Db initialize failed"<<std::endl;
    return false;
  }

  SearchMemCacheData s;
  if (db_cache->Query(id, &s)) {
    std::cout << "GetSearchMemCacheDataFromTT succeed! id=" << id << std::endl;
    ShowSearchMemCacheData(s);
  } else {
    std::cout << "error userid = " << id << std::endl;
    return false;
  }

  db_cache->Close();

  return true;
}

bool GetTTServer(int user_id, int argc, char* argv[], string& ttserver) {
  ttserver = "tokyotryant://";
  if (4 == argc) {
    ttserver += argv[3];
  } else if (0 == user_id % 4) {
    ttserver += "s21:1981";
    //ttserver += "10.22.206.32:1981";
  } else if (1 == user_id % 4) {
    ttserver += "s22:1981";
    //ttserver += "10.22.206.154:1981";
  } else if (2 == user_id % 4) {
    ttserver += "s23:1981";
    //ttserver += "10.22.206.155:1981";
  } else if (3 == user_id % 4) {
    ttserver += "s33:1981";
    //ttserver += "10.22.206.155:1982";
  }

  return true;
}


bool GetUserInfoFromDB(int user_id, SearchCacheData& db_data) {
  db_data._id = user_id;
  {
    Statement sql;
    sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setUserStatusProperties, db_data);
    QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << USER_BORN_FIELD << " FROM user_born WHERE id = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setUserBornProperties, db_data);
    QueryRunner(DB_SEARCH2_USERBORN, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << USER_URL_FIELD << " FROM user_url WHERE id = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setUserUrlProperties, db_data);
    QueryRunner(DB_SEARCH2_USERURL, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << xce::searchcache::test::USER_STATE_FIELD << " FROM user_state WHERE id = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setUserStateProperties, db_data);
    QueryRunner(DB_SEARCH2_USERSTATE, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << xce::searchcache::test::USER_CONFIG_FIELD << " FROM user_config WHERE id = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setConfigProperties, db_data);
    QueryRunner(DB_SEARCH2_USERCONFIG, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << xce::searchcache::test::USER_AUTH_FIELD << " FROM user_right WHERE id = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setUserAuthProperties, db_data);
    QueryRunner(DB_USER_RIGHT, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << xce::searchcache::test::USER_VIEWCOUNT_FIELD << " FROM view_count WHERE id  = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setViewCountProperties, db_data);
    QueryRunner(DB_VIEW_COUNT, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << xce::searchcache::test::USER_NAME_FIELD << " FROM user_names WHERE id  = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setUserNameProperties, db_data);
    QueryRunner(DB_SEARCH2_USERNAME, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << xce::searchcache::test::USER_STAGE_FIELD<< " FROM user_stage WHERE id  = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setUserStageProperties, db_data);
    QueryRunner(DB_SEARCH2_USERSTAGE, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql << "SELECT " << NETWORK_HISTORY_FIELD << " FROM network_history WHERE userid = " << user_id 
      << " AND status <=0 order by network_id";

    vector<mysqlpp::Row>  rows;
    xce::searchcache::test::BatchVectorResultHandlerI handler("userid", rows);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
    db_data.setNetworkPropertiesAccordingVector(rows);
  }
  {
    Statement sql;
    sql << "SELECT " << ELEMENTARY_SCHOOL_INFO_FIELD << " FROM elementary_school_info WHERE userid  = " << user_id ;
    vector<mysqlpp::Row>  rows;
    xce::searchcache::test::BatchVectorResultHandlerI handler("userid", rows);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

    db_data.setElementarySchoolPropertiesAccordingVector(rows);
  }
  {
    Statement sql;
    sql << "SELECT " << JUNIOR_HIGH_SCHOOL_INFO_FIELD << " FROM junior_high_school_info WHERE userid  = " << user_id;
    vector<mysqlpp::Row>  rows;
    xce::searchcache::test::BatchVectorResultHandlerI handler("userid", rows);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
    db_data.setJuniorSchoolPropertiesAccordingVector(rows);
  }

  {
    Statement sql;
    sql << "SELECT " << HIGH_SCHOOL_INFO_FIELD << " FROM high_school_info WHERE userid  = " << user_id;
    vector<mysqlpp::Row>  rows;
    xce::searchcache::test::BatchVectorResultHandlerI handler("userid", rows);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
    db_data.setHighSchoolPropertiesAccordingVector(rows);
  }
  {
    Statement sql;
    sql << "SELECT " << COLLEGE_INFO_FIELD << " FROM college_info WHERE userid = " << user_id;
    vector<mysqlpp::Row>  rows;
    xce::searchcache::test::BatchVectorResultHandlerI handler("userid", rows);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
    db_data.setCollegePropertiesAccordingVector(rows);
  }
  {
    Statement sql;
    sql << "SELECT " << UNIVERSITY_INFO_FIELD << " FROM university_info WHERE userid = " << user_id;
    vector<mysqlpp::Row>  rows;
    xce::searchcache::test::BatchVectorResultHandlerI handler("userid", rows);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
    db_data.setUniversityPropertiesAccordingVector(rows);
  }
  {
    Statement sql;
    sql << "SELECT " << WORKPLACE_INFO_FIELD << " FROM workplace_info WHERE userid  = " << user_id;
    vector<mysqlpp::Row>  rows;
    xce::searchcache::test::BatchVectorResultHandlerI handler("userid", rows);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
    db_data.setWorkplacePropertiesAccordingVector(rows);
  }
  {
    Statement sql;
    sql << "SELECT " << REGION_INFO_FIELD << " FROM region_info WHERE userid = " << user_id;
    vector<mysqlpp::Row>  rows;
    xce::searchcache::test::BatchVectorResultHandlerI handler("userid", rows);
    QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
    db_data.setRegionPropertiesAccordingVector(rows);
  }
  {
    int i = user_id % 100;
    ostringstream table_name;
    table_name << "user_fond_" << i;
    Statement sql;
    sql  << "SELECT " << xce::searchcache::test::USER_FOND_FIELD << " FROM "<<table_name.str()<<" WHERE user_id  = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("user_id", &SearchCacheData::setUserFondProperties, db_data);
    QueryRunner(DB_SEARCH2_USERFOND, CDbRServer).query(sql, handler);
  }
  {
    Statement sql;
    sql  << "SELECT " << xce::searchcache::test::USER_TIME_FIELD << " FROM user_time WHERE id  = " << user_id;
    xce::searchcache::test::BatchResultHandlerI handler("id", &SearchCacheData::setLastlogintimeProperties, db_data);
    QueryRunner(DB_SEARCH2_USERTIME, CDbRServer).query(sql, handler);
  }

  return true;
}


bool CompareTTAndDB(const SearchCacheData& tt_data, const SearchCacheData& mysql_data) {
  if(tt_data._id != mysql_data._id) {
    cout<<"id不一致"<<endl;
    goto not_equal;
  }
  if(0 != tt_data._name.compare(mysql_data._name)) {
    cout<<"name不一致"<<endl;
    goto not_equal;
  }
  if(0 != tt_data._gender.compare(mysql_data._gender)) {
    cout<<"性别不一致"<<endl;
    goto not_equal;
  }
  if(tt_data._star != mysql_data._star){
    cout<<"star不一致"<<endl;
    goto not_equal;
  }
  if(tt_data._status != mysql_data._status) {
    cout<<"status不一致"<<endl;
    goto not_equal;
  }
  if(0 != tt_data._tinyurl.compare(mysql_data._tinyurl)){
    cout<<"tinyurl不一致"<<endl;
    goto not_equal;
  }
  if(0 != tt_data._headurl.compare(mysql_data._headurl)){
    cout<<"headurl不一致"<<endl;
    goto not_equal;
  }
  if(tt_data._browse_config != mysql_data._browse_config){
    cout<<"browse_config不一致"<<endl;
    goto not_equal;
  }
  if(tt_data._status_config != mysql_data._status_config){
    cout<<"status_config不一致"<<endl;
    goto not_equal;
  }
  //TODO: searchcache中暂时没有home_city和home_province
  /* if(0 != tt_data._home_city.compare(mysql_data._home_city)){
    cout<<"home_city不一致"<<endl;
    goto not_equal;
  }
  if(tt_data._home_province.compare(mysql_data._home_province)){
    cout<<"home_province不一致"<<endl;
    goto not_equal;
  }*/

  if (tt_data._networks.size() != mysql_data._networks.size()){
    cout<<"networks.size不一致 : tt_data._networks.size=" << tt_data._networks.size() << ", mysql_data=" << mysql_data._networks.size()<<endl;
    goto not_equal;
  }

  for (size_t i=0; i< tt_data._networks.size(); ++i) { 
     if(tt_data._networks[i].id != mysql_data._networks[i].id){
       cout<<"networks.id不一致: i=" << i <<", tt_data=" << tt_data._networks[i].id << ", mysql_data="<<mysql_data._networks[i].id<<endl;
       goto not_equal;
     }
     if(tt_data._networks[i].stage != mysql_data._networks[i].stage){
       cout<<"networks.stage不一致"<<endl;
       goto not_equal;
     }
     if(0 != tt_data._networks[i].name.compare(mysql_data._networks[i].name)){
       cout<<"networks.name不一致"<<endl;
       goto not_equal;
     }
  } 

  if (tt_data._regions.size() != mysql_data._regions.size()) {
    cout<<"regions.size不一致"<<endl;
    goto not_equal;
  }
  for (size_t i=0; i< tt_data._regions.size(); ++i) { 
     if(tt_data._regions[i].id != mysql_data._regions[i].id){
       cout<<"regions.id不一致"<<endl;
       goto not_equal;
     }
     if(0 != tt_data._regions[i].city.compare(mysql_data._regions[i].city)){
       cout<<"regions.city不一致"<<endl;
       goto not_equal;
     }
     if(0 != tt_data._regions[i].province.compare(mysql_data._regions[i].province)){
       cout<<"regions.province不一致"<<endl;
       goto not_equal;
     }
  } 

  if (tt_data._workplaces.size() != mysql_data._workplaces.size()){
    cout<<"workplaces.size不一致"<<endl;
       goto not_equal;
  }
  for (size_t i=0; i<tt_data._workplaces.size(); ++i) { 
     if(tt_data._workplaces[i].id != mysql_data._workplaces[i].id){
       cout<<"workplaces.id不一致"<<endl;
       goto not_equal;
     }
     if(0 != tt_data._workplaces[i].name.compare(mysql_data._workplaces[i].name)){
       cout<<"workplaces.name不一致"<<endl;
       goto not_equal;
     }
  } 

  if (tt_data._universities.size() != mysql_data._universities.size()){
    cout<<"universities.size不一致"<<endl;
       goto not_equal;
  }
  for (size_t i=0; i<tt_data._universities.size(); ++i) { 
     if(tt_data._universities[i].id != mysql_data._universities[i].id){
       cout<<"universities.id不一致"<<endl;
       goto not_equal;
     }
     if(tt_data._universities[i].year != mysql_data._universities[i].year){
       cout<<"universities.year不一致"<<endl;
       goto not_equal;
     }
     if(0 != tt_data._universities[i].name.compare(mysql_data._universities[i].name)){
       cout<<"universities.name不一致"<<endl;
       goto not_equal;
     }
     if(0 != tt_data._universities[i].department.compare(mysql_data._universities[i].department)){
       cout<<"universities.department不一致"<<endl;
       goto not_equal;
     }
     if(0 != tt_data._universities[i].dorm.compare(mysql_data._universities[i].dorm)){
       cout<<"universities.dorm不一致"<<endl;
       goto not_equal;
     }
  } 
  cout<<"&&&&&&&&&&&&&&&&&&&&&数据完全一致" << mysql_data._id<<endl;
  return true;

not_equal:
  cout<<"&&&&&&&&&&&&&&&&&&&&&数据不一致" << mysql_data._id<<endl;
  return false;
}
