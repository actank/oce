#ifndef __FRIENDOFFRIENDSEARCHERMANAGERI_H__
#define __FRIENDOFFRIENDSEARCHERMANAGERI_H__

#include "MD5.h"
#include "UtilCxx/src/Singleton.h"
#include "UtilCxx/src/Evictor.h"
#include "timeLog.h"
#include "FriendOfFriendCache.h"
#include <boost/shared_ptr.hpp>
#include <SearchServer.h>
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <set>
#include <queue>
#include <algorithm>
using namespace com::xiaonei::search2::model;
using namespace com::xiaonei::search2::server;
namespace com {
  namespace renren{
    namespace search2{      
      namespace friendoffriend { 

        enum {
          HARD_LIMIT = 2000,
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
        bool compare(const FriendInfo& f1, const FriendInfo& f2)  {
          if(f1.type <f2.type)  {
            return true;
          }  else if(f1.type == f2.type)  {
            return f1.commonFriendsNum > f2.commonFriendsNum;
          }  else  {
            return false;
          }
        }


        /*struct FriendInfo
          {
        //好友的id
        int id;
        //好友的类型(零度好友表示自己，一度好友，二度好友，三度好友...)
        short type;
        //共同好友数 
        short commonFriendsNum;
        };
        typedef vector<FriendInfo> FriendInfoSeq;*/

        typedef boost::shared_ptr<timeLog> TimeLogPtr;

        class FetchFriendOfFriend  {
          public:
            FetchFriendOfFriend()  {
            }
            virtual ~FetchFriendOfFriend() { }

            void  Merge(int userId, std::vector<int>& friends, std::map<int,std::vector<int> >& in, FriendInfoSeq& out){
              struct timeval begin;
              struct timeval end;
              gettimeofday(&begin, NULL);
              std::priority_queue<k_node> q;
              {
                vector<int> tmp(1);
                tmp.push_back(userId);
                k_node item;
                item.cur = tmp.begin();
                item.end = tmp.end();
                item.type = 0;
                q.push(item);
              }

              for(std::vector<int>::iterator it=friends.begin();it!=friends.end();){
                k_node item;
                item.cur = it;
                item.end = ++it;
                item.type = 1;
                q.push(item);
              }

              for(std::map<int,std::vector<int> >::iterator it=in.begin();it!=in.end();
                  ++it){
                if(it->second.size()==0)
                  continue;
                k_node item;
                item.cur = it->second.begin();
                item.end = it->second.end();
                item.type = 2;
                q.push(item);
              }

              while(!q.empty()){
                k_node item = q.top();
                if(out.empty() || out.back().id != *(item.cur))  {
                  FriendInfo info;
                  info.id = *item.cur;
                  info.type = item.type;
                  if(item.type == 2){
                    info.commonFriendsNum = 1;
                  }else if(item.type == 1){
                    info.commonFriendsNum = 0;
                  }else if(info.type == 0){
                    info.commonFriendsNum = 0;
                  }
                  out.push_back(info);       
                     
                }  else {
                     item.type == out.back().type; 
                     if(out.back().type != 0)  {
                       out.back().commonFriendsNum++;
                     }
                     
                }
               q.pop();
                if(++item.cur!=item.end){
                  q.push(item);
                }
              }
              //std::sort(out.begin(), out.end(), compare);
              gettimeofday(&end, NULL);
              MCE_INFO("priority-merge id: "<<userId << " 1friends: "<<in.size()<<" 2friends: "<<
                  out.size()<<" cost: "<<((end.tv_sec-begin.tv_sec)*1000000+(end.tv_usec-begin.tv_usec))/1000<<" ms");
            }

            void Combine(std::vector<int> & id_list, FriendInfoSeq& friend_info_list, FriendInfoSeq& friend_info_final)  {
              std::vector<int>::iterator id_cur, id_end;
              FriendInfoSeq::iterator info_cur, info_end;
              id_cur = id_list.begin();
              id_end = id_list.end();
              info_cur = friend_info_list.begin();
              info_end = friend_info_list.end();
              struct timeval begin;
              struct timeval end;
              gettimeofday(&begin, NULL);
              while((id_cur != id_end) && (info_cur != info_end))  {
                if(*id_cur < (*info_cur).id)  {
                  id_cur++;
                }  else {
                  if(*id_cur ==(*info_cur).id)  {
                    friend_info_final.push_back(*info_cur);
                    id_cur++;
                  }
                  info_cur++;
                }                 
              }
              std::stable_sort(friend_info_final.begin(), friend_info_final.end(), compare);//稳定排序
              gettimeofday(&end, NULL);
              MCE_INFO("Time taken in intesecting FoF and name is: "<<
                  ((end.tv_sec*1000000 + end.tv_usec) - (begin.tv_sec*1000000 + begin.tv_usec))/1000<<" ms");
            }

