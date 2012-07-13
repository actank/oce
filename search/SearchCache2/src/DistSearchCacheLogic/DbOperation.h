/* * ===================================================================================== *
 *       Filename:  DbOperation.h
 *
 *    Description:  Basic Operation of reading db
 *
 *        Version:  1.0
 *        Created:  2010年12月04日 17时13分17秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __DBOPERATION_H__
#define __DBOPERATION_H__

#include <IceUtil/IceUtil.h>
#include "SearchCache.h"
#include "DistSearchCache.pb.h"
#include <sys/time.h>
#include <set>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <QueryRunner.h>
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "search/SearchCache2/src//Util/IDbCache.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"


namespace xce {
  namespace searchcache {
    namespace dboperation  {
      using namespace com::xiaonei::xce;
      using namespace MyUtil;
      using namespace mysqlpp;
      using namespace std;
      using namespace oce::searchcache;
      
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

      const string FIELD_PASSPORT_STATUS = "user_passport:STATUS";
      const string FIELD_PASSPORT_SAFESTATUS = "user_passport:SAFESTATUS";
      const string FIELD_USER_NAME = "user_names:NAME";
      const string FIELD_BORN_GENDER = "user_born:GENDER";
      const string FIELD_BORN_ASTROLOGY = "user_born:ASTROLOGY";
      const string FIELD_BORN_HOMECITY = "user_born:HOME_CITY";//
      const string FIELD_BORN_HOMEPROVINCE = "user_born:HOME_PROVINCE";//
      const string FIELD_BORN_BIRTH_DAY = "user_born:BIRTH_DAY";//
      const string FIELD_BORN_BIRTH_YEAR = "user_born:BIRTH_YEAR";//
      const string FIELD_BORN_BIRTH_MONTH = "user_born:BIRTH_MONTH";//
      const string FIELD_URL_TINYURL = "user_url:TINYURL";
      const string FIELD_URL_HEADURL = "user_url:HEADURL";
      const string FIELD_USER_STATE = "user_state:STATE";
      const string FIELD_USER_STAGE = "user_stage:STAGE";
      const string FIELD_USER_STAR = "user_state:STAR";
      const string FIELD_BROWSE_CONFIG = "user_config:BROWSE_CONFIG";
      const string FIELD_STATUS_CONFIG = "user_config:STATUS_CONFIG";
      const string FIELD_BASIC_CONFIG = "user_config:BASIC_CONFIG";
      const string FIELD_PROFILE_PRIVACY = "user_config:PROFILE_PRIVACY";
      const string FIELD_USER_NICKNAME = "user_names:NICKNAME";
      const string FIELD_USER_LASTLOGINTIME = "user_time:LASTLOGINTIME";
      const string FIELD_FOND_INTEREST = "user_fond:INTEREST";
      const string FIELD_FOND_MUSIC = "user_fond:MUSIC";
      const string FIELD_FOND_MOVIE = "user_fond:MOVIE";
      const string FIELD_FOND_GAME = "user_fond:GAME";
      const string FIELD_FOND_COMIC = "user_fond:COMIC";
      const string FIELD_FOND_SPORT = "user_fond:SPORT";
      const string FIELD_FOND_BOOK = "user_fond:BOOK";
      const string FIELD_FOND_SOCIETY = "user_fond:SOCIETY";
      const string FIELD_RIGHT_AUTH = "user_right:AUTH";
      const string FIELD_VIEWCOUNT = "view_count:VIEWCOUNT";

      const string FIELD_PAGE = " id, name, headUrl, tinyUrl, fans_count, sub_type, p_type ";

      const string USER_NAME_FIELD = " id, name ";
      const string USER_STATUS_FIELD = " id, status, safe_status ";
      const string USER_STAGE_FIELD = " id, stage ";
      const string USER_BORN_FIELD = " id, gender ";
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

      const string USER_NAME_FIELD_TT = " id, nickname, name ";
      const string USER_BORN_FIELD_TT = " id, birth_day,birth_month,birth_year,astrology,gender,home_province,home_city ";
      const string USER_STATE_FIELD_TT = " id, state, star ";
      const string USER_FOND_FIELD_TT = " user_id, type, content ";
      const string USER_TIME_FIELD_TT = " id, lastlogintime ";//暂时未用
      const string USER_VIEWCOUNT_FIELD_TT = " id, viewcount ";//暂时未用
      const string USER_AUTH_FIELD_TT = " id, auth ";//add
      const string USER_LAST_REGISTRY = " user_id, operation ";//更新数据调用zoie

      const int ZOIE_ADD = 1;
      const int ZOIE_DELETE = 2;
      const int STATUS_LIMIT = 3;
    //***************************************************************************
    class SearchCacheMemData : public oce::searchcache::DistSearchCacheData, public Ice::Object {
      public:
        SearchCacheMemData() : _cas(0), in_tables_(0) {
        }

        std::string splitProfilePrivacy(std::string profile_privacy);
        void setElementarySchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
        void setJuniorSchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
        void setHighSchoolPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
        void setCollegePropertiesAccordingVector(std::vector<mysqlpp::Row>&);
        void setUniversityPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
        void setWorkplacePropertiesAccordingVector(std::vector<mysqlpp::Row>&);
        void setRegionPropertiesAccordingVector(std::vector<mysqlpp::Row>&);
        void setNetworkPropertiesAccordingVector(std::vector<mysqlpp::Row>&);

        void setUserPassportProperties(mysqlpp::Row& row);
        void setConfigProperties(mysqlpp::Row& row);
        void setUserUrlProperties(mysqlpp::Row& row);
        void setUserStateProperties(mysqlpp::Row& row);
        void setUserStatusProperties(mysqlpp::Row& row);
        void setUserStageProperties(mysqlpp::Row& row);
        void setUserNameProperties(mysqlpp::Row& row);
        void setUserBornProperties(mysqlpp::Row& row);
        void setUserAuthProperties(mysqlpp::Row& row);
        IceUtil::RWRecMutex _dataMutex;

        uint64_t _cas;
      private:

        int in_tables_;
    };
   
    typedef IceUtil::Handle<SearchCacheMemData> SearchCacheMemDataPtr;

    class SearchCacheMemPageData : public oce::searchcache::DistSearchCachePageData, public Ice::Object {
      public:
        SearchCacheMemPageData() : _cas(0), in_tables_(0) {
        }

        void setPageDataProperties(mysqlpp::Row& row);

        IceUtil::RWRecMutex _dataMutex;

        uint64_t _cas;
      private:

        int in_tables_;
    };

    typedef IceUtil::Handle<SearchCacheMemPageData> SearchCacheMemPageDataPtr;

    //***************************************************************************
    class UpdateTask: public MyUtil::Singleton<UpdateTask>  {
     public:
       UpdateTask();
       void handle(std::queue<boost::shared_ptr<UpdateUnit> >& update_units_ptr_queue);
       bool RecordUpdateData(int id, int update_operation);
       void UpdatePCAccordingIds(MyUtil::LongSeq&);
       void UpdatePCAccordingIdAndTableName(UpdateUnitSeq&);
       void UpdatePCAccordingFieldMap(UpdateUnitSeq&);
       void UpdateSpecialTable(MyUtil::LongSeq& uids, Statement& sql, void (SearchCacheData::*pFuncDb)(vector<mysqlpp::Row>& rows_db), 
           void (SearchCacheMemData::*pFuncCache)(vector<mysqlpp::Row>& rows_cache));
       int String2Int(const std::string& strInt);
       Statement generateSql(const string& idlist, const string& tableName, const string& resultFields, const string& condition);
       void generateCondition(const MyUtil::LongSeq& uids, string& idlist);
       void transformData(xce::searchcache::SearchCacheDataPtr original, xce::searchcache::dboperation::SearchCacheMemDataPtr destination); 
       bool UpdateSqlBatch(const vector<long>& uids);
       bool ReadDataFromTT(long userId, xce::searchcache::SearchCacheDataPtr obj,boost::shared_ptr<IDbCache> db_handler);
    };


     //***************************************************************************
    class BatchVectorResultHandlerI: public com::xiaonei::xce::ResultHandler {
     public:
       BatchVectorResultHandlerI(std::string idField, std::map<long, std::vector<mysqlpp::Row> >&);
       virtual bool handle(mysqlpp::Row& row) const;
     private:
       std::string _idField;
       std::map<long, std::vector<mysqlpp::Row> >& _id2RowsMap;
    };

    //***************************************************************************
    class BatchResultHandlerI: public com::xiaonei::xce::ResultHandler {
     public:
       BatchResultHandlerI(std::string id, void (SearchCacheData::*p)(mysqlpp::Row& row), map<long, SearchCacheDataPtr>& id2TT);
       virtual bool handle(mysqlpp::Row& row) const;
       void (SearchCacheData::*_pFunc)(mysqlpp::Row& row);
     private:
       std::string _id;
       map<long, SearchCacheDataPtr>& _id2TT;
    };

    //***************************************************************************
    class BatchBasicStatusResultHandlerI: public com::xiaonei::xce::ResultHandler {
     public:
       BatchBasicStatusResultHandlerI(Ice::Int& maxId, map<long, SearchCacheDataPtr>& id2TT);
       virtual bool handle(mysqlpp::Row& row) const;
     private:
       Ice::Int& _maxId;
       map<long, SearchCacheDataPtr>& _id2TT;
    };
    //***************************************************************************
    class BatchZoieUpdateHandlerI: public com::xiaonei::xce::ResultHandler {
      public:
        BatchZoieUpdateHandlerI(Ice::Int& id, Ice::Int& update_operation, bool& unique_id_operation);
        virtual bool handle(mysqlpp::Row& row) const;
      private:
        Ice::Int& _id;
        Ice::Int& _update_operation;
        bool& _unique_id_operation;
    };

    
    
  }
 }
}
#endif

