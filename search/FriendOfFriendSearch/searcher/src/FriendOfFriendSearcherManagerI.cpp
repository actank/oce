#include "FriendOfFriendSearcherManagerI.h"

#include "SearchServer.h"

#include "../../adapter/include/SearchManagerAdapter.h"
#include "../../adapter/include/PeopleSearchManagerAdapter.h"

#include "time.h"

#include "FriendOfFriendIndexerAdapter.h"

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

boost::shared_ptr<FriendOfFriendQueryCache> CacheKey::querycache_ = boost::shared_ptr<FriendOfFriendQueryCache>(new FriendOfFriendQueryCache(3600));

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

void CacheKey::CacheKeyGenerate(int user_id, string query, string& cache_key)  {
    MD5 transform_md5;
    std::string start_name = "cache_";
    std::string final_key = "";
    char * id_mem = new char[12];

    transform_md5.itoa(user_id, id_mem);

    std::string str_id = (std::string) id_mem;
    final_key += str_id;
    final_key += query;

    transform_md5.reset();
    transform_md5.update(final_key);
    cache_key = start_name + transform_md5.toString();
}


UpdateCache::UpdateCache(int user_id, string query, int total, IndexResultPtr friend_info_list, bool cache_db_right):MyUtil::Task(kTaskLevelUpdateCacheTask),user_id_(user_id),query_(query),total_(total),friend_info_list_(friend_info_list),cache_db_right_(cache_db_right) { }

void UpdateCache::handle()  {
  try {
    IceUtil::RWRecMutex::WLock lock(write_cache_lock_);
    string cache_key;
    int result_size = friend_info_list_ -> contents.size();
    MCE_DEBUG("UpdateCache the friend_info_list size is "<<result_size);
    CacheKeyGenerate(user_id_, query_, cache_key);
    if(result_size > 0)  {
      UserData* arr_users = new UserData[result_size];
      for(int i = 0; i < result_size; i++)  {
        arr_users[i].id_ = friend_info_list_->contents.at(i).id;
        arr_users[i].type_ = atoi((friend_info_list_->contents.at(i).content[FRIENDS_TYPE]).c_str());
        arr_users[i].mutual_ = atoi((friend_info_list_->contents.at(i).content[COMMON_FRIENDS_NUM]).c_str()); 
      }
      FriendOfFriendQueryCache::Cache result_cache(total_, arr_users, result_size);
      if(cache_db_right_)  {
        if (!querycache_->SetCache(cache_key.c_str(), result_cache)) {
          MCE_DEBUG("Set cache error !");
          return;
        }
        MCE_DEBUG("Set search result cache success!");
      }
    }

  }  catch (IceUtil::Exception& e) {
    MCE_WARN("UpdateCache Exception " << e.what());
  }  catch (...) {
    MCE_WARN("UpdateCache unknown Exception ");
  }

}

InvokeMemIndexer::InvokeMemIndexer(int user_id, string &query_string, IndexResultPtr result, TimeLogPtr time, TaskReferencePtr finished, int begin, int limit):MyUtil::Task(kTaskLevelGeneralSearchTask), user_id_(user_id), query_string_(query_string), result_(result), time_(time), finished_(finished), begin_(begin), limit_(limit) { }

void  InvokeMemIndexer::handle()  {
  struct timeval start, end;
  if(FriendOfFriendSearcherManagerI::instance().GetDataFromMem(user_id_, query_string_, result_))  {
    MCE_DEBUG("get data from mem!");
  } else { 
    gettimeofday(&start, NULL); 
    if (FriendOfFriendIndexerAdapter::instance().BuildIndex(user_id_)) {
      if(FriendOfFriendSearcherManagerI::instance().GetDataFromMem(user_id_, query_string_, result_))  {
        MCE_DEBUG("get data from mem now!");
      }
    }
    gettimeofday(&end, NULL);
    time_->IndexerCost = ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec))/1000;
  }
  finished_->Set(true);
  finished_->Post();
}




FriendOfFriendSearcherManagerI::FriendOfFriendSearcherManagerI() : db_(true) {

}

