#include "ResultCache.h"
#include "client/linux/handler/exception_handler.h"
#include "FeedItemI.h"
#include "FeedCacheAdapter.h"
#include "FeedContentAdapter.h"
#include "IceLogger.h"
#include "ServiceI.h"
#include "PerformanceStat.h"
#include "NewsUtil.h"
#include "FeedMemcLoaderAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "XceLoggerAdapter.h"
#include "SortHelper.h"
#include <boost/foreach.hpp>

//#include "FeedSortReplicaAdapter.h"
//#include "SSDFeedDBAdapter.h"
using namespace xce::feed;
bool MyCallback(const char *dump_path, const char *minidump_id, void *context,
    bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}

void MyUtil::initialize() {
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedItemManagerI::instance(),
      service.createIdentity("FIM", ""));

  //controller
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt(
      "FeedNews.Mod");
  int interval =
      service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(
          "FeedNews.Interval", 5);
  xce::serverstate::ServerStateSubscriber::instance().initialize(
      "ControllerFeedNewsR", &FeedItemManagerI::instance(), mod, interval,
      new XceFeedChannel());
  MCE_INFO("MyUtil::initialize. mod:" << mod << " interval:" << interval);

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  int index = props->getPropertyAsIntWithDefault("Service.ServerIndex", 0);
  EDMFeedManager::instance().setIndex(index);

  TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());
//  TaskManager::instance().scheduleRepeated(
//      &UpdateInteractionInDBTimer::instance());
//  TaskManager::instance().scheduleRepeated(
//      &UpdateInteractionInCacheTimer::instance());
  TaskManager::instance().execute(new LoadTask());
  TaskManager::instance().scheduleRepeated(&ShiftStatTimer::instance());
	GetLogger::instance().Init();

  //TaskManager::instance().scheduleRepeated(&CheckEnterHotFeedTimer::instance());

}

void FeedItemManagerI::EraseUser(int uid, const Ice::Current&) {
  int index = GetIndex(uid);
  UidIndex& uid_index = pool_[index].get<1> ();
  UidIndex::iterator it = uid_index.find(uid);
  if (it != uid_index.end()) {
    MCE_INFO("FeedItemManagerI::EraseUser. uid in mem. uid:"<< uid);
    uid_index.erase(it);
  } else {
    MCE_INFO("FeedItemManagerI::EraseUser. uid not in mem. uid:" << uid);
  }
}

int FeedItemManagerI::Size() {
  int count = 0;
  TimeStat tts;
  float total = 0;
  for (int i = 0; i < POOL_SIZE; i++) {
    TimeStat ts;
    IceUtil::Mutex::Lock lock(mutexs_[i]);
    total += ts.getTime();
    //MCE_INFO("FeedItemManagerI::Size --> " << ts.getTime());
    count += pool_[i].size();
  }
  MCE_INFO("FeedItemManagerI::Size --> total: " << tts.getTime() << " " << total << " " << POOL_SIZE);
  return count;
}

const UserFeed* FeedItemManagerI::GetUserFeed(int uid, bool relocate) {
  int index = GetIndex(uid);
  UidIndex& uid_index = pool_[index].get<1> ();
  UidIndex::iterator it = uid_index.find(uid);
  if (it != uid_index.end()) {
    if (relocate) {
      SeqIndex& seq_index = pool_[index].get<0> ();
      seq_index.relocate(seq_index.begin(), pool_[index].project<0> (it));
      it = uid_index.find(uid);
      if (it != uid_index.end()) {
        return &(*it);
      }
    } else {
      return &(*it);
    }
  }
  return NULL;
}



void FeedItemManagerI::AddUserFeed(const UserFeed& user_feed) {
  int index = GetIndex(user_feed.userid_);
  pair<SeqIndex::iterator, bool> p = pool_[index].push_front(user_feed);
  if (!p.second) {
    pool_[index].relocate(pool_[index].begin(), p.first);
  } else if ((int) pool_[index].size() > MAX_USER_COUNT) {
    pool_[index].pop_back();
  }
}

