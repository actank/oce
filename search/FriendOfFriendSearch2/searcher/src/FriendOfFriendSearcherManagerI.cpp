#include "FriendOfFriendSearcherManagerI.h"

#include "SearchServer.h"
#include "../../adapter/include/common.h"
#include "../../adapter/include/SearchManagerAdapter.h"
#include "../../adapter/include/PeopleSearchManagerAdapter.h"
#include "../../adapter/include/FilterSearchManagerAdapter.h"
#include "time.h"
#include "UtilCxx/src/Evictor.h"
#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#define DEBUG_TIME

#ifdef DEBUG_TIME
#include <sys/time.h>
#endif

using namespace MyUtil;
using namespace IceUtil;
using namespace com::renren::search2::friendoffriend;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::search2::model;
using namespace com::xiaonei::search2::server;
using namespace xce::buddy::adapter;


//***************************************************************************

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  FriendOfFriendSearcherManagerI &instance = FriendOfFriendSearcherManagerI::instance();

  instance.bootstrap();

  service.getAdapter()->add(&instance,service.createIdentity(kFriendOfFriendSearcherManager, ""));

  service.registerXceObserver(new ServiceSchemaI);
}

namespace com {
  namespace renren {
    namespace search2 {
      namespace friendoffriend  {
        using namespace MyUtil;

        const int kTaskLevelUpdateCacheTask = 8;
        const int kTaskLevelGeneralSearchTask = 8;
        const string DB_SEARCH2_USERNAME = "user_names";
        const string kUserNameField = "id, name";
        const string kUserNameTable = "user_names";
        const int kMaxSqlQueryNum = 3000;
        const int kTaskLevelForFetchName = 5;

        AllName2Ids::AllName2Ids(int hash_size) : all_name_2_ids_map_(hash_size), all_name_2_ids_map_valid_(false) {
        }

        void AllName2Ids::Set(int id, string name ) {
          IceUtil::RWRecMutex::WLock lock(all_name_2_ids_map_rw_lock_);
          all_name_2_ids_map_[name].push_back(id);
        }

        int AllName2Ids::Size() {
          IceUtil::RWRecMutex::RLock lock(all_name_2_ids_map_rw_lock_);
          return all_name_2_ids_map_.size();
        }

        bool AllName2Ids::Get(string name, std::vector<int>& id_list) {
          IceUtil::RWRecMutex::RLock lock(all_name_2_ids_map_rw_lock_);
          __gnu_cxx::hash_map<string, std::vector<int> >::iterator iter = all_name_2_ids_map_.find(name);
          if(iter != all_name_2_ids_map_.end())  {
            id_list = iter->second;
            return true;
          }
          else  {
            return false;
          }
        }

        void AllName2Ids::Update(int id, string new_name) {
          std::vector<int> ids_vec;
          bool in_vec_not = false;
          IceUtil::RWRecMutex::RLock lock(all_name_2_ids_map_rw_lock_);
          __gnu_cxx::hash_map<string, std::vector<int> >::iterator iter_map = all_name_2_ids_map_.find(new_name);
          if(iter_map != all_name_2_ids_map_.end())  {
            for(std::vector<int>::iterator iter= iter_map->second.begin(); iter != iter_map->second.end(); iter++)  {
              if(id == *iter)  {
                in_vec_not = true;
                break;
              }
            }
            if(!in_vec_not)  {
              iter_map->second.push_back(id);
            }
          }  else  {
            Set(id, new_name);
          }         
        }

        bool AllName2Ids::IsValid() {
          IceUtil::RWRecMutex::RLock lock(all_name_2_ids_map_rw_lock_);
          return all_name_2_ids_map_valid_;
        }

        bool AllName2Ids::SetValid() {
          IceUtil::RWRecMutex::WLock lock(all_name_2_ids_map_rw_lock_);
          all_name_2_ids_map_valid_ = true;
          return true;
        }
        LoadAndUpdateId2NameMapTask::LoadAndUpdateId2NameMapTask(AllName2Ids& all_name_2_ids, const bool read_from_file_flag, const string& file) : MyUtil::Task(kTaskLevelForFetchName), all_name_2_ids_(all_name_2_ids), read_from_file_flag_(read_from_file_flag), file_(file) {
          MCE_INFO("[TEMPLE] in LoadAndUpdateId2NameMapTask::LoadAndUpdateId2NameMapTask");
        }

