#ifndef __TRIPOD_SEARCH_DBADAPTER_H__
#define __TRIPOD_SEARCH_DBADAPTER_H__

#include <IceUtil/IceUtil.h>
#include <sys/time.h>
#include <set>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <QueryRunner.h>
#include "TripodCacheData.h"
#include <boost/lexical_cast.hpp>

namespace xce {
  namespace search {
    namespace tripodsearchdbadapter {
      using namespace xce::search::tripodcachedata;

      const string DB_SEARCH2_USERBASIC = "search2_userbasic";
      const string DB_SEARCH2_USERBORN = "search2_userborn";
      const string DB_SEARCH2_USERFOND = "search2_userfond";
      const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";
      const string DB_SEARCH2_USERCONFIG = "search2_userconfig";
      const string DB_SEARCH2_USERURL = "user_url";
      const string DB_SEARCH2_USERTIME = "search2_usertime";
      const string DB_SEARCH2_USERNAME = "user_names";
      const string DB_SEARCH2_USERSTATE = "search2_user";
      const string DB_SEARCH2_USERSTAGE = "search2_userstage";
      const string DB_VIEW_COUNT = "user_view_count";
      const string DB_USER_RIGHT = "user_right_preloader";//add for privacy
      const string DB_SEARCH2_PAGE = "search2_page";
      const string DB_ZOIE_UPDATE = "search";//更新数据调用zoie

      const string TABLE_NETWORK_HISTORY = "network_history";
      const string TABLE_ELEMENTARY_SCHOOL = "elementary_school_info";
      const string TABLE_JUNIOR_SCHOOL = "junior_high_school_info";
      const string TABLE_HIGH_SCHOOL = "high_school_info";
      const string TABLE_COLLEGE_INFO = "college_info";
      const string TABLE_UNIVERSITY_INFO = "university_info";
      const string TABLE_WORKPLACE_INFO = "workplace_info";
      const string TABLE_REGION_INFO = "region_info";

      const string TABLE_USER_PASSPORT = "user_passport";
      const string TABLE_USER_NAME = "user_names";
      const string TABLE_USER_BORN = "user_born";
      const string TABLE_USER_URL = "user_url";
      const string TABLE_USER_STATE = "user_state";
      const string TABLE_USER_STAGE = "user_stage";
      const string TABLE_USER_CONFIG = "user_config";
      const string TABLE_USER_RIGHT = "user_right";//add for privacy
      const string TABLE_USER_FOND = "user_fond";
      const string TABLE_USER_VIEWCOUNT = "view_count";
      const string TABLE_LAST_REGISTRY = "last_registry_user";//更新数据调用zoie
      const string TABLE_PAGE = "page";

      const string FIELD_PAGE = " id, name, headUrl, tinyUrl, fans_count, sub_type, p_type ";

      const string USER_NAME_FIELD = " id, nickname, name ";
      const string USER_STATUS_FIELD = " id, status, safe_status ";
      const string USER_STAGE_FIELD = " id, stage ";
      const string USER_BORN_FIELD = " id, birth_day,birth_month,birth_year,astrology,gender,home_province,home_city ";
      const string USER_URL_FIELD = " id, tinyurl, headurl ";
      const string USER_CONFIG_FIELD = " id, browse_config, status_config, basic_config, profile_privacy ";  //后加一字段profile_privacy
      const string USER_STATE_FIELD = " id, state ";
      const string NETWORK_HISTORY_FIELD = " userid, network_id, network_name, stage "; 
      const string ELEMENTARY_SCHOOL_INFO_FIELD = " userid, elementary_school_id, elementary_school_name, elementary_school_year ";
      const string JUNIOR_HIGH_SCHOOL_INFO_FIELD = " userid, junior_high_school_id, junior_high_school_name, junior_high_school_year ";
      const string HIGH_SCHOOL_INFO_FIELD = " userid, high_school_id, high_school_name, enroll_year, h_class1, h_class2, h_class3 ";
      const string COLLEGE_INFO_FIELD = " userid, college_id, college_name, department,enroll_year ";
      const string UNIVERSITY_INFO_FIELD  = " userid, university_id, university_name, department, enroll_year, dorm ";
      const string WORKPLACE_INFO_FIELD = " userid, workplace_id, workplace_name ";
      const string REGION_INFO_FIELD = " userid, region_id, province_name, city_name ";

