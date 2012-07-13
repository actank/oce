/*
 * FeedItemI.cpp
 *
 *  Created on: 2010-10-21
 *      Author: wu
 */

#include "client/linux/handler/exception_handler.h"
#include "FeedItemI.h"
#include "FeedCacheAdapter.h"
#include "FeedContentAdapter.h"
#include "IceLogger.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "FeedMemcLoaderAdapter.h"


#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace com::xiaonei::xce;
using namespace xce::feed;
using namespace MyUtil;


//using namespace mop::hi::svc::model;
//using namespace mop::hi::svc::adapter;

using namespace ::xce::serverstate;


void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedItemManagerI::instance(),
      service.createIdentity("FIM", ""));
  MyUtil::TaskManager::instance().scheduleRepeated(new CleanUserTimer());

  srand(time(NULL));
  
//   FeedDBInstance = service.getCommunicator()->getProperties()->getPropertyWithDefault(
  //    "FeedDBInstance", "feed_db");

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt(
      "FeedMini.Mod");
  int interval = 5;
  ServerStateSubscriber::instance().initialize("ControllerFeedMiniR",
      &FeedItemManagerI::instance(), mod, interval, new XceFeedChannel());
}


int FeedItemManagerI::Size(){
  int count = 0;
  for(int i=0; i<POOL_SIZE; i++){
    IceUtil::Mutex::Lock lock(mutexs_[i]);
    count += pool_[i].size(); 
  }
  return count;
}

bool FeedItemManagerI::LoadFeed(int uid, int time, int count){
	string table_name = GetTableName(uid);
	Statement sql;
	sql << "select * from "<< table_name << " where user=" << uid << " and time<FROM_UNIXTIME(" << time <<  ") order by time desc limit " << count;
	mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer,
			table_name).store(sql);
  if (!res) {
		MCE_INFO("FeedItemManagerI::LoadFeed --> failed, uid:" << uid);
    return true;
	}
  if(res.num_rows() == 0){
		MCE_INFO("FeedItemManagerI::LoadFeed --> no more feed, uid:" << uid);
    return false;
  }
  MCE_INFO("FeedItemManagerI::LoadFeed --> uid:" << uid << " time:" << time << " count:" << count << ", size:" << res.num_rows());
	int index = GetIndex(uid);
	{
	  IceUtil::Mutex::Lock lock(mutexs_[index]);
		const UserFeed* user_feed = GetUserFeed(uid);
    if(!user_feed){
      UserFeed tmp(uid);
      AddUserFeed(tmp);  
    }
    user_feed = GetUserFeed(uid);
    if(!user_feed){
      return true;
    }
    for(unsigned int i = 0; i < res.num_rows(); i++) {
			mysqlpp::Row row = res.at(i);
			FeedItem item;
			item.feed = row["feed"];
			item.merge = row["mini_merge"];
			item.time = (time_t) mysqlpp::DateTime(row["time"]);
			item.type = row["type"];
			item.actor = row["actor"];
      user_feed->live_feed_.AddFeedItem(item);
		}
	}
  if(res.num_rows() < count){
		MCE_INFO("FeedItemManagerI::LoadFeed --> no more feed, uid:" << uid << " count:" << res.num_rows());
    return false;
  }
	return true;
}

const UserFeed* FeedItemManagerI::GetUserFeed(int uid, bool relocate)
{
  int index = GetIndex(uid);
  UidIndex& uid_index = pool_[index].get<1> ();
  UidIndex::iterator it = uid_index.find(uid);
  if(it != uid_index.end()){
    if(relocate){
      SeqIndex& seq_index = pool_[index].get<0>();
      seq_index.relocate(seq_index.begin(), pool_[index].project<0>(it));
    }
    return &(*it);
  }
  return NULL;
}

void FeedItemManagerI::RemoveUserFeed(int uid){
  int index = GetIndex(uid);
  UidIndex& uid_index = pool_[index].get<1> ();
  UidIndex::iterator it = uid_index.find(uid);
  if(it != uid_index.end()){
    uid_index.erase(it);    
  }
}

void FeedItemManagerI::AddUserFeed(UserFeed& user_feed){
  int index = GetIndex(user_feed.userid_);
  pair<SeqIndex::iterator, bool> p = pool_[index].push_front(user_feed);
  if(!p.second){
    pool_[index].relocate(pool_[index].begin(), p.first);
  }else if(pool_[index].size() > MAX_USER_COUNT){
    int uid = pool_[index].back().userid_;
    pool_[index].pop_back();
    MCE_INFO("FeedItemManagerI::AddUserFeed --> eject user << uid:" << uid);
  }

}