bool FeedItemManagerI::LoadFeed(int uid) {
  FeedItemSeq items;
  try {
    TimeStat ts;
    items = FeedCacheAdapter::instance().get(uid);
    //PerformanceStat::instance().stat("GetCache", ts.getTime());
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedItemManagerI::LoadFeed --> err, userid:" << uid << " " << e);
    return false;
  }
  MCE_INFO("FeedItemManagerI::LoadFeed --> call FeedCache, uid:" << uid << " res size:" << items.size());

  LongSeq feedids;
  {
    int index = GetIndex(uid);
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    if (GetUserFeed(uid)) {
      return true;
    }
    UserFeed user_feed(uid);
    for (int i = (int) items.size() - 1; i >= 0; i--) {
      feedids.push_back(items.at(i).feed);
      user_feed.unique_feed_list_.Add(uid, items.at(i), true, false);
    }
    FeedItemSeq edmFeeds = EDMFeedManager::instance().getEDMFeed(uid);
    for (int i = 0; i < (int) edmFeeds.size(); i++) {
      //MCE_INFO("FeedItemManagerI::LoadFeed --> have edm userid:" << uid << " feed:" << edmFeeds.at(i).feed << " time:" << edmFeeds.at(i).time);
      user_feed.unique_feed_list_.Add(uid, edmFeeds.at(i), true, false);
    }
    AddUserFeed(user_feed);
  }
//  FeedPool::instance().UpdateInteraction(feedids, false);
  return true;
}


void FeedItemManagerI::CheckLoad(int uid, vector<Ice::Long> & fids, FeedContentDict & dict) {
  vector<Ice::Long> miss_ids;
  for (unsigned i = 0; i != fids.size(); ++i) {
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
      MCE_WARN("FeedItemManagerI::CheckLoad --> LoadFeedSeq uid:" << uid << " size:" << miss_ids.size() << " err, " << e);
      return;
    }
    MCE_INFO("FeedItemManagerI::CheckLoad --> LoadFeedSeq " << uid << " size:" << miss_ids.size());
  }else{
    FeedContentDict dict_loader;
    try {
      dict_loader= FeedMemcLoaderAdapter::instance().GetFeedDict(miss_ids, 100);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedItemManagerI::CheckLoad --> GetFeedDict uid:" << uid << " size:" << miss_ids.size() << " err, " << e);
      return;
    }
    MCE_INFO("FeedItemManagerI::CheckLoad --> GetFeedDict " << uid << " size:" << miss_ids.size() << " res:" << dict_loader.size());
    dict.insert(dict_loader.begin(),dict_loader.end());
  }
}




void FeedItemManagerI::GetFeedContentDict(int uid,LongSeq & feed_ids,FeedContentDict & fdict){
  try {
    fdict = FeedContentAdapter::instance().getFeedDict(feed_ids);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedItemManagerI::BulidFeedData --> call FeedContent.getFeedDict err:" << e);
  }

  MCE_INFO("FeedItemManagerI::BulidFeedData --> call FeedContent, uid:" << uid << " id size:" << feed_ids.size() << " res size:" << fdict.size());
  if(fdict.size() < feed_ids.size()){
    CheckLoad(uid, feed_ids,fdict);
  }
}