        void LoadAndUpdateId2NameMapTask::handle() {
          MCE_INFO("[TEMPLE] Step in LoadAndUpdateId2NameMapTask::handle, read_from_file_flag_=" << read_from_file_flag_ << ", file=" << file_ );
          //1.从数据库中获取所有用户的id和名字，建立id->name的映射
          int begin_id = 0;
          int max_id = 0;
          int idx = 0;
          do {
            begin_id = max_id;
            Statement sql ;
            sql << "SELECT " << kUserNameField << " FROM " << kUserNameTable << " WHERE id>" << begin_id << " LIMIT " << kMaxSqlQueryNum;

            BatchResultHandlerI handler(all_name_2_ids_, max_id);
            QueryRunner(DB_SEARCH2_USERNAME, CDbRServer).query(sql, handler);
            if (0 == ++idx % 100) {
              MCE_INFO("[TEMPLE]max_id=" << max_id);
            }
          } while (begin_id < max_id);

          all_name_2_ids_.SetValid();
          MCE_INFO("[TEMPLE] GetNameFromDB " << " finished once, all_name_2_ids_.Size()=" << all_name_2_ids_.Size());
        }


        bool BatchResultHandlerI::handle(mysqlpp::Row& row) const {
          int user_id = (int)row["id"];
#ifndef NEWARCH                                                                         
          string name = row["name"].get_string();                            
#else                                                                                   
          string name = row["name"].c_str();                                 
#endif     

          name_2_ids_.Set(user_id, name);

          max_id_ = user_id > max_id_ ? user_id : max_id_;

          return true;
        }


        InvokeFriendOfFriend::InvokeFriendOfFriend(int user_id, string &query_string, AllName2Ids& name_ids, IndexResultPtr result, MyUtil::SizedEvictor<int, FriendInfoSeq>& cache_friends, TimeLogPtr time, TaskReferencePtr finished, bool & excep, int begin,  int limit):MyUtil::Task(kTaskLevelGeneralSearchTask), user_id_(user_id), query_string_(query_string), name_ids_(name_ids), result_(result), cache_friends_(cache_friends),time_(time), finished_(finished), excep_(excep), begin_(begin), limit_(limit) { }

        void  InvokeFriendOfFriend::handle()  {
          struct timeval start, end;
          long duration;
          gettimeofday(&start, NULL);
          if(!GetFriendOfFriend(user_id_, query_string_, name_ids_, result_))  {
            MCE_INFO("Error in calculating friend of friends !");
          }
          gettimeofday(&end, NULL);
          duration = ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec))/1000;
          MCE_INFO("Time cost in fetching friend of friends: "<<duration<<" ms");
          finished_->Set(true);
          finished_->Post();
        }

        int InvokeFriendOfFriend::itoa(int i,char*string) {
          int power,j; j=i;
          int count = 0;
          for(power=1;j>=10;j/=10) power*=10;
          for(;power>0;power/=10) {
            *string++ ='0' + i/power;
            i%=power;
            count ++;
          }
          *string='\0';
          return count;
        }