FeedDataSeq FeedItemManagerI::getFeedData(int uid, int begin, int limit,
		const Ice::Current&) {
	return GetFeedDataImpl(uid, IntSeq(), true, begin, limit);
}

FeedDataSeq FeedItemManagerI::getFeedDataByStype(int uid,
		const MyUtil::IntSeq& stypes, int begin, int limit, const Ice::Current&) {
  return GetFeedDataImpl(uid, stypes, true, begin, limit);
}

FeedDataSeq FeedItemManagerI::getFeedDataByType(int uid,
		const MyUtil::IntSeq& types, int begin, int limit, const Ice::Current&) {
  return GetFeedDataImpl(uid, types, false, begin, limit);
}



FeedDataSeq FeedItemManagerI::getFeedData2(int uid, int begin, int limit,
		const Ice::Current&) {
	return FeedDataSeq();
}

void FeedItemManagerI::addFeed(const FeedItem& item,
		const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight,
		const Ice::Current&) {
  MCE_INFO("FeedItemManagerI::AddFeed --> feed:" << item.feed << " actor:" << item.actor << " stype:" << (item.type&0xffff)
      << " mergetype:" << (((unsigned int)item.type) >> 30) << " mergeid:" << item.merge<<"feed time:"<<item.time << " uptime:" << config->updateTime<<" line: "<<__LINE__);
  int index = GetIndex(item.actor);
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(item.actor);
    if (user_feed) {
      user_feed->live_feed_.AddNewFeedItem(item);
      return;
    }
  }
  bool more = LoadFeed(item.actor, 0x7FFFFFFF, LoadFeedCount);
  IceUtil::Mutex::Lock lock(mutexs_[index]);
  const UserFeed* user_feed = GetUserFeed(item.actor);
  if (user_feed) {
    if(!more){
      user_feed->live_feed_.SetOldestTime(item.actor, -1);
    }
    user_feed->live_feed_.AddNewFeedItem(item);
  }
}

void FeedItemManagerI::setRead(int uid, int stype, Ice::Long merge,
		const Ice::Current&) {
	MCE_INFO("FeedItemManagerI::setRead --> uid:" << uid << " stype:" << stype << " merge:" << merge);
  int index = GetIndex(uid);
	vector<LiveFeedItem> res;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      res = user_feed->live_feed_.Remove(stype, merge);
    }
  }

  MCE_INFO("FeedItemManagerI::setRead --> del, uid:" << uid << "stype:" << stype << " merge:" << merge << " size:" << res.size());
  Statement delSql;
  delSql << "delete from " << GetTableName(uid) << " where user=" << uid
      << " and small_type=" << stype << " and mini_merge=" << merge;
  mysqlpp::StoreQueryResult dbres = QueryRunner(FeedDBInstance, CDbWServer, GetTableName(uid)).store(delSql);
  if(!dbres){
    MCE_WARN("FeedItemManagerI::setRead --> del err, uid:" << uid << ", merge:" << merge);
  }
}

void FeedItemManagerI::setReadById(int uid, Ice::Long feedId,
		const Ice::Current&) {
	MCE_INFO("FeedItemManagerI::setReadById --> uid:" << uid << " feedid:" << feedId);	
  int index = GetIndex(uid);
	vector<LiveFeedItem> res;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
	  const UserFeed* user_feed = GetUserFeed(uid);
	  if (user_feed) {
	    res = user_feed->live_feed_.RemoveById(feedId);
	  }
  }
  mysqlpp::StoreQueryResult dbres;
  bool need_del = false;
  int stype = 0;
  Ice::Long merge = 0;
  if(!res.empty()){
    stype = res.front().SmallType();
    merge = res.front().merge();
    need_del = true;
  }else{
    Statement sql;
    sql << "select * from " << GetTableName(uid) << " where user=" << uid
      << " and feed=" << feedId;
    dbres = QueryRunner(FeedDBInstance, CDbRServer,
      GetTableName(uid)).store(sql);
    if(dbres && dbres.num_rows()>0) {
      need_del = true;
      mysqlpp::Row& row = dbres.at(0);
      stype = (int) row["small_type"];
      merge = (Ice::Long) row["mini_merge"];
    }
  }
  if(!need_del){
    MCE_INFO("FeedItemManagerI::setReadById --> need not del, uid:" << uid << " feed:" << feedId);
    return;
  }
  MCE_INFO("FeedItemManagerI::setReadById --> del, uid:" << uid 
  << " feed:" << feedId << " stype:" << stype << " merge:" << merge << " size:" << res.size());
  Statement delSql;
  delSql << "delete from " << GetTableName(uid) << " where user=" << uid
      << " and small_type=" << stype << " and mini_merge=" << merge;
  QueryRunner(FeedDBInstance, CDbWServer, GetTableName(uid)).store(delSql);
}