FeedDataSeq FeedItemManagerI::BuildBasicFeedDataV2(int uid,
		Long3D & fid3d, const hash_map<long,SharedFeedPtr> & feedmap,
		int begin, int limit,set<Ice::Long> & show_fids) {
  LongSeq feed_ids;
  SharedFeedPtr3D feed3d;
  for(int i3d = begin; i3d <= begin+limit && i3d <  fid3d.size();++i3d ){
  	feed3d.resize(feed3d.size()+1);
  	SharedFeedPtr2D & feed2d = *feed3d.rbegin();
  	Long2D & fid2d = fid3d.at(i3d);
    BOOST_FOREACH(Long1D & fid1d,fid2d){
    	feed2d.resize(feed2d.size() + 1);
    	SharedFeedPtr1D & feed1d = *feed2d.rbegin();
    	BOOST_FOREACH(long fid,fid1d){
    		hash_map<long,SharedFeedPtr>::const_iterator fit= feedmap.find(fid);
    		if(fit != feedmap.end()){
    			feed1d.push_back(fit->second);
      		feed_ids.push_back(fid);
      		show_fids.insert(fid);
    		}
    	}
    }
  }

  FeedDataSeq seq;
  FeedContentDict fdict;
  GetFeedContentDict(uid,feed_ids,fdict);

  BOOST_FOREACH(SharedFeedPtr2D & feed2d,feed3d){
    FeedDataPtr tmp = BuildAFeed(uid, feed2d, fdict);
    if (tmp) {
      seq.push_back(tmp);
    }
  }
  return seq;
}

FeedDataResPtr FeedItemManagerI::GetLiveFeedData(int uid, int begin, int limit,
    const Ice::Current&) {
//  return GetFeedDataImpl(uid, IntSeq(), true, begin, limit, false);
  return GetFeedDataRemote(uid, IntSeq(), true, begin, limit, false);
}

FeedDataResPtr FeedItemManagerI::GetLiveFeedDataByStype(int uid,
    const IntSeq& stypes, int begin, int limit, const Ice::Current&) {
//  return GetFeedDataImpl(uid, stypes, true, begin, limit, false);
  return GetFeedDataRemote(uid, stypes, true, begin, limit, false);
}

FeedDataResPtr FeedItemManagerI::GetLiveFeedDataByType(int uid,
    const IntSeq& types, int begin, int limit, const Ice::Current&) {
//  return GetFeedDataImpl(uid, types, false, begin, limit, false);
  return GetFeedDataRemote(uid, types, false, begin, limit, false);
}

FeedDataResPtr FeedItemManagerI::GetHotFeedData(int uid, int begin, int limit,
    const Ice::Current&) {
//	if (uid == 239556923) {
		return GetFeedDataRemote(uid, IntSeq(), true, begin, limit, true);
//	} else {
//		return GetFeedDataImpl(uid, IntSeq(), true, begin, limit, true);
//	}
}

FeedDataResPtr FeedItemManagerI::GetHotFeedDataByStype(int uid,
    const IntSeq& stypes, int begin, int limit, const Ice::Current&) {
//  return GetFeedDataImpl(uid, stypes, true, begin, limit, true);
  return GetFeedDataRemote(uid, stypes, true, begin, limit, true);
}

FeedDataResPtr FeedItemManagerI::GetHotFeedDataByType(int uid,
    const IntSeq& types, int begin, int limit, const Ice::Current&) {
//  return GetFeedDataImpl(uid, types, false, begin, limit, true);
  return GetFeedDataRemote(uid, types, false, begin, limit, true);
}

void GetFeedItemsInFeedCacheAndEdmPool(GetRequest & req){
	GetFeedItemsFromFeedCache(req);
	GetEdmFeedItems(req);
}

void GetFeedItemsFromFeedCache(GetRequest & req, MiddleData & mid){
	FeedItemSeq items = FeedCacheAdapter::instance().Get(req.uid_);
	req.orig_items_.swap(items);
}
void GetEdmFeedItems(GetRequest & req, MiddleData & mid){
	//TODO fromcache
	FeedItemSeq edms = getEDMFeed::instance().getEDMFeed(req.uid_);
	if(edms.empty()){
		return;
	}
	req.orig_edms_.swap(edms);
}

void GetFeedInRange(req,mid){
	
}

void GetFeedContent(req,mid){
	GetFromMemcache(mid);
	if(mid.NotComplete){
	 	GetFromDb(mid);
	}
}

