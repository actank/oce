#ifndef __SEARCHPERSISTENTCACHEI_H__
#define __SEARCHPERSISTENTCACHEI_H__

#include <vector>
#include <queue>
#include <boost/shared_ptr.hpp>
#include "SearchCache.h"
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
#include "search/SearchCache2/src/Util/SearchCacheData.h"
#include "search/SearchCache2/src//Util/IDbCache.h"

namespace xce {
  namespace searchcache {
    const unsigned int kMaxUserIdNumEachProcess = 200;

    const string UPDATE_PREFIX = "sc_date_";
    const string DELETE_PREFIX = "delete_";
    const unsigned int IO_BUFFER_SIZE = 8192;

    const Ice::Int SEARCH_CACHE = 0;

    const std::string SEARCH_CACHE_LOGIC_MANAGER = "S";


    const string DB_SEARCH2_USERBASIC = "search2_userbasic";
    const string DB_SEARCH2_USERBORN = "search2_userborn";
    const string DB_SEARCH2_USERFOND = "search2_userfond";

    const string TABLE_USER_NETWORK_HISTORY = "network_history";

    const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";

    const string DB_SEARCH2_USERCONFIG = "search2_userconfig";
    const string TABLE_USER_CONFIG = "user_config";

    const string DB_SEARCH2_USERURL = "user_url";

    const string DB_SEARCH2_USERTIME = "search2_usertime";

    const string DB_SEARCH2_USERNAME = "user_names";
    const string TABLE_USERS = "user_names";

    const string DB_SEARCH2_USERSTATE = "search2_user";
    const string TABLE_STATE = "user_state";

    const string DB_SEARCH2_USERSTAGE = "search2_userstage";

    const string DB_VIEW_COUNT = "user_view_count";

    const string DB_USER_RIGHT = "user_right_preloader";//add for privacy
    const string TABLE_USER_RIGHT = "user_right";//add for privacy
    
    const int TASK_LEVEL_FILL = 1;
    const int TASK_LEVEL_FILL_BASIC = 2;
    const int TASK_LEVEL_FILL_CONFIG = 3;
    const int TASK_LEVEL_FILL_USERS = 8;

    const int TASK_LEVEL_UPDATE_DBCACHE = 9;

    const int FILL_BATCH_SIZE = 3000;
    const int UPDATE_VIEW_COUNT_BATCH_SIZE = 300;

    //const string USER_BASIC_FIELD = " id, star, selected, name, home_city,home_province, gender, stage, birth_day, birth_month, birth_year, astrology ";
    const string USER_STATUS_FIELD = " id, status ";
    const string USER_STAGE_FIELD = " id, stage ";
    const string USER_BORN_FIELD = " id, birth_day,birth_month,birth_year,astrology,gender,home_province,home_city ";

    const string USER_URL_FIELD = " id, tinyurl, headurl ";
    const string USER_CONFIG_FIELD = " id, browse_config, status_config, basic_config, profile_privacy ";  //后加一字段profile_privacy
    const string USER_STATE_FIELD = " id, state, star ";
    const string NETWORK_HISTORY_FIELD = " userid, network_id, network_name, stage "; 
    const string ELEMENTARY_SCHOOL_INFO_FIELD = " userid, elementary_school_id, elementary_school_name, elementary_school_year ";
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

    const string USER_AUTH_FIELD = " id, auth ";//add


    //***************************************************************************