void FeedItemManagerI::setReadAll(int uid, const Ice::Current&) {
	MCE_INFO("FeedItemManagerI::setReadAll --> uid:" << uid);
  int index = GetIndex(uid);
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      user_feed->live_feed_.RemoveAll();
    }
  }
  Statement sql;
  sql << "delete from " << GetTableName(uid) << " where user = " << uid;
  QueryRunner(FeedDBInstance, CDbWServer, GetTableName(uid)).execute(sql);
}

bool FeedItemManagerI::hasFeed(int uid, const Ice::Current&) {
  {
    IceUtil::Mutex::Lock lock(mutexs_[GetIndex(uid)]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      return user_feed->live_feed_.Size() > 0;
    }
  }
  Statement sql;
  sql << "select * from " << GetTableName(uid) << " where user=" << uid << " limit 1";
  mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer, GetTableName(uid)).store(sql);
  if (!res || res.num_rows()<=0) {
    return false;
  }
  return true;
}

FeedDataSeq FeedItemManagerI::GetFeedDataImpl(int uid, const IntSeq& types, bool stype, int begin, int limit)
{
  int index = GetIndex(uid);
  int old_time = 0x7FFFFFFF;
  int orig_size = 0;
  vector<vector<vector<LiveFeedItem> > > live_feeds;
  {
      IceUtil::Mutex::Lock lock(mutexs_[index]);
      const UserFeed* user_feed = GetUserFeed(uid);
      if(user_feed){
        if(types.empty()){
          live_feeds = user_feed->live_feed_.GetFeed(uid, begin, limit);
        }else{
          if(stype){
            live_feeds = user_feed->live_feed_.GetFeedByStype(uid, types,  begin, limit);
          }else{
            live_feeds = user_feed->live_feed_.GetFeedByType(uid, types,  begin, limit);
          }
        }
        old_time = user_feed->live_feed_.GetOldestTime();
        orig_size = user_feed->live_feed_.Size();
      }
  }
  int now_time = old_time; 
  if(limit > live_feeds.size() && old_time>0){
    MCE_INFO("FeedItemManagerI::GetFeedDataImpl --> need load uid:" << uid << " mergesize:" << live_feeds.size());
    int absent = limit - live_feeds.size();
    int count = (absent/LoadFeedCount + 1 ) * LoadFeedCount;
    bool more = LoadFeed(uid, old_time, count);
    {
      IceUtil::Mutex::Lock lock(mutexs_[index]);
      const UserFeed* user_feed = GetUserFeed(uid);
      if(user_feed){
        if(!more){
          user_feed->live_feed_.SetOldestTime(uid, -1);
        }
        if(types.empty()){
          live_feeds = user_feed->live_feed_.GetFeed(uid, begin, limit);
        }else{
          if(stype){
            live_feeds = user_feed->live_feed_.GetFeedByStype(uid, types, begin, limit);
          }else{
            live_feeds = user_feed->live_feed_.GetFeedByType(uid, types, begin, limit);
          }
        }
        now_time = user_feed->live_feed_.GetOldestTime();
        orig_size = user_feed->live_feed_.Size();
      }
    }
  }
  if(orig_size > LoadFeedCount){
    NeedClean(uid);
  }
  FeedDataSeq feeds;
  feeds = BuildFeedContents(uid, live_feeds);
  MCE_INFO("FeedItemManagerI::GetFeedDataImpl --> uid:" << uid << " begin:" << begin << " limit:" << limit << " mergesize:" << live_feeds.size()
          << " size:" << live_feeds.size() << " res:" << feeds.size() << " types:" << types.size() << " bystype:" << stype
          << " oldtime:" << old_time << " nowtime:" << now_time << " orginsize:" << orig_size);
  return feeds;
}