          private:
            struct k_node{
              std::vector<int>::iterator cur;
              std::vector<int>::iterator end;
              int type;
              bool operator<(const k_node& other) const{
                if(*cur!=*(other.cur))
                  return *cur > *(other.cur);
                else
                  return type > other.type; 
              }
            };
        };

        /// @brief 所有用户的id到姓名的一个大映射表，可从文件中读取，也可保存到文件中
        class AllName2Ids {
          public:
            /// @brief AllName2Ids
            /// 
            /// @param hash_size(in)  由于使用了hash_map, 为减少resize带来的性能问题，采用预分配的方法
            AllName2Ids(int hash_size);
            void Set(int id, string name);
            bool Get(string name, std::vector<int>& id_list);
            void Update(int id, string new_name);
            bool IsValid();
            bool SetValid();
            int Size();
          private:
            __gnu_cxx::hash_map<string, vector<int> > all_name_2_ids_map_;
            IceUtil::RWRecMutex all_name_2_ids_map_rw_lock_;
            bool all_name_2_ids_map_valid_;
        };

        class LoadAndUpdateId2NameMapTask : public MyUtil::Task {
          public :
            LoadAndUpdateId2NameMapTask(AllName2Ids& all_name_2_ids, const bool read_from_file_flag, const string& file);
            virtual void handle();
          private:
            AllName2Ids& all_name_2_ids_;
            bool read_from_file_flag_;
            string file_;
        };

        class BatchResultHandlerI: public com::xiaonei::xce::ResultHandler {
          public:
            BatchResultHandlerI(AllName2Ids& name_2_ids, int& max_id) : name_2_ids_(name_2_ids), max_id_(max_id) {};
            virtual bool handle(mysqlpp::Row& row) const;
          private :
            AllName2Ids& name_2_ids_;
            bool read_from_file_flag_;
            int& max_id_;
        };  


        class FriendOfFriendSearcherManagerI: virtual public IndexManager, public MyUtil::Singleton<FriendOfFriendSearcherManagerI>{
          public:
            FriendOfFriendSearcherManagerI();
            virtual IndexResultPtr searchIndex(const IndexCondition& query, int begin, int limit, const Ice::Current&);
            virtual IndexResultPtr searchIndexWithRestriction(const IndexCondition& query, int begin, int limit, const string& callerName, const Ice::Current&);


            void bootstrap();
            virtual bool isValid(const Ice::Current&);

          private :

            bool IsOnlyQuery(const IndexCondition &query);
            MyUtil::SizedEvictor<int, FriendInfoSeq> cache_friend_info_;
            IndexResultPtr GeneralSearch(const int user_id, IndexCondition query, TimeLogPtr time, int begin = 0, int limit = HARD_LIMIT);
            int CombineResults(const std::string &query, IndexResultPtr fof_result,
                IndexResultPtr general_result, IndexResultPtr out_result,
                TimeLogPtr time, int begin, int limit);
            int CombineResultsOld(const std::string &query, IndexResultPtr fof_result,
                IndexResultPtr general_result, IndexResultPtr out_result,
                TimeLogPtr time, int begin, int limit);
            int itoa(int i,char* string);

            AllName2Ids all_name_2_ids_;
            IceUtil::RWRecMutex rw_cache_lock_;  //保证并发正常
            bool db_right_; 
            bool query_cache_right_;
            int cluster_;
            int mod_;
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

        class InvokeFriendOfFriend: virtual public MyUtil::Task  {
          public:
            InvokeFriendOfFriend(int user_id, string &query_string, AllName2Ids& name_ids, IndexResultPtr result, 
                MyUtil::SizedEvictor<int, FriendInfoSeq>& cache_friends, TimeLogPtr time, TaskReferencePtr finished, bool & excep, int begin = 0, int limit  = HARD_LIMIT);
            virtual void handle();
            bool GetFriendOfFriend(int user_id, string &query_string, AllName2Ids& name_ids, IndexResultPtr friend_of_friends);
            int itoa(int i,char*string);
          private:
            int user_id_;
            string query_string_;
            AllName2Ids& name_ids_;
            IndexResultPtr result_;
            MyUtil::SizedEvictor<int, FriendInfoSeq>& cache_friends_;
            TimeLogPtr time_;
            TaskReferencePtr finished_;
            bool& excep_;
            int begin_;
            int limit_;
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
