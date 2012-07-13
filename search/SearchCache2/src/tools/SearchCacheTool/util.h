#ifndef _UTIL_H_
#define _UTIL_H_

#include <SearchCache.h>
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include <IceUtil/IceUtil.h>
#include "search/SearchCache2/src/Util/BinString.h"
#include "Util.h"
#include "Channel.h"

using namespace MyUtil;
using namespace std;
using namespace IceUtil;
using namespace xce::searchcache;
namespace xce  {
  namespace searchcache {
    namespace test {
const string DB_SEARCH2_USERBASIC = "search2_userbasic";
const string DB_SEARCH2_USERBORN = "search2_userborn";
const string DB_SEARCH2_USERFOND = "search2_userfond";

const string TABLE_USER_NETWORK_HISTORY = "network_history";

const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";

const string DB_SEARCH2_USERCONFIG = "search2_userconfig";
const string TABLE_USER_CONFIG = "user_config";
const string DB_SEARCH2_USERURL = "user_url";

const string DB_SEARCH2_USERTIME = "search2_usertime";

const string DB_SEARCH2_USERNAME = "user_names";//"search_user_names";//"user_names"
const string TABLE_USERS = "user_names";

const string DB_SEARCH2_USERSTATE = "search2_user";
const string TABLE_STATE = "user_state";

const string DB_SEARCH2_USERSTAGE = "search2_userstage";

const string DB_VIEW_COUNT = "user_view_count";

const string DB_USER_RIGHT = "user_right_preloader";

const string TABLE_RIGHT = "user_right";

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_FILL_BASIC = 2;
const int TASK_LEVEL_FILL_CONFIG = 3;
const int TASK_LEVEL_FILL_USERS = 8;

const int TASK_LEVEL_UPDATE_DBCACHE = 9;

const int FILL_BATCH_SIZE = 3000;
const int UPDATE_VIEW_COUNT_BATCH_SIZE = 300;
const int LIMIT = 300000000;
const int IDS_CHECK_SIZE = 1000;
const string USER_STATUS_FIELD = " id, status ";
const string USER_STAGE_FIELD = " id, stage ";
const string USER_BORN_FIELD = " id, birth_day,birth_month,birth_year,astrology,gender,home_province,home_city ";

const string USER_URL_FIELD = " id, tinyurl, headurl ";
const string USER_CONFIG_FIELD = " id, browse_config, status_config, basic_config, profile_privacy ";
const string USER_STATE_FIELD = " id, state, star ";
const string NETWORK_HISTORY_FIELD = " userid, network_id, network_name, stage ";
const string ELEMENTARY_SCHOOL_INFO_FIELD = " userid, elementary_school_id, elementary_school_name,elementary_school_year ";
const string JUNIOR_HIGH_SCHOOL_INFO_FIELD = " userid, junior_high_school_id, junior_high_school_name, junior_high_school_year ";
const string HIGH_SCHOOL_INFO_FIELD = " userid, high_school_id, high_school_name, enroll_year, h_class1, h_class2, h_class3 ";
const string COLLEGE_INFO_FIELD = " userid, college_id, college_name, department,enroll_year ";
const string UNIVERSITY_INFO_FIELD  = " userid, university_id, university_name, department, enroll_year, dorm ";
const string WORKPLACE_INFO_FIELD = " userid, workplace_id, workplace_name ";
const string REGION_INFO_FIELD = " userid, region_id, province_name, city_name ";
const string USER_FOND_FIELD = " user_id, type, content ";
const string USER_TIME_FIELD = " id, lastlogintime ";
const string USER_NAME_FIELD = " id, nickname, name ";
const string USER_VIEWCOUNT_FIELD = " id, viewcount ";
const string USER_AUTH_FIELD = " id, auth ";


void BinStr2ByteSeqMap(long userId, BinStringPtr pBinString, Int2ByteSeq& id2Map);
int get_rand();
SearchCacheManagerPrx getSearchManager(SearchCacheChannel& channel, string search_cache_adapter_name) ;

 class BatchResultHandlerI: public com::xiaonei::xce::ResultHandler {
 public:
   BatchResultHandlerI(std::string id,  void (xce::searchcache::SearchCacheData::*p)(mysqlpp::Row& row), SearchCacheData& db_data) : 
     _id(id), _pFunc(p), _db_data(db_data) {}

   virtual bool handle(mysqlpp::Row& row) const {
     (_db_data.*_pFunc)(row);
     return true;
   }

 private:
   std::string _id;
   void (xce::searchcache::SearchCacheData::*_pFunc)(mysqlpp::Row& row);
   SearchCacheData& _db_data;
};

class BatchVectorResultHandlerI: public com::xiaonei::xce::ResultHandler {
 public:
   BatchVectorResultHandlerI(std::string idField, std::vector<mysqlpp::Row> &rows):
     _idField(idField), _rows(rows) {}
   virtual bool handle(mysqlpp::Row& row) const {
     _rows.push_back(row);
     return true;
   }
 private:
   std::string _idField;
   std::vector<mysqlpp::Row>& _rows;
};

class BasicStatusResultHandlerI: public com::xiaonei::xce::ResultHandler {
  public:
    BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<long>& ids, std::map<long, int>& id2status);
    virtual bool handle(mysqlpp::Row& row) const;
  private:
    Ice::Int& beginId_;
    std::vector<long>& ids_;
    std::map<long, int>& id2status_;
};




}
}
}

#endif