FeedItemSeq FeedItemManagerI::GetOriginalFeedData(int uid, const Ice::Current&){
  IceUtil::Mutex::Lock lock(mutexs_[GetIndex(uid)]);
  const UserFeed* user_feed = GetUserFeed(uid);
  if(user_feed){
    return user_feed->live_feed_.GetAllItems();
  }
  return FeedItemSeq();
}

void FeedItemManagerI::NeedClean(int user, int time) {
  IceUtil::Mutex::Lock lock(clean_mutex_);
  clean_map_[user] = time;
}

void FeedItemManagerI::CleanUser() {
  MCE_INFO("FeedItemManagerI::CleanUser --> user size:" << Size());
  map<int, int> cleans;
  {
    IceUtil::Mutex::Lock lock(clean_mutex_);
    cleans.swap(clean_map_);
  }
  MCE_INFO("FeedItemManagerI::CleanUser --> size:" << cleans.size());
  int c = 0;
  for(map<int, int>::iterator it=cleans.begin(); it!=cleans.end(); ++it){
    if(time(NULL)-it->second > 1*60 ){
      IceUtil::Mutex::Lock lock(mutexs_[GetIndex(it->first)]);
      const UserFeed* user_feed = GetUserFeed(it->first);
      if (user_feed) {
        user_feed->live_feed_.Clean();
        c++;
      }
    }else{
      NeedClean(it->first, it->second);
    }   
  }
  MCE_INFO("FeedItemManagerI::CleanUser --> size:" << cleans.size() << " cleaned:" << c);
}


vector<FeedItemSeq> FeedItemManagerI::GetFeedItemSeqSeqByUsers(
		const MyUtil::IntSeq& ids, const Ice::Current&) {
  const IntSeq types;
  FeedItemSeq tmpItemSeq;
  FeedItemSeqSeq itemseqseq;
  bool hasLoaded = false;
  for (vector<int>::const_iterator vit = ids.begin(); vit != ids.end(); ++vit) {
    int uid = (*vit);
    int index = GetIndex(uid);
    {
      IceUtil::Mutex::Lock lock(mutexs_[index]);
      const UserFeed* user_feed = GetUserFeed(uid);
      if (user_feed) {
        hasLoaded = true;
      }
    }
    bool more = true;
    if (!hasLoaded) {
      more = LoadFeed(uid, 0x7FFFFFFF, LoadFeedCount);
    }
    int s = 0;
    {
      IceUtil::Mutex::Lock lock(mutexs_[index]);
      const UserFeed* user_feed = GetUserFeed(uid);
      if (user_feed) {
        if(!more){
          user_feed->live_feed_.SetOldestTime(uid, -1);
        }
        FeedItemSeqSeq tmp = user_feed->live_feed_.GetLatestItems(uid, 10);
        s = tmp.size();
        itemseqseq.insert(itemseqseq.end(), tmp.begin(), tmp.end());
      }
    }
	  MCE_INFO("FeedItemManagerI::GetFeedItemSeqSeqByUsers --> id:" << uid << " size:" << s);
  }
  return itemseqseq;
}

FeedDataPtr FeedItemManagerI::BuildAFeed(int uid, const vector<vector<LiveFeedItem> >& group, const FeedContentDict& fdict){
  FeedDataPtr data;
  FeedReplyPtr reply;
  ostringstream xml;
  vector<Ice::Long> valid_feedids;
  int stype = 0;
  int gather_count = 0;
  for(size_t j=0; j<group.size(); j++){
    bool getid = false;
    int merge_count = 0;
    int merge_type = 0;
    for(size_t k=0; k<group.at(j).size(); k++){
      FeedContentDict::const_iterator dict_it = fdict.find(group.at(j).at(k).feed_id());
      merge_type = group.at(j).at(k).NewsMergeType();
      if(dict_it != fdict.end()){
        if(dict_it->second){
          stype = dict_it->second->data->type&0xFFFF;
          if(gather_count==0 && !data){
            data = dict_it->second->data;
            reply = dict_it->second->reply;
          }
          if(gather_count==0 && data &&
            ((merge_count==0 && merge_type==FeedMergeReplace) || (merge_count<5 && merge_type==FeedMergeAppend))){
            xml << dict_it->second->data->xml;
            merge_count++;
          }
          if(gather_count>0 && !getid){
            valid_feedids.push_back(group.at(j).at(k).feed_id());
            getid = true;
          }
        }else{
          int index = GetIndex(uid);
          IceUtil::Mutex::Lock lock(mutexs_[index]);
          const UserFeed* user_feed = GetUserFeed(uid);
          if(user_feed){
            user_feed->live_feed_.RemoveById(group.at(j).at(k).feed_id(), false);
            MCE_INFO("FeedItemManagerI::BuildAFeed --> remove uid:" << uid << " feedid:" << group.at(j).at(k).feed_id());
          }
        }
      }
      if(merge_type==FeedMergeReplace){
        break;
      }else if(merge_type==FeedMergeAppend){
        if(k>=5){
          break;
        }
      }
    }
    if(data){
      gather_count++;
    }
  }
  if(reply){
   xml << reply->oldReply << reply->newReply << "<reply_count>" << reply->count << "</reply_count>";
  }
  if(valid_feedids.size() > 0){
   string name = "merge_feed";
   xml << "<" << name << "><count>" << valid_feedids.size() << "</count>";
   for (size_t j = 0; j < valid_feedids.size(); j++) {
     xml << "<fid>" << valid_feedids.at(j) << "</fid>";
   }
   xml << "</" << name << ">";
  }
  if(data){
   data->xml = xml.str();
  }
  return data;
} 

