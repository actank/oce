#ifndef __FRIENDOFFRIENDSEARCHERMANAGERI_H__
#define __FRIENDOFFRIENDSEARCHERMANAGERI_H__

#include "../../library/include/map.h"
#include "../../library/include/mapdb.h"
#include "../../library/include/querycache.h"
#include "MD5.h"
#include "UtilCxx/src/Singleton.h"
#include "timeLog.h"
#include <boost/shared_ptr.hpp>
#include <SearchServer.h>
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <set>
using namespace com::xiaonei::search2::model;
using namespace com::xiaonei::search2::server;
namespace com {
  namespace renren{
    namespace search2{      
      namespace friendoffriend { 

        enum {
          HARD_LIMIT = 500,
        };

        const int kTaskLevelFill = 1;
        const string kFriendOfFriendSearcherManager = "FOF";
        static const std::string FRIENDS_TYPE = "FriendsType";
        static const std::string COMMON_FRIENDS_NUM = "CommonFriendsNum";
        static const std::string IS_FRIEND = "is_friend";
        static const std::string PAGE_NAME = "name";
        static const std::string APP_NAME = "app_name";
        static const std::string ID_FIELD = "GENERAL|user_basic.id";
        static const std::string QUERY_QUERY = "query";
        static const std::string QUERY_CALLER = "caller";
        static const int TIME_OUT = 2000;

        typedef boost::shared_ptr<timeLog> TimeLogPtr;

        class CacheKey  {
          public:
            CacheKey()  {
            }
            virtual ~CacheKey(){ }
            void CacheKeyGenerate(int user_id, std::string query, std::string& cache_key);
            static boost::shared_ptr<FriendOfFriendQueryCache> querycache_;
         };


        class FriendOfFriendSearcherManagerI: virtual public IndexManager, public MyUtil::Singleton<FriendOfFriendSearcherManagerI>, public CacheKey{
          public:
            FriendOfFriendSearcherManagerI();
            virtual IndexResultPtr searchIndex(const IndexCondition& query, int begin, int limit, const Ice::Current&);
            
            bool GetDataFromMem(const int user_id, std::string& names, IndexResultPtr friendinfo_list);

            void bootstrap();
            virtual bool isValid(const Ice::Current&);

          private :

            bool IsName(const std::string & query);
            bool IsOnlyQuery(const IndexCondition &query);

            IndexResultPtr GeneralSearch(const int user_id, IndexCondition query, TimeLogPtr time, int begin = 0, int limit = HARD_LIMIT);
            int CombineResults(const std::string &query, IndexResultPtr fof_result,
                IndexResultPtr general_result, IndexResultPtr out_result,
                TimeLogPtr time, int begin, int limit);
            int itoa(int i,char* string);

            std::set<std::string> family_names_in_set_;
            std::set<std::string> not_regular_names_in_set_;
            IceUtil::RWRecMutex rw_cache_lock_;  //保证并发正常
            FriendOfFriendMapDB db_;
            bool db_right_; 
            bool query_cache_right_;
            int cluster_;
            int mod_;
        };

        class UpdateCache: virtual public MyUtil::Task, public CacheKey  {
          public:
            UpdateCache(int user_id, std::string query, int total, IndexResultPtr friend_info_list, bool cache_db_right);
            virtual void handle();
          private:
            int user_id_;
            string query_;
            int total_;
            bool cache_db_right_;
            IndexResultPtr friend_info_list_;
            IceUtil::RWRecMutex write_cache_lock_;
        };

        class TaskReference {
          public:
            TaskReference(bool search_completed) : search_completed_(search_completed)  {
            }
            void Post() {
              try {
                MCE_DEBUG("[TEMPLE] before Post");
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
                if (search_completed_) {
                  mutex_.notify();
                }
              } catch (IceUtil::Exception& e) {
                MCE_FATAL("TaskReference::Post() IceUtil::Exception " << e.what());
              } catch (std::exception& e) {
                MCE_FATAL("TaskReference::Post() std::exception " << e.what());
              } catch (...) {
                MCE_FATAL("TaskReference::Post() unknown exception ");
              }

            }
            bool Wait(TimeLogPtr general_cost) {
              MCE_DEBUG("[TEMPLE] in TaskReference::Wait() ");
              double time = general_cost->GeneralSearchCost;
              try {
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
                if (!search_completed_) {
                  if(time < TIME_OUT)  {
                    mutex_.timedWait(IceUtil::Time::milliSeconds(TIME_OUT - general_cost->GeneralSearchCost));
                  }
                  else  {
                    mutex_.timedWait(IceUtil::Time::milliSeconds(0));
                  }
                }
                return search_completed_; 
              } catch (IceUtil::Exception& e) {
                MCE_FATAL("TaskReference::Wait() IceUtil::Exception " << e.what());
              } catch (std::exception& e) {
                MCE_FATAL("TaskReference::Wait() std::exception " << e.what());
              } catch (...) {
                MCE_FATAL("TaskReference::Wait() unknown exception ");
              }
              return true;
            }
            void Set(bool complete)  {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
              search_completed_ = complete;
            }
          private:
            bool search_completed_;
            IceUtil::Monitor<IceUtil::Mutex> mutex_;
        };

        typedef boost::shared_ptr<TaskReference>  TaskReferencePtr;

        class InvokeMemIndexer: virtual public MyUtil::Task  {
          public:
            InvokeMemIndexer(int user_id, string &query_string, IndexResultPtr result, TimeLogPtr time, TaskReferencePtr finished, int begin = 0, int limit = HARD_LIMIT);
            virtual void handle();
          private:
            int user_id_;
            string query_string_;
            IndexResultPtr result_;
            int begin_;
            int limit_;
            TimeLogPtr time_;
            TaskReferencePtr finished_;
        };

        class ServiceSchemaI: public MyUtil::Descriptor {
          public:
            virtual void configure(const Ice::PropertiesPtr& props);
        };
      }
    }

  }
}
#endif