    class MessageQueueThread;
    class SearchLogicManagerI: virtual public SearchCacheManager,
    public MyUtil::Singleton<SearchLogicManagerI> {
     public:
       SearchLogicManagerI();
       virtual void load(const ::MyUtil::IntSeq&, const Ice::Current&);
       virtual void update(const UpdateUnitSeq&, const Ice::Current&);
       virtual bool isValid(const Ice::Current& = Ice::Current());
       virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
       //哑接口，仅对MemCache有效
       bool puts(const Int2ByteSeq& id2Map, const Ice::Current&); 
       virtual SearchCacheResult getSearchCache(::Ice::Int, const Ice::Current&);

       virtual Int2SearchCacheResultMap getSearchCacheList(const ::MyUtil::IntSeq&,
           const Ice::Current&);
       virtual Int2ByteSeq getSearchCacheList2(const ::MyUtil::IntSeq&, 
           const ::Ice::Current&);
     


       bool removes(const MyUtil::LongSeq& userIds, const Ice::Current&);


       void bootstrap(int cluster, int mod, string dsn, int initialize);

       bool inline belongsMe(int id){
         return true;
         /*
            if (_cluster==0) return true;
            return (_mod == (id % _cluster));
            */
       }

       boost::shared_ptr<IDbCache> getDbHandler(int userId) {
         map<int, boost::shared_ptr<IDbCache> >::iterator it = db_cache_map_.find(userId % _cluster); 
         if (it == db_cache_map_.end())
           return boost::shared_ptr<IDbCache>((IDbCache*)NULL);
         return (*it).second;
       }

       string getDSN() { return _dsn;}
       


     private:
       typedef vector<MessageQueueThread*> MessageQueueThreadVec;
       MessageQueueThreadVec _vecMessageQueueThread;
       bool _valid;
       IceUtil::RWRecMutex _validMutex;

       int _cluster;
       int _mod;
       string _dsn;
       map<int, boost::shared_ptr<IDbCache> > db_cache_map_;
    };

    class LoadUpdateDataToDbCacheTask: virtual public MyUtil::Task {
     private:
       MyUtil::LongSeq uids_;
     public:
       LoadUpdateDataToDbCacheTask(MyUtil::LongSeq uids);
       virtual void handle();
    };

 
   class MonitorUpdateLoginTimeTask : virtual public MyUtil::Task {
     public:
       /// @brief MonitorUpdateIdListFileTask
       /// 
       /// @param id_list_file 
       /// @param update_id_list_file_tag 
       MonitorUpdateLoginTimeTask(boost::shared_ptr<std::map<int,std::string> > last_logintime,MyUtil::LongSeq& updateLogTTIds,MyUtil::LongSeq& deleteIds,Int2ByteSeq& id2BytesMap);
       virtual void handle();
       void Write2TTAndMemCacheBuffer(long userId, SearchCacheDataPtr& obj, boost::shared_ptr<xce::searchcache::IDbCache>&, MyUtil::LongSeq&
deleteIds,  MyUtil::LongSeq& updateLogTTIds, Int2ByteSeq& id2BytesMap);
       void BinStr2ByteSeqMap(long userId, BinString* pBinString, Int2ByteSeq& id2Map);
     private:
       /// @brief 
       boost::shared_ptr<std::map<int,std::string> > last_logintime_;
       MyUtil::LongSeq& updateLogTTIds_;
       MyUtil::LongSeq& deleteIds_;
       Int2ByteSeq& id2BytesMap_;
    };


    class UpdateTask {
     public:
       UpdateTask();
       void handle(std::queue<boost::shared_ptr<UpdateUnit> >& update_units_ptr_queue);
       void UpdatePCAccordingIds(MyUtil::LongSeq&, MyUtil::LongSeq& updateLogIds, MyUtil::LongSeq& deleteIds, Int2ByteSeq& id2BytesMap);
       void UpdatePCAccordingIdAndTableName(UpdateUnitSeq&, MyUtil::LongSeq& queryFailedIds, MyUtil::LongSeq& updateLogIds, MyUtil::LongSeq& deleteIds, Int2ByteSeq& id2BytesMap);
       void UpdatePCAccordingFieldMap(UpdateUnitSeq&, MyUtil::LongSeq& queryFailedIds, MyUtil::LongSeq& updateLogTTIds, MyUtil::LongSeq& deleteIds, Int2ByteSeq& id2BytesMap);
       void UpdateSpecialTable(MyUtil::LongSeq& uids, com::xiaonei::xce::Statement& sql, void (SearchCacheData::*pFunc)(std::vector<mysqlpp::Row>& rows),  MyUtil::LongSeq& queryFailedIds, MyUtil::LongSeq& updateLogTTIds, MyUtil::LongSeq& deleteLogTTIds, Int2ByteSeq& id2BytesMap);
       int String2Int(const std::string&);
       void BinStr2ByteSeqMap(long userId, BinString* pBinString, Int2ByteSeq& id2Map);
       bool ReadSearchCacheDataFromTT(long userId, SearchCacheDataPtr& obj, boost::shared_ptr<xce::searchcache::IDbCache>&);
       void Write2TTAndMemCacheBuffer(long userId, SearchCacheDataPtr& obj, boost::shared_ptr<xce::searchcache::IDbCache>&, MyUtil::LongSeq& deleteIds,  MyUtil::LongSeq& updateLogTTIds, Int2ByteSeq& id2BytesMap);
     private:
       std::map<int,std::string> mm_last_logintime;
    };