void FeedItemManagerI::CheckLoad(int uid, vector<Ice::Long> & fids, FeedContentDict & dict) {
  vector<Ice::Long> miss_ids;
  for (int i = 0; i < fids.size(); ++i) {
    if (dict.find(fids.at(i)) == dict.end()) {
      miss_ids.push_back(fids.at(i));
    }
  }
  if(miss_ids.empty()){
    return;
  }
  //float get_rate = (float)dict.size() / (float)fids.size();
  if (!dict.empty()) {
    try {
      FeedMemcLoaderAdapter::instance().LoadFeedSeq(miss_ids);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedItemManagerI::CheckLoad --> load uid:" << uid << " size:" << miss_ids.size() << " err, " << e);
      return;
    }
    MCE_INFO("FeedItemManagerI::CheckLoad --> load " << uid << " size:" << miss_ids.size());
  }else{
    FeedContentDict dict_loader;
    try {
      dict_loader= FeedMemcLoaderAdapter::instance().GetFeedDict(miss_ids, 100);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedItemManagerI::CheckLoad --> load uid:" << uid << " size:" << miss_ids.size() << " err, " << e);
      return;
    }
    MCE_INFO("FeedItemManagerI::CheckLoad --> load " << uid << " size:" << miss_ids.size() << " res:" << dict_loader.size());
    dict.insert(dict_loader.begin(),dict_loader.end());
  }
}

FeedDataSeq FeedItemManagerI::BuildFeedContents(int uid, vector<vector<vector<LiveFeedItem> > >& feeds){
  LongSeq feed_ids;
  for(size_t i=0; i<feeds.size(); i++){
    for(size_t j=0; j<feeds.at(i).size(); j++){
      for(size_t k=0; k<feeds.at(i).at(j).size(); k++){
        feed_ids.push_back(feeds.at(i).at(j).at(k).feed_id());
      }
    }
  }
  FeedContentDict fdict;
  try {
    fdict = FeedContentAdapter::instance().getFeedDict(feed_ids);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedItemManagerI::BulidFeedData --> call FeedContent.getFeedDict err:" << e);
    return FeedDataSeq();
  }
  MCE_INFO("FeedItemManagerI::BulidFeedData --> call FeedContent, uid:" << uid << " id size:" << feed_ids.size() <<  " res size:" << fdict.size());
  if(fdict.size() < feed_ids.size()){
    CheckLoad(uid, feed_ids, fdict);
  }
  FeedDataSeq seq;
  for(size_t i=0; i<feeds.size(); i++){
    FeedDataPtr tmp;
    tmp = BuildAFeed(uid, feeds.at(i), fdict);
    if(tmp){
      seq.push_back(tmp);
    }
  }
  return seq;
}

void FeedItemManagerI::Reload(int uid, const Ice::Current&){
  IceUtil::Mutex::Lock lock(mutexs_[GetIndex(uid)]);
  RemoveUserFeed(uid); 
}

void FeedItemManagerI::PressureTest(int uid, int begin, int limit, const Ice::Current&){
//  int p = rand()%5;
//  getFeedData(uid, p*10, 10);
}