      const string USER_FOND_FIELD = " user_id, type, content ";
      const string USER_TIME_FIELD = " id, lastlogintime ";//暂时未用
      const string USER_VIEWCOUNT_FIELD = " id, viewcount ";//暂时未用
      const string USER_AUTH_FIELD = " id, auth ";//暂时未用
      const string USER_LAST_REGISTRY = " user_id, operation ";//更新数据调用zoie

      const int ZOIE_ADD = 1;
      const int ZOIE_DELETE = 2;
      const int STATUS_LIMIT = 4;

      class CreateKeysResultHandler : virtual public com::xiaonei::xce::ResultHandler {
        public:
          CreateKeysResultHandler(std::vector<std::string>& userKeys, std::string user) : _userKeys(userKeys), _user(user) {}
        protected:
          virtual bool handle(mysqlpp::Row& row) const {
            std::string userId;
            int status = (int)row["status"];
            if (status >= STATUS_LIMIT) { //cache中不限制page，建索引的时候判断
              return true;
            }
#ifndef NEWARCH
            userId = row[_user.c_str()].get_string();
#else
            userId = row[_user.c_str()].c_str();
#endif
            _userKeys.push_back(userId);
            return true;
          }
        private:
          std::vector<std::string>& _userKeys;
          std::string _user;
      };

      class BatchBasicStatusResultHandlerI : virtual public com::xiaonei::xce::ResultHandler {
        public:
          BatchBasicStatusResultHandlerI(std::map<std::string, TripodCacheDataPtr>& id2tripodData, std::string user, std::vector<std::string>& deleteKeys) : _id2tripodData(id2tripodData), _user(user), _deleteKeys(deleteKeys) {}
        protected:
          virtual bool handle(mysqlpp::Row& row) const {
            std::string userId;
            int status = (int)row["status"];
#ifndef NEWARCH
            userId = row[_user.c_str()].get_string();
#else       
            userId = row[_user.c_str()].c_str();
#endif      
            if (status >= STATUS_LIMIT) {
              MCE_DEBUG("BatchBasicStatusResultHandlerI::handle userId:" << userId << " status:" << status);
              _deleteKeys.push_back(userId);
              return true;
            }
            TripodCacheDataPtr obj =  new (std::nothrow) TripodCacheData;
            obj->setUserPassportProperties(row);
            _id2tripodData[userId] = obj;
            return true;
          }
        private:
          std::map<std::string, TripodCacheDataPtr>& _id2tripodData;
          std::string _user;
          std::vector<std::string>& _deleteKeys;
      };

      class BatchResultHandlerI : virtual public com::xiaonei::xce::ResultHandler {
        public:
          BatchResultHandlerI(void (TripodCacheData::*p)(mysqlpp::Row& row), std::map<std::string, TripodCacheDataPtr>& id2tripodData, std::string user) : _pFunc(p), _id2tripodData(id2tripodData), _user(user) {}
        protected:
          virtual bool handle(mysqlpp::Row& row) const {
            std::string userId;
#ifndef NEWARCH
            userId = row[_user.c_str()].get_string();
#else       
            userId = row[_user.c_str()].c_str();
#endif      
            if(NULL != _id2tripodData[userId].get())  {
              ((_id2tripodData[userId].get())->*_pFunc)(row);
            }
            return true;
          }
        private:
          void (TripodCacheData::*_pFunc)(mysqlpp::Row& row);
          std::map<std::string, TripodCacheDataPtr>& _id2tripodData;
          std::string _user;
      };

      class BatchVectorResultHandlerI : virtual public com::xiaonei::xce::ResultHandler {
        public:
          BatchVectorResultHandlerI(std::map<std::string, std::vector<mysqlpp::Row> >& id2RowsMap, std::string user) : _id2RowsMap(id2RowsMap), _user(user) {}
        protected:
          virtual bool handle(mysqlpp::Row& row) const {
            std::string userId;
#ifndef NEWARCH
            userId = row[_user.c_str()].get_string();
#else       
            userId = row[_user.c_str()].c_str();
#endif      
            _id2RowsMap[userId].push_back(row);
            return true;
          }
        private:
          std::map<std::string, std::vector<mysqlpp::Row> >& _id2RowsMap;
          std::string _user;
      };
    }
  }
}
#endif