FeedDataResPtr FeedItemManagerI::GetFeedDataImpl(int uid, const IntSeq& types,
    bool stype, int begin, int limit, bool hot) {
 	GetRequest req(uid,types,stype,begin,limit,hot);
	MiddleData mid();

	GetFeedItemsInFeedCacheAndEdmPool(req,mid);
	SortHelper::MergeAndSort(req,mid);
	GetFeedContent(req,mid);
	FeedDataResPtr res = BuildFeedData(req,mid);
	if(req->showedms()){
		EraseEdms(req);	
	}
	MCE_INFO(req.ToString() << mid.ToString());
	return res;
}

void FeedItemManagerI::IncView(int uid,set<long> & showfids){
  {
    int index = GetIndex(uid);
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      user_feed->unique_feed_list_.IncView(showfids);
    }
  }
}


void MakeLong3D(hash_map<long ,SharedFeedPtr> & fmap,Long3D & f3d){
	typedef hash_map<long ,SharedFeedPtr>::iterator LongFeed;
	for(LongFeed longfeed = fmap.begin(); longfeed != fmap.end();++longfeed){
		Long1D f1d;
		f1d.push_back(longfeed->first);
		Long2D f2d;
		f2d.push_back(f1d);
		f3d.push_back(f2d);
	}
}
FeedDataResPtr FeedItemManagerI::GetFeedDataRemote(int uid, const IntSeq& types,
    bool stype, int begin, int limit, bool hot) {
  //MCE_INFO("FeedItemManagerI::getFeedDataImpl --> uid:" << uid << " begin:" << begin << " limit:" << limit 
  //        << " types:" << types.size() << " bystype:" << stype << " hot:" << hot);
  float load_cost, build_cost, merge_cost,sort_cost,convert_cost;
  TimeStat ts;
  int index = GetIndex(uid);
  bool load = true;
  vector<vector<vector<SharedFeedPtr> > > feeds;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);

    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      load = false;
    }
  }

  TimeStat ts1;
  if (load) {
    LoadFeed(uid);
  }
  int unviewed_count = 0;
  int edm_count = 0;
  load_cost = ts1.getTime();
  ts1.reset();
  vector<vector<vector<SharedFeedPtr> > > random;
  SortParamPtr param = new SortParam;
  hash_map<long ,SharedFeedPtr> feedmap;
  vector<long> edms;
  FeedFilterPtr filter = new FeedFilter(types,stype,begin == 0 && hot);
  int edm_size = 0;
  time_t lastgettime;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      MCE_INFO("Dump LiveFeed. uid:" << user_feed->userid_ << " "<< user_feed->unique_feed_list_.DumpToString());
      user_feed->unique_feed_list_.GetFeedWrappers(filter,param->feeds,feedmap,edm_size);
      lastgettime = user_feed->last_get_time_;
      user_feed->last_get_time_ = time(NULL);
    }
  }
  param->begin = begin;
  param->sort = hot;
  param->showedm = hot;
  param->showrandom = hot;
  param->gatherbytime = hot;
  param->lastGetTime = lastgettime;
  SortResultPtr sortres = new SortResult ;
  merge_cost = ts1.getTime();

  try{
  	sortres = FeedSortAdapter::instance().Sort(uid,param);
  }catch(Ice::Exception& e){
  	MCE_WARN("FeedItemManagerI::GetFeedDataRemote. fail. redirect. uid:" << uid << " exception:" << e);
//  	return 0;
  	return GetFeedDataImpl(uid,types,
  	   stype,begin, limit, hot);
  }


  sort_cost = ts1.getTime();

  FeedDataResPtr res = new FeedDataRes;


  res->UnviewedCount = unviewed_count;

  convert_cost = ts1.getTime();
  MCE_INFO("#######. fids:" << sortres->fids.size()
  		<< " feedmap:" << feedmap.size() << " feeds:" << feeds.size()
  		<< " sort_cost:" << sort_cost << " convert_cost:" << convert_cost);

//  MakeLong3D(feedmap,sortres->fids);
  set<long> showfids;
  res->data = BuildBasicFeedDataV2(uid, sortres->fids, feedmap,begin, limit,showfids);