    class UpdateLogTTTask:virtual public MyUtil::Task{
     public:
       UpdateLogTTTask(MyUtil::LongSeq& deleteLogTTIds, MyUtil::LongSeq& updateLogTTIds);
       virtual void handle();
       std::string LongSeq2String(const MyUtil::LongSeq& uids);
       static boost::shared_ptr<IDbCache> _db_cache;

     private:
       void DoUpdateLogTT(MyUtil::LongSeq& ids, const std::string PREFIX);
       MyUtil::LongSeq _deleteLogTTIds;
       MyUtil::LongSeq _updateLogTTIds;
    };

    class FillTask: virtual public MyUtil::Task {
     public:
       FillTask();
       virtual void handle();
    };

    class UpdateViewCountTask: virtual public MyUtil::Task {
     public:
       UpdateViewCountTask();
       virtual void handle();
    };

    class UpdateViewCountOnceTask: virtual public MyUtil::Task {
     public:
       UpdateViewCountOnceTask();
       virtual void handle();
    };


    //***************************************************************************

    class SearchCacheFactoryI: public MyUtil::ServantFactory {
     public:
       SearchCacheFactoryI();
       virtual Ice::ObjectPtr create(Ice::Long id);
       virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids);
    };

    //***************************************************************************
    class SingleResultHandlerI: public com::xiaonei::xce::ResultHandler {
     public:
       SingleResultHandlerI(SearchCacheDataPtr& result,void (SearchCacheData::*p)(mysqlpp::Row& row) );
       virtual bool handle(mysqlpp::Row& row) const;
     private:
       SearchCacheDataPtr _result;
       void (SearchCacheData::*_pFunc)(mysqlpp::Row& row);
    };

    //***************************************************************************
    class BatchVectorResultHandlerI: public com::xiaonei::xce::ResultHandler {
     public:
       BatchVectorResultHandlerI(std::string idField, std::map<long, std::vector<mysqlpp::Row> >&);
       virtual bool handle(mysqlpp::Row& row) const;
       std::string _idField;
       std::map<long, std::vector<mysqlpp::Row> >& _id2RowsMap;
    };
    //***************************************************************************
    class BatchResultHandlerI: public com::xiaonei::xce::ResultHandler {
     public:
       BatchResultHandlerI(std::string id, void (SearchCacheData::*p)(mysqlpp::Row& row));
       virtual bool handle(mysqlpp::Row& row) const;
       std::string _id;
       void (SearchCacheData::*_pFunc)(mysqlpp::Row& row);
    };
    //***************************************************************************
    class BatchBasicStatusResultHandlerI: public com::xiaonei::xce::ResultHandler {
     public:
       BatchBasicStatusResultHandlerI(Ice::Int& maxId);
       virtual bool handle(mysqlpp::Row& row) const;
     private:
       Ice::Int& _maxId;
    };

    //***************************************************************************
    class BasicStatusResultHandler2I: public com::xiaonei::xce::ResultHandler {
     public:
       BasicStatusResultHandler2I(Ice::Int& maxId, std::set<int>& valid_ids_set);
       virtual bool handle(mysqlpp::Row& row) const;
     private:
       Ice::Int& _maxId;
       std::set<int>& valid_ids_set_;
    };


    //***************************************************************************
    class BatchViewCountResultHandlerI: public com::xiaonei::xce::ResultHandler {
     public:
       BatchViewCountResultHandlerI(std::set<int>& valid_ids_set);
       virtual bool handle(mysqlpp::Row& row) const;
       std::string Int2String(int userId) const;
     private:
       std::set<int>& valid_ids_set_;
    };




    //***************************************************************************
    class BatchTask: public MyUtil::Task{
     public:
       BatchTask(std::string id, std::string db, void (SearchCacheData::*p)(mysqlpp::Row& row),
           const com::xiaonei::xce::Statement& sql,int level);
       virtual void handle();
     private:
       com::xiaonei::xce::Statement _sql;
       std::string _db;
       std::string _id;
       void (SearchCacheData::*_pFunc)(mysqlpp::Row& row);
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