/// @brief FriendOfFriendSearcherManagerI::bootstrap
/// 与业务逻辑相关的初始化
void FriendOfFriendSearcherManagerI::bootstrap() {
  ServiceI& service = ServiceI::instance();
  string family_name_dict, not_regular_name_dict, memcache_db, searchcache_db;

  cluster_ = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendOfFriendSearcher.Cluster");
  mod_ = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendOfFriendSearcher.Mod");
  family_name_dict = service.getCommunicator()->getProperties()->getProperty("FriendOfFriendSearcher.Dict1");
  not_regular_name_dict = service.getCommunicator()->getProperties()->getProperty("FriendOfFriendSearcher.Dict2");
  db_right_ = true;
  query_cache_right_ = true;
  memcache_db = service.getCommunicator()->getProperties()->getProperty("FriendOfFriendSearcher.MemCacheDb");
  searchcache_db = service.getCommunicator()->getProperties()->getProperty("FriendOfFriendSearcher.SearchCacheDb");

  if(!db_.SetServers(memcache_db.c_str())) {
    db_right_ = false;
    MCE_INFO("SetServers() failed.");
  }
  if(!querycache_ ->SetServers(searchcache_db.c_str()))  {
    query_cache_right_ = false;
    MCE_INFO("querycahce SetServers() failed.");
  } else {
    MCE_INFO("querycahce SetServers() OK.");
  }

  ifstream input_file1(family_name_dict.c_str());
  ifstream input_file2(not_regular_name_dict.c_str());
  string temp;
  while(!input_file1.eof())  {
    getline(input_file1, temp, '\n');
    family_names_in_set_.insert(temp);
  }
  while(!input_file2.eof())  {
    getline(input_file2, temp, '\n');
    not_regular_names_in_set_.insert(temp);
  }
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

bool FriendOfFriendSearcherManagerI::IsName(const std::string & query)  {
  if(query.empty())
    return false;
  else {
    size_t index = query.find(" ");
    if(index >= 0 && index < query.length())
      return false;
    if(((query[0] & 0x80) == 0x00) && ((query[0] >= 'a' && query[0] <= 'z') || (query[0] >= 'A' && query[0] <= 'Z')))
      return true;
    if(query.length() >= 1 && query.length() < 3)
      return false;
    string first_letter = query.substr(0,3);
    if(family_names_in_set_.find(first_letter) != family_names_in_set_.end())  {
      MCE_DEBUG("Query:"<<query<<" is family name!");
      return true;
    } 
    else  {
      if(not_regular_names_in_set_.find(first_letter) != not_regular_names_in_set_.end())  {
        MCE_DEBUG("Query:"<<query<<" is name but not regular!");
        return true;   
      }
    }  
  }
  return false;
}

bool FriendOfFriendSearcherManagerI::GetDataFromMem(const int user_id, std::string& name,
    IndexResultPtr friendinfo_list)  {
  FriendOfFriendMap::Result friendinfo_result;
  IndexResultItem friend_item;
  FriendOfFriendMap mem_map;

  if (!db_right_)  {
    return false;
  }

  if(!db_.GetMap(user_id, mem_map))  {
    MCE_INFO("GetMap() failed!");
    return false;
  }

  if(mem_map.Find(name.c_str(),friendinfo_result))  {
    char friends_type[10];
    char common_friends[10];

    MCE_DEBUG("MEM find it!");
    friendinfo_list->totalCount = friendinfo_result.GetCount();
    friendinfo_list->typedCount[User] = friendinfo_result.GetCount();
    for(size_t i = 0; i < friendinfo_result.GetCount(); i++)  {
        const UserData *ptr_item = friendinfo_result.GetItems() + i;

        friend_item.id = ptr_item -> GetID();
        friend_item.type = User;
        itoa(ptr_item -> GetType(), friends_type);
        friend_item.content[FRIENDS_TYPE] = (string) friends_type;
        itoa(ptr_item -> GetMutual(), common_friends);
        friend_item.content[COMMON_FRIENDS_NUM] =  (string) common_friends;
        if(friend_item.content[FRIENDS_TYPE] == "0" || friend_item.content[FRIENDS_TYPE] == "1")  {
          friend_item.content[IS_FRIEND] = "1";
        } else  {
          friend_item.content[IS_FRIEND] = "0";
        }

        friendinfo_list->contents.push_back(friend_item);        

        MCE_INFO("GET id:"<< friend_item.id<< " type:"<< friend_item.content[FRIENDS_TYPE]<<" common friends:"<< friend_item.content[COMMON_FRIENDS_NUM]);
    }
    return true;
  }
  MCE_DEBUG("EXIT MEM");
  return true;
}

int FriendOfFriendSearcherManagerI::itoa(int i,char*string) {
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

IndexResultPtr FriendOfFriendSearcherManagerI::searchIndex(const IndexCondition& query, int begin, 
                                            int limit, const Ice::Current&)  {
     struct timeval total_start, total_end;
     TimeLogPtr log_duration(new timeLog);
     int duration, total_count;
     IndexResultPtr  out_result;
     string query_string = const_cast<IndexCondition&>(query).queryCondition[QUERY_QUERY];
     int userid = 0;
     log_duration->IndexerCost = 0;
     log_duration->CombineCost = 0;

     gettimeofday(&total_start, NULL);

     if (query.opUserCondition.find(ID_FIELD) != query.opUserCondition.end()) {
       userid = atoi((const_cast<IndexCondition&>(query).opUserCondition[ID_FIELD]).c_str());
     }
     MCE_INFO("The userid is " << userid << " query:" << query_string);
     try  { 
       if (IsOnlyQuery(query) && IsName(query_string)) {
         IndexResultPtr friendinfo_list = new IndexResult();
         TaskReferencePtr finished(new TaskReference(false));
         bool return_right;

         TaskManager::instance().execute(new InvokeMemIndexer(userid, query_string, friendinfo_list, log_duration, finished));

         IndexResultPtr original_result = GeneralSearch(userid, query, log_duration);
         return_right = finished->Wait(log_duration);
         if(!return_right)  {
           MCE_INFO("Time out when waiting for Indexer!");
         }
         out_result = new IndexResult();
         out_result->currentPos = 0;
         out_result->totalCount = 0;
         total_count = CombineResults(query_string, friendinfo_list, original_result,
             out_result, log_duration, begin, limit);
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
     MCE_INFO("Time taken in Searcher is : "<<duration<<" ms");
     out_result->timeCost = duration;
     MCE_INFO("The final combined results are below: ");
     MCE_INFO("currentPos:"<<out_result->currentPos<<" totalCount:"<<out_result->totalCount<<" timeCost:"
         <<out_result->timeCost<<" searchTypeCount:"<<out_result->typedCount[User];)
     MCE_INFO("Indexer cost: "<<log_duration->IndexerCost<<" ms   General cost: "
               <<log_duration->GeneralSearchCost<<" ms   Combine cost: "<<log_duration->CombineCost<<" ms");
     return out_result;
}

IndexResultPtr FriendOfFriendSearcherManagerI::GeneralSearch(const int user_id, IndexCondition query, TimeLogPtr time, int begin, int limit)  {
  if (query.type == OpenSearch) {
    IndexResultPtr result = SearchManagerAdapter::instance().SearchIndex(query, begin , limit);
    time->GeneralSearchCost = result->timeCost;
    return result;
  } else if (query.type == OpenPeopleSearch) {
    IndexResultPtr result = PeopleSearchManagerAdapter::instance().SearchIndex(query, begin , limit);
    time->GeneralSearchCost = result->timeCost;
    return result;
  } else {
    return new IndexResult;
  }
}

int FriendOfFriendSearcherManagerI::CombineResults(const std::string &query, IndexResultPtr fof_result,
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
  if (general_size + fof_size == 0 || begin >= general_size + fof_size) {
    gettimeofday(&total_end, NULL);
    duration = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start.tv_sec*1000000 + total_start.tv_usec))/1000;
    time -> CombineCost = duration;
    return 0;
  }

  std::set<int>  friends_id_info;
  for (int i = 0; i < fof_size; ++i) {
    IndexResultItem &item = fof_result->contents.at(i);
    friends_id_info.insert(item.id);
  }

  result_begin = begin;
  result_left = limit;

  if (general_size > 0) {
    int pos = 0;

    for (general_pos = 0; general_pos < general_size && pos < result_begin + result_left; ++general_pos) {
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

  if (result_begin < fof_size) {
    int end = result_begin + result_left;
    end = (end > fof_size ? fof_size : end);
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
  if (result_begin < general_size) {
    int count = 0;
    for (int i = result_begin; count < result_left && i < general_size; ++i) {
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