//  MCE_INFO("FeedItemManagerI::GetFeedDataRemote. showfids size:" << showfids.size()
//  		<< " showfids:" << PrintHelper::Print(showfids));

  IncView(uid,showfids);

  build_cost = ts1.getTime();

  MCE_INFO("FeedItemManagerI::GetFeedDataImplR --> uid:" << uid << " begin:" << begin << " limit:" << limit
      << " types:" << types.size() << " bystype:" << stype << " hot:" << hot << " feedsize:" << sortres->fids.size()
      << " " << res->data.size()
      << " loadtime:" << load_cost << " mergetime:" << merge_cost
      << " buildtime:" << build_cost << " totaltime:" << ts.getTime()
      << " sort_cost:" << sort_cost << " edmCount:" << sortres->edmCount);

	GetLogger::instance().SendLog(uid,res->data);

	if(filter->show_edm() && sortres->edmCount >0){
    EDMFeedManager::instance().removeUser(uid);
	}

  ////////////
  //test
  //  EraseUser(uid);
  ////////////
  return res;
}
/*
 FeedDataSeq FeedItemManagerI::getFeedData(int uid, int begin, int limit, const Ice::Current&){
 return getLiveFeedData(uid, begin, limit);
 }*/

void FeedItemManagerI::AddFeed(const FeedItem& item,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight,
    const Ice::Current&) {
  MCE_INFO("FeedItemManagerI::AddFeed --> feed:" << item.feed << " actor:" << item.actor << " stype:" << (item.type&0xffff)
      << " mergetype:" << (((unsigned int)item.type) >> 30) << " mergeid:" << item.merge << " uptime:" << config->updateTime);
  Int2IntMap::const_iterator it = id2weight.begin();
  for (; it != id2weight.end(); ++it) {
    if (item.time < 0) {
      EDMFeedManager::instance().addEDMFeed(item, it->first);
    }
    int index = GetIndex(it->first);
    bool get = false;
    {
      IceUtil::Mutex::Lock lock(mutexs_[index]);
      const UserFeed* user_feed = GetUserFeed(it->first);
      if (user_feed) {
        const_cast<FeedItem&> (item).weight = it->second;
        //Ice::Long oldfeed = -1;
        user_feed->unique_feed_list_.Add(it->first, item, config->updateTime, true);
        get = true;
        //if(oldfeed > 0){
        //  MCE_INFO("evict from hot " << it->first << " " << oldfeed);
        //  user_feed->hot_feed_.Remove(oldfeed);
        //}
        //vector<LiveFeedItem*> seq;
        //seq.push_back(li);
        //user_feed->hot_feed_.CheckEnterHot(seq, false, config->updateTime);
      }
    }
  }
}

void FeedItemManagerI::SetRead(int uid, int stype, Ice::Long merge,
    const Ice::Current&) {
  int index = GetIndex(uid);
  vector<Ice::Long> feedids;
  bool get = false;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      feedids = user_feed->unique_feed_list_.Remove(stype, merge);
      get = true;
      //user_feed->hot_feed_.Remove(stype, merge);
    }
  }
  MCE_INFO("FeedItemManagerI::setRead --> uid:" << uid << " stype:" << stype << " merge:" << merge);
  for (size_t i = 0; i < feedids.size(); i++) {
    FeedCacheAdapter::instance().remove(uid, feedids.at(i));
  }
}

void FeedItemManagerI::SetReadById(int uid, Ice::Long feedId,
    const Ice::Current&) {
  MCE_DEBUG("FeedItemManagerI::SetReadById, uid:" << uid << ",feedid" << feedId);
  int index = GetIndex(uid);
  vector<Ice::Long> feedids;
  bool get = false;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      feedids = user_feed->unique_feed_list_.Remove(feedId);
      //user_feed->hot_feed_.Remove(feedId);
      get = true;
    }
  }
  MCE_INFO("FeedItemManagerI::setReadById --> uid:" << uid << " feedid:" << feedId << " removed:" << feedids.size());
  for (size_t i = 0; i < feedids.size(); i++) {
    FeedCacheAdapter::instance().remove(uid, feedids.at(i));
  }
}