        bool InvokeFriendOfFriend::GetFriendOfFriend(int user_id, string &query_string, AllName2Ids& name_ids, IndexResultPtr friend_of_friends)  {
          std::vector<int> id_list;
          FriendInfoSeq friends_info_list;
          FriendInfoSeq friends_info_final;

          IndexResultItem friend_item;
          if(name_ids.Get(query_string, id_list))  {
            char friends_type[10];
            char common_friends[10];

            boost::shared_ptr<FetchFriendOfFriend> fetch_friends(new FetchFriendOfFriend());
            friends_info_list = cache_friends_.find(user_id, friends_info_list);
            if(friends_info_list.size() == 0) {
              try  {
                std::vector<int> direct_friends_list = BuddyByIdCacheAdapter::instance().getFriendListAsc(user_id, 2000);
                std::map<int, vector<int> > id_2_indirect_friends_map = BuddyByIdCacheAdapter::instance().getFriendLists(direct_friends_list);
                fetch_friends->Merge(user_id, direct_friends_list, id_2_indirect_friends_map, friends_info_list);
                cache_friends_.add(friends_info_list, user_id);
              }  catch (Ice::Exception& e) {
                MCE_INFO("BuddyByIdCacheAdapter::get Ice::Exception " << e << ", id=" << user_id);
                excep_ = true;
                return false;
              } catch (std::exception& e) {
                MCE_INFO("BuddyByIdCacheAdapter::get std::exception " << e.what() << ", id=" << user_id);
                excep_ = true;
                return false;
              } catch (...) {
                MCE_INFO("BuddyByIdCacheAdapter::get Unknown exception" << ", id=" << user_id);
                excep_ = true;
                return false;
              } 
            }
            fetch_friends->Combine(id_list, friends_info_list, friends_info_final);
            friend_of_friends->totalCount = friends_info_final.size();
            friend_of_friends->typedCount[User] = friends_info_final.size();
            for(size_t i = 0; i < friends_info_final.size(); i++)  {
              FriendInfo friend_tmp = friends_info_final.at(i);
              friend_item.id = friend_tmp.id;
              friend_item.type = User;
              itoa(friend_tmp.type, friends_type); 
              friend_item.content[FRIENDS_TYPE] = (string) friends_type;
              itoa(friend_tmp.commonFriendsNum, common_friends);
              friend_item.content[COMMON_FRIENDS_NUM] =  (string) common_friends;
              if(friend_item.content[FRIENDS_TYPE] == "0" || friend_item.content[FRIENDS_TYPE] == "1")  {
                friend_item.content[IS_FRIEND] = "1";
              } else  {
                friend_item.content[IS_FRIEND] = "0";
              }

              friend_of_friends->contents.push_back(friend_item);        

              MCE_INFO("GET id:"<< friend_item.id<< " type:"<< friend_item.content[FRIENDS_TYPE]<<" common friends:"<< friend_item.content[COMMON_FRIENDS_NUM]);
            }
            return true;
          }  else  {
            MCE_INFO("Can not get name="+query_string+" of friends!");
            return false;
          }               
        }

        FriendOfFriendSearcherManagerI::FriendOfFriendSearcherManagerI() : all_name_2_ids_(25000000) {

        }

        /// @brief FriendOfFriendSearcherManagerI::bootstrap
        /// 与业务逻辑相关的初始化
        void FriendOfFriendSearcherManagerI::bootstrap() {
          ServiceI& service = ServiceI::instance();
          cluster_ = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendOfFriendSearcher.Cluster");
          mod_ = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendOfFriendSearcher.Mod");

          bool read_from_file_flag = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendOfFriendIndexer.ReadFromFileFlag");
          MCE_INFO("FriendOfFriendIndexer.ReadFromFileFlag=" << read_from_file_flag);
          string file = service.getCommunicator()->getProperties()->getProperty("FriendOfFriendIndexer.Id2NameMapFile");
          MCE_INFO("FriendOfFriendIndexer.Id2NameMapFile=" << file);

          //加载并更新id到姓名的映射
          MCE_INFO("[TEMPLE] TaskManager::instance().execute(new LoadAndUpdateId2NameMapTask)...");                                   
          TaskManager::instance().execute(new LoadAndUpdateId2NameMapTask(all_name_2_ids_, read_from_file_flag, file));

          MCE_INFO("Done!");
        }



        bool FriendOfFriendSearcherManagerI::IsOnlyQuery(const IndexCondition &query) {
          if (query.type == OpenSearch) {
            return true;
          }
          com::xiaonei::search2::model::Str2StrMap::const_iterator iter;
          for (iter = query.queryCondition.begin(); iter != query.queryCondition.end(); ++iter) {
            const std::string &name = iter->first;
            if (name != QUERY_QUERY && name != QUERY_CALLER) {
              MCE_DEBUG("not query: " << name);
              return false;
            }
          }
          return true;
        }

        //这是一个空实现,目的是为了让这个类可以编译通过，因为在slice里面增加了一个新的接口，他必须实现
        IndexResultPtr FriendOfFriendSearcherManagerI::searchIndexWithRestriction(const IndexCondition& query, int begin, 
            int limit, const string& callerName, const Ice::Current&) {
          return 0;
        }

