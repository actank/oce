/*
 * FeedInteractionI.cpp
 *
 *  Created on: 2010-4-22
 *      Author: ld
 */

#include <sstream>
#include "client/linux/handler/exception_handler.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "FeedInteractionI.h"
#include "IceUtil/RWRecMutex.h"
#include "IceLogger.h"
#include "PerformanceStat.h"
#include "DbHelper.h"
#include "FeedInteractionNAdapter.h"

using namespace xce::feed;
using namespace com::xiaonei::xce;


const string DbHelper::kDbTableName = "feed_interaction";
const string DbHelper::kDbLogicName = "feed_db";
const string DbHelper::kColFeedId = "feed";
const string DbHelper::kColReplyCount = "reply_count";
const string DbHelper::kColClickCount = "click_count";
const string DbHelper::kColViewCount = "view_count";
const string DbHelper::kColRange = "range";
const string DbHelper::kColActor = "actor";
const string DbHelper::kColTime = "time";

bool MyCallback(const char *dump_path,
   const char *minidump_id,
   void *context,
   bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}
void MyUtil::initialize() {
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback
                                       , 0, true);
	ServiceI& service = ServiceI::instance();
	FeedInteractionCache::instance().Init();
  service.getAdapter()->add(&FeedInteractionI::instance(), service.createIdentity("M", ""));

	TaskManager::instance().scheduleRepeated(&ClearSaveDbVectorTimer::instance());
	TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());
	TaskManager::instance().scheduleRepeated(&UpdateTimer::instance());
}