void FeedItemManagerI::SetReadAll(int uid, const Ice::Current&) {
  MCE_INFO("FeedItemManagerI::SetReadAll --> uid:" << uid);
  int index = GetIndex(uid);
  bool get = false;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      user_feed->unique_feed_list_.RemoveAll();
      //user_feed->hot_feed_.RemoveAll();
      get = true;
    }
  }
  FeedCacheAdapter::instance().removeAll(uid);
}

FeedDataSeq FeedItemManagerI::GetFeedDataByIds(const MyUtil::LongSeq& fids,
    const Ice::Current&) {
  MCE_INFO("FeedItemManagerI::GetFeedDataByIds --> size:" << fids.size());
  FeedContentDict dict;
  try {
    dict = FeedContentAdapter::instance().getFeedDict(fids);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedItemManagerI::getFeedDataByIds --> " << e);
  }
  FeedDataSeq seq;
  for (FeedContentDict::iterator it = dict.begin(); it != dict.end(); ++it) {
    if (it->second) {
      if (it->second->reply) {
        ostringstream xml;
        xml << it->second->data->xml;
        xml << it->second->reply->oldReply << it->second->reply->newReply
            << "<reply_count>" << it->second->reply->count << "</reply_count>";
        it->second->data->xml = xml.str();
      }
      seq.push_back(it->second->data);
    }
  }
  return seq;
}

FeedDataSeq FeedItemManagerI::GetFeedDataByIdsWithUid(int uid,
    const MyUtil::LongSeq& fids, Ice::Long first, int stype,
    const Ice::Current&) {
  MCE_INFO("FeedItemManagerI::GetFeedDataByIdsWithUid. uid:" << uid << " fids size:" << fids.size() <<" first" << first << " stype:" << stype);
  if (!MergeConfig::instance().IsComment(stype)
      && !MergeConfig::instance().IsMain(stype)) {
    MCE_DEBUG("redirect to GetFeedDataByIds");
    return GetFeedDataByIds(fids);
  }
  MCE_DEBUG("Are merge stype");

  float load_cost/*, build_cost, merge_cost*/;
  TimeStat ts;
  int index = GetIndex(uid);
  bool load = true;
  vector<vector<vector<SharedFeedPtr> > > feeds;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    //EraseUserFeed(uid);//TODO 待删除
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      load = false;
    }
  }

  TimeStat ts1;
  if (load) {
    LoadFeed(uid);
  }
  int /*unviewed_count = 0, */edm_count = 0;
  load_cost = ts1.getTime();
  ts1.reset();

  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      int unviewed_count;
      bool hot = true;
      vector<vector<vector<SharedFeedPtr> > > random;
      feeds = user_feed->unique_feed_list_.GetGatheredFeed(uid, 0, 250, hot, random,
          unviewed_count, edm_count);
      //      feeds = user_feed->live_feed_.GetGatheredFeed(uid, begin, limit, hot,
      //          random, unviewed_count, edm_count);
    }
  }
  set<Ice::Long> fid_set(fids.begin(), fids.end());

  int x = -1;
  for (size_t i = 0; i != feeds.size(); ++i) {
    for (size_t j = 0; j != feeds.at(i).size(); ++j) {
      for (size_t k = 0; k != feeds.at(i).at(j).size(); ++k) {
        if (fid_set.count(feeds.at(i).at(j).at(k)->feed_id())) {
          x = i;
          goto finish;
        }
      }
    }
  }

  finish:

  MCE_DEBUG("GetFeedDataByIdsWithUid. x:"<< x);
  if (x == -1) {
    MCE_DEBUG("GetFeedDataByIdsWithUid. can not find");
    return FeedDataSeq();
  }

  vector<vector<vector<SharedFeedPtr> > > new_feeds;
  MyUtil::LongSeq feed_ids;

  for (size_t j = 0; j != feeds.at(x).size(); ++j) {
    MyUtil::LongSeq tmp_ids;
    bool is_first = false;
    for (size_t k = 0; k != feeds.at(x).at(j).size(); ++k) {
      Ice::Long feedid = feeds.at(x).at(j).at(k)->feed_id();
      if (feedid != first) {
        tmp_ids.push_back(feedid);
      } else {
        is_first = true;
      }
    }
    if (!is_first) {
      feed_ids.insert(feed_ids.end(), tmp_ids.begin(), tmp_ids.end());
      vector<vector<SharedFeedPtr> > tmp;
      tmp.push_back(feeds.at(x).at(j));
      new_feeds.push_back(tmp);
    }
  }
  MCE_DEBUG("GetFeedDataByIdsWithUid.  feed_ids:" << PrintHelper::Print(feed_ids));

  FeedContentDict fdict;
  try {
    TimeStat ts;
    fdict = FeedContentAdapter::instance().getFeedDict(feed_ids);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedItemManagerI::GetFeedDataByIdsWithUid --> call FeedContent.getFeedDict err:" << e);
    return FeedDataSeq();
  }

  FeedDataSeq res;
  for (size_t i = 0; i < new_feeds.size(); i++) {
    FeedDataPtr tmp = BuildAFeed(uid, new_feeds.at(i), fdict);
    res.push_back(tmp);
  }

  return res;
}