        IndexResultPtr FriendOfFriendSearcherManagerI::searchIndex(const IndexCondition& query, int begin, 
            int limit, const Ice::Current&)  {
          struct timeval total_start, total_end;
          TimeLogPtr log_duration(new timeLog);
          int duration, total_count;
          IndexResultPtr  out_result;
          string query_string = const_cast<IndexCondition&>(query).queryCondition[QUERY_QUERY];
          int userid = 0;
          bool excep = false;
          log_duration->IndexerCost = 0;
          log_duration->CombineCost = 0;

          gettimeofday(&total_start, NULL);

          if (query.opUserCondition.find(ID_FIELD) != query.opUserCondition.end()) {
            userid = atoi((const_cast<IndexCondition&>(query).opUserCondition[ID_FIELD]).c_str());
          }
          try  { 
            if (all_name_2_ids_.Size() > 0 &&  IsOnlyQuery(query)) {
              IndexResultPtr friendinfo_list = new IndexResult();
              TaskReferencePtr finished(new TaskReference(false));
              bool return_right;

              TaskManager::instance().execute(new InvokeFriendOfFriend(userid, query_string, all_name_2_ids_, friendinfo_list, 
                    cache_friend_info_, log_duration, finished, excep));
//              MCE_INFO("TEST ====> Before GeneralSearch!");
              IndexResultPtr original_result = GeneralSearch(userid, query, log_duration);
              return_right = finished->Wait(log_duration);
              if(!return_right)  {
                MCE_INFO("Time out when waiting for Indexer!");
                return original_result;
              }
              if(!excep)  {
                out_result = new IndexResult();
                out_result->currentPos = 0;
                out_result->totalCount = 0;
//                MCE_INFO("TEST ====> Before CombineResults!");
                total_count = CombineResults(query_string, friendinfo_list, original_result,
                    out_result, log_duration, begin, limit);
//                MCE_INFO("TEST ====> After CombineResults!");
              }  else  {
                out_result = GeneralSearch(userid, query, log_duration, begin, limit);
                total_count = out_result->totalCount;
              }
            } else  {
              out_result = GeneralSearch(userid, query, log_duration, begin, limit);
              total_count = out_result->totalCount;
            }
          }  catch (Ice::Exception& e) {
            MCE_INFO("searchIndex::get Ice::Exception " << e << ", id=" << userid<<", query="<<query_string);
          } catch (std::exception& e) {
            MCE_INFO("searchIndex::get std::exception " << e.what() << ", id=" << userid<<", query="<<query_string);
          } catch (...) {
            MCE_INFO("searchIndex::get Unknown exception" << ", id=" << userid<<", query="<<query_string);
          } 
          gettimeofday(&total_end, NULL);
          duration = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start.tv_sec*1000000 + total_start.tv_usec))/1000;
          out_result->timeCost = duration;
          MCE_INFO("UserId: "<<userid<<" Query: "<<query_string<<" CurrentPos: "<< out_result->currentPos<<" TotalCount: "<<out_result->totalCount<<" SearchTypeCount: "<<out_result->typedCount[User]<<" General cost: "<<log_duration->GeneralSearchCost<<" ms; Combine cost: "<<log_duration->CombineCost<<" ms; Time taken in Searcher is: "<<duration<<" ms");
          return out_result;
        }

        IndexResultPtr FriendOfFriendSearcherManagerI::GeneralSearch(const int user_id, IndexCondition query, TimeLogPtr time, int begin, int limit)  {
          if (query.type == OpenSearch) {
            IndexResultPtr result = SearchManagerAdapter::instance().SearchIndex(query, begin , limit);
            time->GeneralSearchCost = (int)result->timeCost;
            return result;
          } else if (query.type == OpenPeopleSearch) {
            IndexResultPtr result = PeopleSearchManagerAdapter::instance().SearchIndex(query, begin , limit);
            time->GeneralSearchCost = (int)result->timeCost;
            return result;
          } else if (query.type == FilterSearch)  {
            IndexResultPtr result = FilterSearchManagerAdapter::instance().SearchIndex(query, begin, limit);
            time->GeneralSearchCost = (int)result->timeCost;
            return result;
          } else {
            return new IndexResult;
          }
        }

        int FriendOfFriendSearcherManagerI::CombineResults(const std::string &query, IndexResultPtr fof_result,
            IndexResultPtr general_result, IndexResultPtr out_result, TimeLogPtr time, int begin, int limit)  {
//          MCE_INFO("TEST ====> FriendOfFriendSearcherManagerI::CombineResults! " << "fof_size=" << fof_result->contents.size() << " general_size=" << general_result->contents.size() << " begin=" << begin << " limit=" << limit);
//          int result_left;
//          int result_begin;
          size_t fof_size;
          size_t general_size;
          int general_pos = 0;
          struct timeval total_start, total_end;
          int duration;
          IndexResultPtr user_result, app_result, page_result,all_result;
          int user_size = 0, app_size = 0, page_size = 0;

          user_result = new IndexResult();
          app_result = new IndexResult();
          page_result = new IndexResult();
          all_result = new IndexResult();

          gettimeofday(&total_start, NULL);
          fof_size = fof_result->contents.size();
          general_size = general_result->contents.size();

//          MCE_INFO("TEST ====> fof_size=" << fof_size << " general_size=" << general_size);
          begin = (begin > HARD_LIMIT ? HARD_LIMIT : begin);
          limit = (limit > HARD_LIMIT ? HARD_LIMIT : limit);
          if (begin + limit > HARD_LIMIT) {
            begin = HARD_LIMIT - limit;
          }
          if (general_size + fof_size == 0 || begin >= int(general_size + fof_size)) {
            gettimeofday(&total_end, NULL);
            duration = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start.tv_sec*1000000 + total_start.tv_usec))/   1000;
            time -> CombineCost = duration;
            return 0;
          }
          