namespace xce {

namespace feed {

FeedInteractionI::FeedInteractionI() {
	// TODO Auto-generated constructor stub

}

FeedInteractionI::~FeedInteractionI() {
	// TODO Auto-generated destructor stub
}

void ClearSaveDbVectorTimer::handle() {
  MCE_INFO("ClearSaveDbVectorTimer::handle");
	FeedInteractionI::instance().ClearDirtyCache();
}

void FeedInteractionCache::Init(){
  for(int i=0; i<kCacheSize; i++){
    cache_array_[i].feed_ = 0;
  }
}
void FeedInteractionCache::InsertFeedInteraction(const FeedInteractionInfoPtr& interaction_info){
  MCE_DEBUG("FeedInteractionCache::InsertFeedInteraction");

	Ice::Long feed = interaction_info->feed;
	InteractionCacheItem cache_item;
	cache_item.feed_ = interaction_info->feed;
	cache_item.range_ = interaction_info->range;
	cache_item.count_ = 0;

  FeedInteractionDbItem db_item;
  db_item.feed_ = interaction_info->feed;
  db_item.reply_count_ = 0;
  db_item.click_count_ = 0;
  db_item.view_count_ = 0;
  db_item.range_ = interaction_info->range;
  db_item.actor_ = interaction_info->actor;
  db_item.time_ = interaction_info->time;
 
  //int cache_index = feed % kCacheSize;
  if(cache_array_[GetCacheIndex(feed)].feed_ == feed){
    return;
  }
  InteractionCacheItem tmp = QueryCacheItemFromDb(feed);
  if(tmp.feed_ == feed){
    return;
  }
  
  {
    if(cache_array_[GetCacheIndex(feed)].feed_>0 && cache_array_[GetCacheIndex(feed)].feed_!=cache_item.feed_){
      //PerformanceStat::instance().stat("ReplacedInsert", 0);
    }
    IceUtil::RWRecMutex::WLock lock(cache_rw_mutex_[GetMutexIndex(cache_item.feed_)]);
    cache_array_[GetCacheIndex(feed)] = cache_item;
    MCE_INFO("FeedInteractionCache::InsertFeedInteraction -->cache_array  index:" << GetCacheIndex(feed) 
    << " feed:" << cache_array_[GetCacheIndex(feed)].feed_ << " actor:" << interaction_info->actor << " range:" << cache_array_[GetCacheIndex(feed)].range_
    << " count:" << cache_array_[GetCacheIndex(feed)].count_);
  }

  {
     IceUtil::Mutex::Lock lock(dirty_mutex_);
     dirty_cache_items_[feed%DbHelper::kTableCount][feed] = db_item;
     //MCE_INFO("FeedInteractionCache::InsertFeedInteraction -->dirty_cache "
     //    << " feed:" << feed << " reply_count:" << dirty_cache_items_[feed].reply_count_
     //    << " click_count:" <<  dirty_cache_items_[feed].click_count_
     //    << " range:" << dirty_cache_items_[feed].range_
     //    << " actor:" <<  dirty_cache_items_[feed].actor_
     //    << " time:" <<  dirty_cache_items_[feed].time_);
  }
}


bool FeedInteractionCache::IsHitted(Ice::Long feed) {
	
  bool is_hitted = false;
	//int cache_index = feed % kCacheSize;
  //MCE_INFO("@@@@ " << feed << " " << cache_index);
	if (feed == cache_array_[GetCacheIndex(feed)].feed_) {
		is_hitted = true;
	}
	return is_hitted;
}


pair<bool, InteractionCacheItem> FeedInteractionCache::FindInDirtyCache(Ice::Long feed) {

  IceUtil::Mutex::Lock lock(dirty_mutex_);
  pair<bool, InteractionCacheItem> rt;
  InteractionCacheItem cache_item;

  if (dirty_cache_items_[feed%DbHelper::kTableCount].count(feed)) {
    //MCE_INFO("FeedInteractionCache::FindInDirtyCache --> find feed in dirty cache:");

    map<Ice::Long,FeedInteractionDbItem>::iterator it = dirty_cache_items_[feed%DbHelper::kTableCount].find(feed);
    FeedInteractionDbItem db_item = it->second;

    cache_item.feed_ = db_item.feed_;
    cache_item.range_ = db_item.range_;
    cache_item.count_ = (db_item.reply_count_ << 16) + db_item.click_count_;

    //MCE_INFO("FeedInteractionCache::FindInDirtyCache --> feed_:" << cache_item.feed_
    //    << " range:" << cache_item.range_ << " reply_count:" << db_item.reply_count_
    //    << " click_count:" << db_item.click_count_);
    
    rt.first = true;
    rt.second = cache_item; 
    

  } else {
    rt.first = false;
   
  }

  return rt;

}

InteractionCacheItem FeedInteractionCache::QueryCacheItemFromDb(Ice::Long feed){
  MCE_DEBUG("FeedInteractionCache::QueryCacheItemFromDb");
  InteractionCacheItem cache_item;
  pair<bool, InteractionCacheItem> rt = FindInDirtyCache(feed);
  if (rt.first) {
    cache_item = rt.second;    
    return cache_item;  
  }
	Statement sql;
	sql << "SELECT * FROM " << DbHelper::GetTableName(feed) << " WHERE " << DbHelper::kColFeedId
			<< " = " << feed;
	mysqlpp::StoreQueryResult res;
	try {
    TimeStat ts;
    res = QueryRunner(DbHelper::kDbLogicName, CDbRServer, DbHelper::GetExpr(feed)).store(sql);
    //PerformanceStat::instance().stat("QueryDB", ts.getTime()); 
	} catch (const Ice::Exception& e) {
		MCE_WARN("FeedInteractionCache::QueryCacheItemFromDb --> sql ice exception:" << e);
	} catch (const std::exception& e) {
		MCE_WARN("FeedInteractionCache::QueryCacheItemFromDb --> sql exception:" << e.what());
	}

	if (!res) {
		MCE_WARN("FeedInteractionCache::QueryCacheItemFromDb --> res null:" << feed);
		return cache_item;
	}
  
  if (res.num_rows()>0) {
    mysqlpp::Row& row = res.at(0);
    cache_item.feed_ = row[DbHelper::kColFeedId.c_str()];
    int reply_count = row[DbHelper::kColReplyCount.c_str()];
    int click_count = row[DbHelper::kColClickCount.c_str()];
    int view_count = row[DbHelper::kColViewCount.c_str()];
    cache_item.count_ = (((Ice::Long)view_count)<<32) | (((Ice::Long)reply_count)<<16) | click_count;
    cache_item.range_ = row[DbHelper::kColRange.c_str()];
    //MCE_INFO("FeedInteractionCache::QueryCacheItemFromDb --> feed_:" << cache_item.feed_
    //  << " range:" << cache_item.range_ << " reply_count:" << reply_count << " click_count:" << click_count);
  } else {
    //MCE_WARN("FeedInteractionCache::QueryCacheItemFromDb --> can not find feed:" << feed <<" in db!");
  }
	return cache_item;
}

vector<InteractionCacheItem> FeedInteractionCache::QueryCacheItemFromDb(const MyUtil::LongSeq& feeds){
  MCE_DEBUG("FeedInteractionCache::QueryCacheItemFromDb");
  vector<InteractionCacheItem> res;
  map<int, MyUtil::LongSeq> feeds_in_db;
  TimeStat ts;
  TimeStat ts1;
  int count = 0;
  for(size_t i=0; i<feeds.size(); i++){
    pair<bool, InteractionCacheItem> rt = FindInDirtyCache(feeds.at(i));
    if (rt.first) {
      res.push_back(rt.second);
    }else{
      feeds_in_db[feeds.at(i)%DbHelper::kTableCount].push_back(feeds.at(i));
      count++;
    }
  }
  //PerformanceStat::instance().stat("QueryInDirtyCache", ts.getTime());
  if(feeds_in_db.empty()){
    return res;
  }
  ts.reset();
  MyUtil::ObjectResultPtr result;
  ParallelLoaderPtr loader = new ParallelLoader;
  for(map<int, MyUtil::LongSeq>::iterator it = feeds_in_db.begin(); it!=feeds_in_db.end(); ++it){ 
    ostringstream sql;
	  sql << "SELECT * FROM " << DbHelper::GetTableName(it->first) << " WHERE " << DbHelper::kColFeedId << " IN (";
	  for(size_t i=0; i<it->second.size(); i++){
      if(i!=0){
        sql << ", ";
      }
      sql << it->second.at(i);
    }
    sql << ")";
    loader->addQuery(DbHelper::kDbLogicName, sql.str(), DbHelper::GetExpr(it->first));
  }
  //PerformanceStat::instance().stat("QueryGenSql", ts.getTime());
  
  ts.reset();
  loader->query(result);
  //PerformanceStat::instance().stat("QueryDBBatch", count); 
  ts.reset();
  for (MyUtil::ObjectMap::iterator it = result->data.begin(); it != result->data.end(); ++it) {
    InteractionCacheItemObjectPtr obj = InteractionCacheItemObjectPtr::dynamicCast(it->second);
    res.push_back(obj->item);
  }	
  //PerformanceStat::instance().stat("QueryCast", ts.getTime());
  //PerformanceStat::instance().stat("QueryAll", ts1.getTime());
	return res;
}

InteractionCacheItem FeedInteractionCache::UpdateCache(int cache_index, int type) {
  MCE_DEBUG("FeedInteractionCache::UpdateCache");
  if (type == 1) {
    cache_array_[cache_index].count_ ++;
  } else if(type == 2) {
    cache_array_[cache_index].count_ += kReplyCountStep;
  } else if(type == 3){
    cache_array_[cache_index].count_ += kViewCountStep;
  }
  
  //int feed = cache_array_[cache_index].feed_;
  //int count = cache_array_[cache_index].count_;
  /////////////////dirty_cache_items_[feed] = count;

  //int click_count = count & 0xffff;
  //int reply_count = count >>16;



  //MCE_INFO("FeedInteractionCache::UpdateCache --> cache_index:" << cache_index 
  //    << "feed_:" << cache_array_[cache_index].feed_   
  //    << " range:" << cache_array_[cache_index].range_
  //    << " reply_count:" << reply_count << " click_count:" <<click_count
  //    << " dirty_cache_items_[" << feed << "]=" << count);
  
  return cache_array_[cache_index];
}

bool FeedInteractionCache::UpdateFeedInteraction(Ice::Long feed, int type, bool load) {
  MCE_DEBUG("FeedInteractionCache::UpdateFeedInteraction");
  InteractionCacheItem cache_item;
  //int cache_index = feed % kCacheSize;
  bool is_hitted = false;
  bool updated = false;
	{
     IceUtil::RWRecMutex::WLock lock(cache_rw_mutex_[GetMutexIndex(feed)]);
     is_hitted = IsHitted(feed);
     if (is_hitted) {
       cache_item = UpdateCache(GetCacheIndex(feed), type);
       updated = true;
     }
  }
 
  if (!is_hitted && load) {
    cache_item = QueryCacheItemFromDb(feed);
    if (-1 == cache_item.feed_) {
      //MCE_WARN("FeedInteractionCache::UpdateFeedInteraction --> cache_item.feed_ == -1");
      return true;
    }
    
    if(cache_array_[GetCacheIndex(feed)].feed_>0 && cache_array_[GetCacheIndex(feed)].feed_!=cache_item.feed_){
      //PerformanceStat::instance().stat("ReplacedUpdate", 0);
    }
    IceUtil::RWRecMutex::WLock lock(cache_rw_mutex_[GetMutexIndex(feed)]);
    if ( cache_array_[GetCacheIndex(feed)].feed_ != cache_item.feed_) {
      cache_array_[GetCacheIndex(feed)] = cache_item;
    } 
    cache_item = UpdateCache(GetCacheIndex(feed), type);
    updated = true; 
  }
  if(updated){
     IceUtil::Mutex::Lock lock(dirty_mutex_);
     FeedInteractionDbItem db_item;
     db_item.feed_ = cache_item.feed_;
     db_item.click_count_ = cache_item.count_ & 0xffff;
     db_item.reply_count_ = (cache_item.count_ >> 16) & 0xffff;
     db_item.view_count_ = cache_item.count_ >> 32;
     if (dirty_cache_items_[feed%DbHelper::kTableCount].count(feed)) {
       
       if(dirty_cache_items_[feed%DbHelper::kTableCount][feed].reply_count_ < db_item.reply_count_) {
         dirty_cache_items_[feed%DbHelper::kTableCount][feed].reply_count_ = db_item.reply_count_;
       }
       
       if( dirty_cache_items_[feed%DbHelper::kTableCount][feed].click_count_ <  db_item.click_count_) {
         dirty_cache_items_[feed%DbHelper::kTableCount][feed].click_count_ = db_item.click_count_;
       }
      
       if( dirty_cache_items_[feed%DbHelper::kTableCount][feed].view_count_ <  db_item.view_count_) {
         dirty_cache_items_[feed%DbHelper::kTableCount][feed].view_count_ = db_item.view_count_;
       }
     } else {
       dirty_cache_items_[feed%DbHelper::kTableCount][feed] = db_item;

     }
     //MCE_INFO("FeedInteractionCache::UpdateFeedInteraction --> insert dirty cache"
     //    << "feed_:" << db_item.feed_
     // << " range:" << db_item.range_
     // << " reply_count:" << db_item.reply_count_ << " click_count:" <<db_item.click_count_ << " view_count:" << db_item.view_count_
     // << " actor:" << db_item.actor_ << " time:" << db_item.time_);
 
  }
  return updated;
}


map<Ice::Long, FeedInteractionDbItem>::iterator
    FeedInteractionCache::SaveDb(const map<Ice::Long, FeedInteractionDbItem>& temp_map,
    map<Ice::Long, FeedInteractionDbItem>::iterator it) {

  MCE_INFO("FeedInteractionCache::SaveDb");

  Statement sql;

  ostringstream os;


  string table_name = DbHelper::GetTableName(it->first);
  string expr = DbHelper::GetExpr(it->first);
  //MCE_INFO("@@@@ --> expr=" << expr);
  sql << "insert into " << table_name << " (" << DbHelper::kColFeedId
      << "," << DbHelper::kColReplyCount << "," << DbHelper::kColClickCount << ","
      << DbHelper::kColViewCount << "," << DbHelper::kColRange <<  "," << DbHelper::kColActor
      << "," << DbHelper::kColTime << ") values ";

  //os << "insert into " << table_name << " (" << DbHelper::kColFeedId
  //    << "," << DbHelper::kColReplyCount << "," << DbHelper::kColClickCount << ","
  //    << DbHelper::kColRange <<  "," << DbHelper::kColActor
  //    << "," << DbHelper::kColTime << ") values ";
  
  for(int i=0; i<1000 && it != temp_map.end(); i++,it++) {

    if (i) {
      sql << ",";
      //os << ",";
    }

    sql << "(" << it->first << ","
        << it->second.reply_count_ << "," << it->second.click_count_ << "," << it->second.view_count_ << ","
        << it->second.range_ << ","  << it->second.actor_ << ","
        << it->second.time_ << ")";

    //os << "(" << it->first << ","
    //    << it->second.reply_count_ << "," << it->second.click_count_ << ","
    //    << it->second.range_ << ","  << it->second.actor_ << ","
    //    << it->second.time_ << ")";
    
  }

  sql << " on duplicate key update " << DbHelper::kColReplyCount << "=values("
      << DbHelper::kColReplyCount << ")," << DbHelper::kColClickCount << "=values("
      << DbHelper::kColClickCount << ")," << DbHelper::kColViewCount << "=values(" << DbHelper::kColViewCount << ")";
  //os << " on duplicate key update " << DbHelper::kColReplyCount << "=values("
  //    << DbHelper::kColReplyCount << ")," << DbHelper::kColClickCount << "=values("
  //    << DbHelper::kColClickCount << ")";

  //MCE_INFO("FeedInteractionCache::SaveDb --> sql:" << os.str());
  try {
    QueryRunner(DbHelper::kDbLogicName, CDbWServer, expr).schedule(sql);
  } catch (const Ice::Exception& e) {
    MCE_INFO("FeedInteractionCache::SaveDb --> sql ice exception:" << e);
  } catch (const std::exception& e) {
    MCE_INFO("FeedInteractionCache::SaveDb --> sql exception:" << e.what());
  }
  
  return it;


}


void FeedInteractionCache::ClearDirtyCache() {
  map<int, map<Ice::Long, FeedInteractionDbItem> > temp_map;
	{
    IceUtil::Mutex::Lock lock(dirty_mutex_);
		dirty_cache_items_.swap(temp_map);
	}

  if (!temp_map.size()){
   return ;
  }
  MCE_INFO("FeedInteractionCache::ClearDirtyCache --> size " << temp_map.size());

  map<int, map<Ice::Long, FeedInteractionDbItem> >::iterator index_it = temp_map.begin();

  for(; index_it!=temp_map.end(); ++index_it){
    map<Ice::Long, FeedInteractionDbItem>::iterator it = index_it->second.begin();
    while (it != index_it->second.end()) {
      it = SaveDb(index_it->second, it);
    }
  }
}




InteractionCacheItem FeedInteractionCache::FindFeedInteraction(Ice::Long feed) {
  MCE_DEBUG("FeedInteractionCache::FindFeedInteraction");
  //int cache_index = feed % kCacheSize;  
  InteractionCacheItem cache_item;
  {  	
     IceUtil::RWRecMutex::RLock lock(cache_rw_mutex_[GetMutexIndex(feed)]);
     bool is_hitted = IsHitted(feed);
     if (is_hitted) {
       cache_item = cache_array_[GetCacheIndex(feed)];
       //int temp_count = cache_item.count_;
       //int click_count = temp_count & 0xffff;
       //int reply_count = temp_count >>16;
       //MCE_INFO("FeedInteractionCache::FindFeedInteraction --> hit cache.   cache_index:" 
       //    << cache_index << " feed_:" << cache_item.feed_ 
       //    << " range:" << cache_item.range_ << " reply_count:" << reply_count
       //    << " click:" << click_count);
       return cache_item;
     }
  }
	
  {
     cache_item = QueryCacheItemFromDb(feed);
		 if(cache_item.feed_ < 0){
       return cache_item;
     }
     if(cache_array_[GetCacheIndex(feed)].feed_>0 && cache_array_[GetCacheIndex(feed)].feed_!=cache_item.feed_){
      MCE_INFO("@@@@ --> " << cache_array_[GetCacheIndex(feed)].feed_ << " " << cache_item.feed_);
      //PerformanceStat::instance().stat("ReplacedFind", 0);
     }
     TimeStat ts;
     IceUtil::RWRecMutex::WLock lock(cache_rw_mutex_[GetMutexIndex(cache_item.feed_)]);
     //PerformanceStat::instance().stat("Lock", ts.getTime());
     cache_array_[GetCacheIndex(feed)] = cache_item;
     int temp_count = cache_item.count_;
     int click_count = temp_count & 0xffff;
     int reply_count = temp_count >>16;
     //MCE_INFO("FeedInteractionCache::FindFeedInteraction --> not  hit.   cache_index:"
     //    << cache_index << " feed_:" << cache_item.feed_
     //    << " range:" << cache_item.range_  << " reply_count:" << reply_count
     //    << " click:" << click_count);
	}
	return cache_item;
}

vector<InteractionCacheItem> FeedInteractionCache::FindFeedInteraction(const MyUtil::LongSeq feeds) {
  vector<InteractionCacheItem> res;
  MyUtil::LongSeq feeds_in_db;
  TimeStat ts;
  for(size_t i=0; i<feeds.size(); i++){
    int cache_index = GetCacheIndex(feeds.at(i));  
    InteractionCacheItem cache_item;
    IceUtil::RWRecMutex::RLock lock(cache_rw_mutex_[GetMutexIndex(feeds.at(i))]);
    bool is_hitted = IsHitted(feeds.at(i));
    if (is_hitted) {
      cache_item = cache_array_[cache_index];
      res.push_back(cache_item);
    }else{
      feeds_in_db.push_back(feeds.at(i));
    }
  }
  //PerformanceStat::instance().stat("FindInMem", ts.getTime());

  ts.reset();
  vector<InteractionCacheItem> resdb = QueryCacheItemFromDb(feeds_in_db);
  //PerformanceStat::instance().stat("FindInDb", ts.getTime());
  //res.insert(res.end(), resdb.begin(), resdb.end());
  
  ts.reset();
  for(size_t i=0; i<resdb.size(); i++){
    if(resdb.at(i).feed_ < 0){
      continue;
    }
	  res.push_back(resdb.at(i));
    int cache_index = GetCacheIndex(resdb.at(i).feed_);
    if(cache_array_[cache_index].feed_>0 && cache_array_[cache_index].feed_!=resdb.at(i).feed_){
      MCE_INFO("@@@@ --> " << cache_array_[cache_index].feed_ << " " << resdb.at(i).feed_);
      //PerformanceStat::instance().stat("ReplacedFind", 0);
    }
    TimeStat ts;
    IceUtil::RWRecMutex::WLock lock(cache_rw_mutex_[GetMutexIndex(resdb.at(i).feed_)]);
    //PerformanceStat::instance().stat("Lock", ts.getTime());
	  cache_array_[cache_index] = resdb.at(i);
  }
  //PerformanceStat::instance().stat("FindUpMem", ts.getTime());
  return res;
}


/* bool SaveDbMap::Count(Ice::Long feed) {

  IceUtil::Mutex::Lock lock(mutex_);

  return save_db_map_.count(feed);
} */



/* void SaveDbMap::InsertNewFeedInteraction(const FeedInteractionDbItem& db_item) {
	IceUtil::Mutex::Lock lock(mutex_);
  save_db_map_.insert(make_pair(db_item.feed_, db_item));
//	save_db_vector_.push_back(db_item);
} */

/* void SaveDbMap::ClearMap() {
  MCE_DEBUG("SaveDbMap::ClearMap");

	map<Ice::Long, FeedInteractionDbItem> temp_map;
	bool is_empty = true;
	{
		IceUtil::Mutex::Lock lock(mutex_);

		if (!save_db_map_.empty()) {
			is_empty = false;
			save_db_map_.swap(temp_map);

		}
	}

	Statement sql;
  ostringstream os;
	if (!is_empty) {

		sql << "insert into " << kDbTableName << " (" << kColFeedId
				<< "," << kColReplyCount << "," << kColClickCount << "," 
        << kColRange <<  "," << kColActor
				<< "," << kColTime << ") values ";

    os << "insert into " << kDbTableName << " (" << kColFeedId
        << "," << kColReplyCount << "," << kColClickCount << ","  
        << kColRange <<  "," << kColActor
        << "," << kColTime << ") values ";


		for (map<Ice::Long, FeedInteractionDbItem>::iterator it =
				temp_map.begin(); it != temp_map.end(); ++it) {
			if (it != temp_map.begin()) {
				sql << ",";
        os << ",";
			}
  
      //int temp_count = it->count;
      int reply_count = 0;
      int click_count = 0;


			sql << "(" << it->first << ","
					<< reply_count << "," << click_count << "," 
          << it->second.range_ << ","	 << it->second.actor_ << ","
					<< it->second.time_ << ")";
  
      os << "(" << it->first << ","
          << reply_count << "," << click_count << ","  
          << it->second.range_ << ","  << it->second.actor_ << ","
          << it->second.time_ << ")";


		}

    MCE_DEBUG("SaveDbMap::ClearMap --> sql:" << os.str());


		try {
		  QueryRunner(kDbLogicName, CDbWServer).store(sql);
		} catch (const Ice::Exception& e) {
			MCE_INFO("SaveDbVector::ClearVector --> sql ice exception:" << e);
		} catch (const std::exception& e) {
			MCE_INFO("SaveDbVector::ClearVector --> sql exception:" << e.what());
		}
	}

}  */

void FeedInteractionI::AddFeedInteraction(long feed, int actor, int range, int time, const ::Ice::Current&){
  return;
  PerformanceStat::instance().stat("AddInteraction", 0);
  FeedInteractionInfoPtr tmp = new FeedInteractionInfo;
  tmp->feed = feed;
  tmp->actor = actor;
  tmp->range = range;
  tmp->time = time;
  FeedInteractionCache::instance().InsertFeedInteraction(tmp); 
  //feed_interaction_cache_.InsertFeedInteraction(tmp);
}
/*
bool FeedInteractionI::InsertFeedInteraction(const FeedInteractionInfoPtr& interaction_info,
		const ::Ice::Current&) {
	
  MCE_INFO("FeedInteractionI::InsertFeedInteraction --> interaction_info.feed:" << interaction_info->feed
      << " range:" << interaction_info->range 
      << " actor:" << interaction_info->actor 
      << " time:" << interaction_info->time);

  feed_interaction_cache_.InsertFeedInteraction(interaction_info);
//	FeedInteractionDbItem db_item;
//	db_item.feed_ = interaction_info->feed;
	//db_item.range_ = interaction_info->range;
	//db_item.actor_ = interaction_info->actor;
//	db_item.time_ = interaction_info->time;
//	db_item.reply_count_ = 0;
 // db_item.click_count_ = 0;


	//save_db_map_.InsertNewFeedInteraction(db_item);
  return true;
}
*/
void FeedInteractionI::Inc(int type, Ice::Long feed, int count, const ::Ice::Current&){
  //PerformanceStat::instance().stat("Inc", 0);
  return;
  bool res = FeedInteractionCache::instance().UpdateFeedInteraction(feed, type);
  if(!res){
    if(type == 1){
      UpdateTimer::instance().PushClick(feed);
    }else if(type == 2){
      UpdateTimer::instance().PushReply(feed);
    }
  }
}

void FeedInteractionI::IncBatch(int type, const MyUtil::Long2IntMap& feedCounts, const ::Ice::Current&){
  //PerformanceStat::instance().stat("IncBatch", 0);
	return;
  MyUtil::Long2IntMap::const_iterator it = feedCounts.begin();
  for(; it!=feedCounts.end(); ++it){
    Inc(type, it->first, it->second);
    //FeedInteractionCache::instance().UpdateFeedInteraction(it->first, type);
  }
}

FeedInteractionCountPtr FeedInteractionI::GetFeedInteractionCount(long feed, bool load, const ::Ice::Current&) {
	return 0;
  MCE_DEBUG("FeedInteractionI::GetFeedInteractionCount --> feed:" << feed);
  InteractionCacheItem temp_item = FeedInteractionCache::instance().FindFeedInteraction(feed);
  
  FeedInteractionCountPtr interaction_ptr = new FeedInteractionCount;

  interaction_ptr->feed = temp_item.feed_;
  interaction_ptr->clickCount = temp_item.count_ & 0xffff;
  interaction_ptr->replyCount = temp_item.count_ >> 16;
  interaction_ptr->range = temp_item.range_;
  
  MCE_INFO("FeedInteractionI::GetFeedInteractionCount --> feed:" << interaction_ptr->feed
      << " clickCount:" << interaction_ptr->clickCount 
      << " replyCount:" << interaction_ptr->replyCount);

  return interaction_ptr;
}


FeedInteractionCountSeq FeedInteractionI::GetFeedInteractionCountSeq(const ::MyUtil::LongSeq& feeds_seq, bool load, const ::Ice::Current&) {
  return FeedInteractionCountSeq();
  FeedInteractionCountSeq feed_interaction_count_seq;
  TimeStat ts;
  vector<InteractionCacheItem> res = FeedInteractionCache::instance().FindFeedInteraction(feeds_seq);
  PerformanceStat::instance().stat("GetInteractionLoad", ts.getTime());
  ts.reset();
  for (size_t i = 0 ; i!= res.size(); i++) {
    //Ice::Long feed = feeds_seq.at(i);
    //InteractionCacheItem temp_item = FeedInteractionCache::instance().FindFeedInteraction(feed);
    FeedInteractionCountPtr feed_interaction_count = new FeedInteractionCount; 
    feed_interaction_count->feed = res.at(i).feed_;
    feed_interaction_count->clickCount = res.at(i).count_ & 0xffff;
    feed_interaction_count->replyCount = res.at(i).count_ >> 16;
    feed_interaction_count->range = res.at(i).range_;
    //MCE_INFO("FeedInteractionI::GetFeedInteractionCountSeq --> feed:" << feed_interaction_count->feed
    //  << " clickCount:" << feed_interaction_count->clickCount 
    //  << " replyCount:" << feed_interaction_count->replyCount);
    feed_interaction_count_seq.push_back(feed_interaction_count);
  }
  PerformanceStat::instance().stat("GetInteractionRes", ts.getTime());
  return feed_interaction_count_seq;
}

void FeedInteractionI::IncHitCount(const string& content, const ::Ice::Current&){
  PerformanceStat::instance().stat("Click", 0);
  const string dst = "NF_NEWSDEL_"; 
  int pos = content.find(dst);
  if(pos == string::npos){
    return;
  }
  int begin = pos + dst.length();
  int end = content.find("\"", begin);
  string tmp = content.substr(begin, end-begin);
  vector<string> splits;
  boost::split(splits, tmp, boost::is_any_of("_"), boost::token_compress_on);
  if(splits.empty()){
    return;
  }
  Ice::Long feedid = 0;
  try{
    feedid = boost::lexical_cast<Ice::Long>(splits.at(0));
  }catch(...){
    MCE_WARN("FeedInteractionI::IncHitCount --> cast err, " << splits.at(0));
  }
  if(feedid > 0){
    FeedInteractionNAdapter::instance().IncHitCount(feedid, content);
    //Inc(1, feedid, 1);
    //if(!FeedInteractionCache::instance().UpdateFeedInteraction(feedid, 1)){
    //  UpdateTimer::instance().PushClick(feedid);
    //}
  }
}

void FeedInteractionI::ClearDirtyCache() {
  MCE_INFO("FeedInteractionI::SaveDb");
  FeedInteractionCache::instance().ClearDirtyCache();
}


////////////////////////////////////////////////////////////////////////////////////////
bool ResultHandlerI::handle(mysqlpp::Row& row) const {
  InteractionCacheItemObjectPtr obj = new InteractionCacheItemObject;
  //InteractionCacheItem cache_item;
  obj->item.feed_ = row[DbHelper::kColFeedId.c_str()];
  int reply_count = row[DbHelper::kColReplyCount.c_str()];
  int click_count = row[DbHelper::kColClickCount.c_str()];
  int view_count = row[DbHelper::kColViewCount.c_str()];
  obj->item.count_ = (((Ice::Long)view_count)<<32) | (((Ice::Long)reply_count)<<16) | click_count;
  obj->item.range_ = row[DbHelper::kColRange.c_str()];
  
  //MCE_INFO("ResultHandlerI::handle --> feed_:" << obj->item.feed_
  //<< " range:" << obj->item.range_ << " reply_count:" << reply_count << " click_count:" << click_count); 
  _result->data[obj->item.feed_] = obj;
  return true;
}
void UpdateTimer::handle(){
  vector<Ice::Long> clicks;
  vector<Ice::Long> replys;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    clicks.swap(clicks_);
    replys.swap(replys_);
  }
  MCE_INFO("UpdateTimer::handle --> click:" << clicks.size() << " reply:" << replys.size());
  