FeedItemSeq FeedItemManagerI::GetOriginalFeedData(int uid, const Ice::Current&) {
  int index = GetIndex(uid);
  vector<SharedFeedPtr> feeds;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      feeds = user_feed->unique_feed_list_.GetFeed();
    }
  }
  FeedItemSeq seq;
  for (size_t i = 0; i < feeds.size(); i++) {
    FeedItem item;
    item.feed = feeds.at(i)->id_;
    item.merge = feeds.at(i)->merge_;
    item.type = feeds.at(i)->type_;
    item.actor = feeds.at(i)->actor_;
    item.time = feeds.at(i)->time_;
    seq.push_back(item);
  }
  return seq;
}

int FeedItemManagerI::GetUnviewedCount(int uid, int type, const Ice::Current&) {
  int index = GetIndex(uid);
  vector<SharedFeedPtr> feeds;
  {
    IceUtil::Mutex::Lock lock(mutexs_[index]);
    const UserFeed* user_feed = GetUserFeed(uid);
    if (user_feed) {
      vector<vector<vector<SharedFeedPtr> > > tmp;
      int unviewed_count = 0;
      int edm_count = 0;
      user_feed->unique_feed_list_.GetGatheredFeed(uid, 0, 0, false, tmp,
          unviewed_count, edm_count);
      return unviewed_count;
    }
  }
  return 0;
}

//void 	FeedItemManagerI::Fids2Feeds(vector<vector<vector<long> > > & fid3d,
//		const hash_map<long,FeedPtr> & feedmap,vector<vector<vector<FeedPtr> > > & feed3d,int begin,int limit ){
//	BOOST_FOREACH(vector<vector<long> > & fid2d, fid3d){
//		vector<vector<FeedPtr> > feed2d;
//		BOOST_FOREACH(vector<long> & fid1d,fid2d){
//			vector<FeedPtr> feed1d;
//			BOOST_FOREACH(long fid, fid1d){
//				hash_map<long,FeedPtr>::const_iterator mit = feedmap.find(fid);
//				if(mit != feedmap.end()){
//					feed1d.push_back(mit->second);
//				}
//			}
//			if(!feed1d.empty()){
//				feed2d.push_back(feed1d);
//			}
//		}
//		if(!feed2d.empty()){
//			feed3d.push_back(feed2d);
//		}
//	}
//}



/*
 void FeedItemManagerI::EnterHotFeed(int uid){
 MCE_INFO("FeedItemManagerI::EnterHotFeed --> uid:" << uid);
 int index = GetIndex(uid);
 {
 IceUtil::Mutex::Lock lock(mutexs_[index]);
 const UserFeed* user_feed = GetUserFeed(uid);
 if(user_feed){
 user_feed->live_feed_.CheckEnterHot(uid, true);
 }
 }
 }*/