//          MCE_INFO("TEST ====> OK1111");
          std::set<int>  friends_id_info;
          for (size_t i = 0; i < fof_size; ++i) {
            IndexResultItem &item = fof_result->contents.at(i);
//            MCE_INFO("TEST ====> id=" << fof_result->contents.at(i).id);
            friends_id_info.insert(item.id);
            user_result->contents.push_back(item);
          }
//          MCE_INFO("TEST ====> user_size=" << user_result->contents.size());

          if (general_size > 0) {
            for (general_pos = 0; general_pos < (int)general_size; ++general_pos) {
              IndexResultItem &item = general_result->contents.at(general_pos);
              if (item.type == Page) {
                page_result->contents.push_back(item);
              } else if (item.type == App) {
                app_result->contents.push_back(item);
              } else if (item.type == User) {
                if (friends_id_info.find(item.id) != friends_id_info.end()) { // duplicated
                  continue;
                }
                item.content[FRIENDS_TYPE] = "7";
                item.content[COMMON_FRIENDS_NUM] = "0";
                user_result->contents.push_back(item);
              }
            }

            page_size = page_result->contents.size();
            app_size = app_result->contents.size();
            user_size = user_result->contents.size();
          }

//-------合并搜索结果为一个大结果-------
          if (page_size != 0) {
            all_result->contents.push_back(page_result->contents.at(0));
          }
          if (app_size != 0) {
            all_result->contents.push_back(app_result->contents.at(0));
          }
          int user_end = (user_size > limit - 2) ? limit - 2 : user_size;
          for (int i = 0; i < user_end; i++) {
            all_result->contents.push_back(user_result->contents.at(i));
          }
          if (page_size != 0) {
            for (int i = 1; i < page_size; i++) {
              all_result->contents.push_back(page_result->contents.at(i));
            }
          }
          if (app_size != 0) {
            for (int i = 1; i < app_size; i++) {
              all_result->contents.push_back(app_result->contents.at(i));
            }
          }
//          int user_begin = limit - 2;
          for (int i = user_end; i < user_size; i++) {
            all_result->contents.push_back(user_result->contents.at(i));
          }
          int out_end = (int(general_size) > begin + limit) ? begin + limit : int(general_size);
          for (int i = begin; i < out_end; ++i) {
            out_result->contents.push_back(all_result->contents.at(i));
          }