  set<Ice::Long> idset;
  for(size_t i=0; i<clicks.size(); i++){
    idset.insert(clicks.at(i));
  }
  for(size_t i=0; i<replys.size(); i++){
    idset.insert(replys.at(i));
  }

  MCE_INFO("UpdateTimer::handle --> set size:" << idset.size());
  vector<Ice::Long> tmp;
  for(set<Ice::Long>::iterator it = idset.begin();  it!=idset.end(); ++it){
    if(!FeedInteractionCache::instance().IsHitted(*it)){
      tmp.push_back(*it);
    }
  }
  if(tmp.empty()){
    MCE_INFO("UpdateTimer::handle --> empty");
    return;
  }
  MCE_INFO("UpdateTimer::handle --> load from db size:" << tmp.size());
  FeedInteractionCache::instance().FindFeedInteraction(tmp);
  MCE_INFO("UpdateTimer::handle --> load from db end");

  for(size_t i=0; i<clicks.size(); i++){
    FeedInteractionCache::instance().UpdateFeedInteraction(clicks.at(i), 1, true);
    //FeedInteractionI::instance().Inc(1, clicks_.at(i), 1);
  }
  for(size_t i=0; i<replys.size(); i++){
    FeedInteractionCache::instance().UpdateFeedInteraction(replys.at(i), 2, true);
    //FeedInteractionI::instance().Inc(2, replys_.at(i), 1);
  }
}

}
}