/*
 void FeedItemManagerI::CheckEnterAll(){
 MCE_INFO("FeedItemManagerI::CheckEnterAll --> begin");
 int count = 0;
 for(int i=0; i<POOL_SIZE; i++){
 vector<int> ids;
 {
 IceUtil::Mutex::Lock lock(mutexs_[i]);
 SeqIndex& seq_index = pool_[i].get<0> ();
 SeqIndex::iterator it = seq_index.begin();
 for(; it!=seq_index.end(); ++it){
 ids.push_back((*it).userid_);
 }
 }
 count += ids.size();
 for(size_t j=0; j<ids.size(); j++){
 EnterHotFeed(ids.at(j));
 }
 }
 MCE_INFO("FeedItemManagerI::CheckEnterAll --> end " << count);
 }*/

FeedItemSeqSeq FeedItemManagerI::GetFeedItemSeqSeqByUsers(const IntSeq& uids,
    const Ice::Current&) {
  return FeedItemSeqSeq();
}

void FeedItemManagerI::load(int uid, const Ice::Current&) {
	static int level = 0;
	if(uid > -100 && uid <= 0 ){
		MCE_INFO("FeedItemManagerI::load.old level:" << level << " new level:" << -uid);
		level = -uid;
		return;
	}

	if(uid % 10 < level){
		MCE_INFO("FeedItemManagerI::load --> " << uid << " level:" << level );
		GetHotFeedData(uid, 0, 20);
	}else{
		MCE_INFO("FeedItemManagerI::load. uid:" << uid << " above level:" << level)
	}
}

void UpdateInteractionInCacheTimer::handle() {
  SharedFeedPool::instance().UpdateInteraction();
  //CheckEnterHotFeedTimer::instance().SetReady();
}

void UpdateInteractionInDBTimer::handle() {
  MCE_INFO("UpdateInteractionInDBTimer::handle --> begin");
  while (true) {
    set<Ice::Long> ids;
    {
      IceUtil::Mutex::Lock lock(mutex_);
      ids.swap(feedids_);
    }
    MCE_INFO("UpdateInteractionInDBTimer::handle --> size:" << ids.size());
    vector<Ice::Long> tmp_ids;
    set<Ice::Long>::iterator it = ids.begin();
    for (; it != ids.end();) {
      tmp_ids.push_back(*it);
      if (++it == ids.end() || tmp_ids.size() == 1000) {
        SharedFeedPool::instance().UpdateInteraction(tmp_ids);
        tmp_ids.clear();
      }
    }
    IceUtil::Mutex::Lock lock(mutex_);
    if (feedids_.empty()) {
      break;
    }
  }
  MCE_INFO("UpdateInteractionInDBTimer::handle --> end");
}

void GetLogger::Init(){                                                                                                                    
   logger_ = XceLoggerAdapter::instance().getLogger(1, 32768);                                                                                
	 logger_->initialize();
	 path_.push_back("feed");
	 path_.push_back("news");
}
void GetLogger::SendLog(int uid,const FeedDataSeq & feeds){
 		//if(feeds.empty()){
		//	return;
		//}
 		ostringstream oss;
		oss << "n " << uid << " ";
		for(FeedDataSeq::const_iterator it = feeds.begin(); it != feeds.end(); ++it){
		 	oss << (*it)->actor << " " << ((*it)->type & 0xFFFF )<< " ";
		}
		logger_->addLog(path_, oss.str());
		//MCE_INFO("GetLogger::SendLog. log: " << oss.str());
}

/*
 void CheckEnterHotFeedTimer::handle(){
 MCE_INFO("CheckEnterHotFeedTimer::handle --> " << ready);
 if(!ready){
 return;
 }
 FeedItemManagerI::instance().CheckEnterAll();
 ready = false;
 }*/