//---------------------------------------------------------
          out_result->currentPos = begin;
          out_result->totalCount = general_result->totalCount;
          out_result->typedCount = general_result->typedCount;
          out_result->typedCount[User] = out_result->typedCount[User];
          gettimeofday(&total_end, NULL);
          duration = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start.tv_sec*1000000 + total_start.tv_usec))/1000;
          time -> CombineCost = duration;
          return out_result->totalCount;
        }


        int FriendOfFriendSearcherManagerI::CombineResultsOld(const std::string &query, IndexResultPtr fof_result,
            IndexResultPtr general_result, IndexResultPtr out_result, TimeLogPtr time, int begin, int limit)  {
          int result_left;
          int result_begin;
          size_t fof_size;
          size_t general_size;
          int general_pos = 0;
          struct timeval total_start, total_end;
          int duration;

          gettimeofday(&total_start, NULL);
          fof_size = fof_result->contents.size();
          general_size = general_result->contents.size();

          begin = (begin > HARD_LIMIT ? HARD_LIMIT : begin);
          limit = (limit > HARD_LIMIT ? HARD_LIMIT : limit);
          if (begin + limit > HARD_LIMIT) {
            begin = HARD_LIMIT - limit;
          }
          if (general_size + fof_size == 0 || begin >= int(general_size + fof_size)) {
            gettimeofday(&total_end, NULL);
            duration = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start.tv_sec*1000000 + total_start.tv_usec))/1000;
            time -> CombineCost = duration;
            return 0;
          }

          std::set<int>  friends_id_info;
          for (size_t i = 0; i < fof_size; ++i) {
            IndexResultItem &item = fof_result->contents.at(i);
            friends_id_info.insert(item.id);
          }

          result_begin = begin;
          result_left = limit;

          if (general_size > 0) {
            int pos = 0;

            for (general_pos = 0; general_pos < (int)general_size && pos < result_begin + result_left; ++general_pos) {
              IndexResultItem &item = general_result->contents.at(general_pos);
              if (item.type == Page || item.type == App) {
                if (pos >= result_begin) {
                  out_result->contents.push_back(item);
                }
                ++pos;
              } else {
                break;
              }
            }
            if (pos <= result_begin) {
              result_begin -= pos;
            } else {
              result_left -= pos - result_begin;
              result_begin = 0;
            }
          }

          if (result_begin < (int)fof_size) {
            int end = result_begin + result_left;
            end = (end > (int)fof_size ? (int)fof_size : end);
            for (int i = result_begin; i < end; ++i) {
              IndexResultItem &item = fof_result->contents.at(i);
              out_result->contents.push_back(item);
            }
            int count = end - result_begin;
            result_begin = 0;
            result_left -= count;
          } else {
            result_begin -= fof_size;
          }

          result_begin += general_pos;
          if (result_begin < (int)general_size) {
            int count = 0;
            for (int i = result_begin; count < result_left && i < (int)general_size; ++i) {
              IndexResultItem &item = general_result->contents.at(i);
              if (item.type == User) {
                if (friends_id_info.find(item.id) != friends_id_info.end()) { // duplicated
                  continue;
                }
                item.content[FRIENDS_TYPE] = "7";
                item.content[COMMON_FRIENDS_NUM] = "0";
              }
              out_result->contents.push_back(item);
              ++count;
            }
            result_begin = 0;
            result_left -= count;
          } else {
            result_begin -= general_size;
          }

          out_result->currentPos = begin;
          out_result->totalCount = general_result->totalCount;
          out_result->typedCount = general_result->typedCount;
          out_result->typedCount[User] = out_result->typedCount[User];
          gettimeofday(&total_end, NULL);
          duration = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start.tv_sec*1000000 + total_start.tv_usec))/1000;
          time -> CombineCost = duration;
          return out_result->totalCount;
        }

        bool FriendOfFriendSearcherManagerI::isValid(const Ice::Current&) {
          return true;
        }

        //***************************************************************************

        void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
          TaskManager::instance().config(props, "Fill", kTaskLevelFill, ThreadPoolConfig(1, 1));
        }

      }
    }
  }
}
