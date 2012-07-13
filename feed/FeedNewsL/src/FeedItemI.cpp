
#include "FeedItemI.h"
#include "FeedBuilder.h"
#include "client/linux/handler/exception_handler.h"
#include "FeedItemI.h"
#include "FeedCacheAdapter.h"
#include "FeedContentAdapter.h"
#include "IceLogger.h"
#include "ServiceI.h"
#include "PerformanceStat.h"
#include "FeedMemcLoaderAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "XceLoggerAdapter.h"
#include <boost/foreach.hpp>
#include "SharedFeedCache.h"
#include "EDMFeed.h"

using namespace xce::feed;

//bool MyCallback(const char *dump_path, const char *minidump_id, void *context,
//    bool succeeded) {
//  printf("got crash %s %s\n", dump_path, minidump_id);
//  return true;
//}

void MyUtil::initialize() {
//  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedItemManagerI::instance(), service.createIdentity("FIM", ""));
  //controller
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt( "FeedNews.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault( "FeedNews.Interval", 5);
  xce::serverstate::ServerStateSubscriber::instance().initialize( "ControllerFeedNewsR", &FeedItemManagerI::instance(), mod, interval,
      new XceFeedChannel());
  MCE_INFO("MyUtil::initialize. mod:" << mod << " interval:" << interval);

  MCE_INFO("@@@@ HeavyFeed:" << sizeof(HeavyFeed) << " object:" << sizeof(Ice::Object)
  		<< " SharedFeed:" << sizeof(SharedFeed) << " SharedFeedPtr:" << sizeof(SharedFeedPtr)
  );
	TypeFilter::instance().Init();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  int index = props->getPropertyAsIntWithDefault("Service.ServerIndex", 0);
  if(index == 50) {
  	index = 12;//TODO 改成index
  }
  EDMFeedManager::instance().setIndex(index);
  TaskManager::instance().execute(new EdmLoadTask());
//  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
//  int index = props->getPropertyAsIntWithDefault("Service.ServerIndex", 0);
//  TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());
//  TaskManager::instance().execute(new LoadTask());
//  TaskManager::instance().scheduleRepeated(&ShiftStatTimer::instance());
  TaskManager::instance().scheduleRepeated(&StatTimer::instance());
  TaskManager::instance().schedule(&MaxFidUploader::instance());
	GetLogger::instance().Init();
}
void FeedItemManagerI::EraseUser(int uid, const Ice::Current& cur) {
	if(uid < 0){
		MCE_WARN("FeedItemManagerI::EraseUser. uid:" << uid << " uid less than 0 " << GetIp(cur));
		return;
	}

	bool status = UserInfoPool::instance().Erase(uid);
	MCE_INFO("FeedItemManagerI::EraseUser. uid:" << uid << " in mem:" << status << " " << GetIp(cur));
}

//const UserFeed* FeedItemManagerI::GetUserFeed(int uid, bool relocate) {
//  int index = GetIndex(uid);
//  UidIndex& uid_index = pool_[index].get<1> ();
//  UidIndex::iterator it = uid_index.find(uid);
//  if (it != uid_index.end()) {
//    if (relocate) {
//      SeqIndex& seq_index = pool_[index].get<0> ();
//      seq_index.relocate(seq_index.begin(), pool_[index].project<0> (it));
//      it = uid_index.find(uid);
//      if (it != uid_index.end()) {
//        return &(*it);
//      }
//    } else {
//      return &(*it);
//    }
//  }
//  return NULL;
//}
//
//
//
//void FeedItemManagerI::AddUserFeed(const UserFeed& user_feed) {
//  int index = GetIndex(user_feed.userid_);
//  pair<SeqIndex::iterator, bool> p = pool_[index].push_front(user_feed);
//  if (!p.second) {
//    pool_[index].relocate(pool_[index].begin(), p.first);
//  } else if ((int) pool_[index].size() > MAX_USER_COUNT) {
//    pool_[index].pop_back();
//  }
//}

void FeedItemManagerI::CheckLoad(int uid, const vector<Ice::Long> & fids, FeedContentDict & dict) {
  vector<Ice::Long> miss_fids;
  BOOST_FOREACH(long fid, fids){
  	if(!dict.count(fid)) miss_fids.push_back(fid);
  }
  if(miss_fids.empty()) return;

  if (!dict.empty()) {//TODO 可加条件
    try {
      FeedMemcLoaderAdapter::instance().LoadFeedSeq(miss_fids);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedItemManagerI::CheckLoad --> LoadFeedSeq uid:" << uid << " size:" << miss_fids.size() << " err, " << e);
      return;
    }
    MCE_INFO("FeedItemManagerI::CheckLoad --> LoadFeedSeq uid:" << uid << " size:" << miss_fids.size());
  }else{
    FeedContentDict dict_loader;
    try {
      dict_loader= FeedMemcLoaderAdapter::instance().GetFeedDict(miss_fids, 100);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedItemManagerI::CheckLoad --> GetFeedDict uid:" << uid << " size:" << miss_fids.size() << " err, " << e);
      return;
    }
    MCE_INFO("FeedItemManagerI::CheckLoad --> GetFeedDict uid:" << uid << " size:" << miss_fids.size() << " res:" << dict_loader.size());
    dict.insert(dict_loader.begin(),dict_loader.end());
  }
}

pair<float,float> FeedItemManagerI::GetFeedContentDict(GetRequest & req,FeedContentDict & fdict){
  set<long> & fidset = req.GetShowedFids();;
  vector<long> fids(fidset.begin(),fidset.end());

  TimeStat ts;
	try {
    fdict = FeedContentAdapter::instance().getFeedDict(fids);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedItemManagerI::GetFeedContentDict. get memcached fail. uid:" << req.uid_ << " err:" << e);
  }
  int memcnt = fdict.size();
  float memcost = ts.Get();

  bool load_db = false;
  if(fdict.size() < fids.size()){
    CheckLoad(req.uid_,fids,fdict);
    load_db = true;
  }
  int dbcnt = fdict.size() - memcnt;
  float dbcost = ts.Get();

  return make_pair(memcost,dbcost);
}

FeedDataResPtr FeedItemManagerI::GetLiveFeedData(int uid, int begin, int limit,
    const Ice::Current&) {
  return GetFeedDataImpl(uid, IntSeq(), true, begin, limit, false);
}

FeedDataResPtr FeedItemManagerI::GetLiveFeedDataByStype(int uid,
    const IntSeq& stypes, int begin, int limit, const Ice::Current&) {
  return GetFeedDataImpl(uid, stypes, true, begin, limit, false);
}

FeedDataResPtr FeedItemManagerI::GetLiveFeedDataByType(int uid,
    const IntSeq& types, int begin, int limit, const Ice::Current&) {
	return GetFeedDataImpl(uid, types, false, begin, limit, false);
}

FeedDataResPtr FeedItemManagerI::GetHotFeedData(int uid, int begin, int limit,
    const Ice::Current&) {
		return GetFeedDataImpl(uid, IntSeq(), true, begin, limit, true);
}

FeedDataResPtr FeedItemManagerI::GetHotFeedDataByStype(int uid,
    const IntSeq& stypes, int begin, int limit, const Ice::Current&) {
  return GetFeedDataImpl(uid, stypes, true, begin, limit, true);
}

FeedDataResPtr FeedItemManagerI::GetHotFeedDataByType(int uid,
    const IntSeq& types, int begin, int limit, const Ice::Current&) {
  return GetFeedDataImpl(uid, types, false, begin, limit, true);
}

UserInfoPtr FeedItemManagerI::LocateUserInfo(int uid){
//	UserInfoCache * cache = GetCache(uid);
	UserInfoPtr uinfo = UserInfoPool::instance().Find(uid);

	if(uinfo) {
		MCE_INFO("FeedItemManagerI::LocateUserInfo. user in mem. uid:" << uid << " info. " << uinfo->ToString());
		return uinfo;
	}
	MCE_INFO("FeedItemManagerI::LocateUserInfo. user not in mem. uid:" << uid);
 	uinfo = new UserInfo(uid);
 	if(!(uinfo->Init())){
		MCE_WARN("FeedItemManagerI::LocateUserInfo. uid:" << uid << " init fail");
		return 0;
 	}

 	UserInfoPool::instance().Put(uid,uinfo);

	uinfo = UserInfoPool::instance().Find(uid);
	if(!uinfo){
		MCE_WARN("FeedItemManagerI::LocateUserInfo. uid:" << uid << " locate fail");
	}
//	MCE_INFO("@@@ " << uinfo->ToString() << " ptr:"<< uinfo.get());
	return uinfo;
}

UserInfoPtr FeedItemManagerI::FindUserInfo(int uid){
//	UserInfoCache * cache = GetCache(uid);
	return UserInfoPool::instance().Find(uid);
}

//UserInfoPtr FeedItemManagerI::LocateUserInfo(int uid){
//	UserInfoCache * cache = GetCache(uid);
//	UserInfoPtr uinfo = cache->find(uid,0);//不是LRU，只能用find
//
//	if(uinfo) {
//		MCE_INFO("FeedItemManagerI::LocateUserInfo. user in mem. uid:" << uid << " info. " << uinfo->ToString()
//				<< " ptr:" << uinfo.get()
//				<< " cache index:"<< GetIndex(uid) <<" size:" << cache->size());
//		return uinfo;
//	}
//	MCE_INFO("FeedItemManagerI::LocateUserInfo. user not in mem. uid:" << uid
//			<< " cache index:"<< GetIndex(uid) <<" size:" << cache->size());
// 	uinfo = new UserInfo(uid);
// 	if(!(uinfo->Init())){
//		MCE_WARN("FeedItemManagerI::LocateUserInfo. uid:" << uid << " init fail");
//		return 0;
// 	}
//
//	cache->add(uinfo, uid);
//
//	uinfo = cache->find(uid,0);
//	if(!uinfo){
//		MCE_WARN("FeedItemManagerI::LocateUserInfo. uid:" << uid << " find fail");
//	}
////	MCE_INFO("@@@ " << uinfo->ToString() << " ptr:"<< uinfo.get());
//	return uinfo;
//}

class GetCost{
public:
	float loadtime,mergetime,buildtime,totaltime,contenttime,othertime;
	string ToString(){
		ostringstream os;
		os << " loadtime:" << loadtime
				<< " mergetime:" << mergetime
				<< " buildtime:" << buildtime
				<< " totaltime:" << loadtime+mergetime+buildtime+contenttime+othertime
				<< " contenttime:" << contenttime
				<< " othertime:" << othertime
				;
		return os.str();
	}
//
//	float locate,max,above,edm,merge,dictmem,dictdb,build,setmax,null,rmedm;
//	string ToString(){
//		ostringstream os;
//		os << " locate:" << locate << " max:" << max << " above:" << above
//				<< " edm:" << edm
//				<< " merge:" << merge
//				<< " dictmem:" << dictmem << " dictdb:" << dictdb
//				<< " build:" << build << " setmax:" << setmax << " null:" << null<<" edm:" << rmedm
//				<< " totaltime:" << locate+max+above+edm+merge+dictmem+dictdb+build+setmax+null+rmedm;
//		return os.str();
//	}
};

bool FeedItemManagerI::CheckArgs(int uid, const IntSeq& types,
    bool stype, int begin, int limit, bool hot){
	if(uid < 0 || begin < 0 || limit <= 0){
		MCE_WARN("FeedItemManagerI::CheckArgs. arguments invalid. uid:" << uid
				<< " types:" << PrintHelper::Print(types) << " begin:" << begin
				<< " limit:" << limit << " hot:" << hot);
		return false;
	}
	return true;
}

set<long> GetNullFids(FeedContentDict & fdict){
	set<long> fids;
	BOOST_FOREACH(FeedContentDict::value_type & vt, fdict){
		if(!(vt.second)){
			fids.insert(vt.first);
		}
	}
	return fids;
}

static int MakeUnviewCount(int count){
	int flag = count > 0?1:0;
	return flag << 31|count;
}
static int GetOlderCount(int UnviewCount){
	return UnviewCount & 0x7FFFFFFF;
}

static bool HasOlder(int UnviewCount){
	return UnviewCount & 0x80000000;
}

FeedDataResPtr FeedItemManagerI::GetFeedDataImpl(int uid, const IntSeq& types,
    bool stype, int begin, int limit, bool hot) {
	FeedDataResPtr res = new FeedDataRes;

	try{
	TimeStat ts; GetCost tc;
	GetRequest req(uid,types,stype,begin,limit,hot);

	if(!req.CheckArgs()) return res;


	UserInfoPtr uinfo = LocateUserInfo(uid);
	if(!uinfo) return res;
//	long pre_max_read = uinfo->GetMaxRead();
	tc.loadtime = ts.Get();

	uinfo->MergeAndSort(req);
	tc.mergetime = ts.Get();
	//PrintHelper::Print(req.hfeed3d_,uid);
	FeedContentDict fdict;
	pair<float,float> p = GetFeedContentDict(req,fdict);
	tc.contenttime = ts.Get();

	FeedBuilder builder(uid,fdict,req.showed_comment_map_,req.sort_);
	res = builder.BuildFeedData(req);
	res->UnviewedCount = MakeUnviewCount(req.GetOlderCount());
	tc.buildtime = ts.Get();


	uinfo->SetReadByIdsExactly(builder.GetNullFids());
	req.ClearEdms();
	tc.othertime = ts.Get();

	bool maxchanged = uinfo->SetMaxRead(req.GetMaxFid());
	if(maxchanged)
		MaxFidUploader::instance().Add(uid,uinfo->GetMaxRead());
	
	if(req.GetMaxFid() > 0 and req.MustIncViewBelowFid())
		uinfo->IncViewBelowFid(req.GetMaxFid());

	MCE_INFO("FeedItemManagerI::GetFeedDataImpl --> uid:" << uid << " begin:" << begin << " limit:" << limit
			<< " types:" << types.size() << " bystype:" << stype << " hot:" << hot << " feedsize:" << uinfo->Size()
			<< " res:" << res->data.size()
			<< tc.ToString()
			<< " fdict:" << fdict.size()
			<< " nullfids:" << builder.GetNullFids().size()
//			<< " nullfids:" << PrintHelper::Print(builder.GetNullFids())
			<< " maxchanged:" << maxchanged
			<< " maxreadfid:" << uinfo->GetMaxRead()
			<< " older_count:" << GetOlderCount(res->UnviewedCount)
			<< " has_older:" << HasOlder(res->UnviewedCount)
			<< " show_fids:" << req.GetShowedFids().size()
			<< " useless_count:" << req.GetUselessCount()
			<< " types:" << PrintHelper::Print(types)
//		<< " include_types:" << PrintHelper::Print(req.GetIncludeTypes())
			<< " exclude_types:" << PrintHelper::Print(req.GetExcludeTypes())
//			<< " uinfo:" << uinfo->ToString()
	);

	return res;

	} catch(Ice::Exception& e) {
		MCE_WARN(" FeedItemManagerI::GetFeedDataImpl uid:" << uid
				<<" error:" << e);
		return res;
	}
}

void FeedItemManagerI::AddFeed(const FeedItem& fitem,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight,
    const Ice::Current& cur) {
	//if(fitem.time > 0){
	//	MCE_INFO("FeedItemManager::AddFeed. fid:" << fitem.feed << " time:" << fitem.time << " id2weight:" << id2weight.size()
	//			<< " .not add");
	//	return;
	//}

	SharedFeedPtr hdl = SharedFeedCache::instance().Add(fitem);

	vector<int> uids, uids_mem;

	BOOST_FOREACH(const MyUtil::Int2IntMap::value_type & vt, id2weight){
		int uid = vt.first;
		UserInfoPtr uinfo = FindUserInfo(uid);
		if(uinfo){
			HeavyFeed hfeed(fitem,hdl);
			hfeed.SetUpdateTime(config->updateTime);
			hfeed.SetWeight(vt.second);
			uinfo->AddHeavyItem(uid,hfeed);
			uids_mem.push_back(uid);
		}
		if(uid == 220678672
				|| uid == 347348947	){
			MCE_INFO("FeedItemManager::AddFeed. @@. target:" << uid << " fid:" << fitem.feed  << " actor:" << fitem.actor << " weight:" << vt.second);	
		}
		uids.push_back(uid);
	}

	EDMFeedManager::instance().addEDMFeed(fitem,uids);

	MCE_INFO("FeedItemManager::AddFeed. fid:" << fitem.feed << " stype:" << hdl->GetStype() 
			<< " actor:" << fitem.actor	
			<< " time:" << fitem.time 
			<< " mg:" << fitem.merge
			<< " uids_size:" << uids.size()
			<< " uids:" << PrintHelper::Print(uids)
			<< " uids_mem size:" << uids_mem.size()
			<< " uids_mem:" << PrintHelper::Print(uids_mem));
}


void FeedItemManagerI::SetRead(int uid, int stype, Ice::Long merge,
    const Ice::Current&) {
	UserInfoPtr uinfo = FindUserInfo(uid);
	vector<long> fids;
	if(uinfo) {
		fids = uinfo->SetRead(stype,merge);
	}
	MCE_INFO("FeedItemManagerI::SetRead. uid:" << uid << " stype:" << stype
			<< " merge:" << merge << " in mem:" << boolalpha <<(uinfo != 0)
			<< " removed fids:" << PrintHelper::Print(fids));
}

void FeedItemManagerI::SetReadById(int uid, Ice::Long fid,
    const Ice::Current& cur) {
	UserInfoPtr uinfo = FindUserInfo(uid);
	vector<long> fids;
	if(uinfo) {
		fids = uinfo->SetReadById(fid);
	}
	MCE_INFO("FeedItemManagerI::SetRead. uid:" << uid << " fid:" << fid
			<< " in mem:" << boolalpha <<(uinfo != 0)
			<< " removed fids:" << PrintHelper::Print(fids)
			<< " " << GetIp(cur)
	);
}

void FeedItemManagerI::SetReadAll(int uid, const Ice::Current& cur) {
	UserInfoPtr uinfo = FindUserInfo(uid);
	if(uinfo) {
		uinfo->SetReadAll();
	}
  MCE_INFO("FeedItemManagerI::SetReadAll. uid:" << uid
  		<< " in mem:" << (uinfo != 0)
			<< " " << GetIp(cur)
  ) ;
}

FeedDataSeq FeedItemManagerI::GetFeedDataByIds(const MyUtil::LongSeq& fids,
    const Ice::Current& cur) {
  FeedContentDict dict;
  try {
    dict = FeedContentAdapter::instance().getFeedDict(fids);
    CheckLoad(0,fids,dict);
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

  MCE_INFO("FeedItemManagerI::GetFeedDataByIds --> size:" << fids.size() << " fids:"
  		<< " fids:" << PrintHelper::Print(fids) << " res size:" << seq.size()
			<< " " << GetIp(cur)
  );
  return seq;
}

FeedDataSeq FeedItemManagerI::GetFeedDataByIdsWithUid(int uid,
    const MyUtil::LongSeq& fids, Ice::Long first, int stype,
    const Ice::Current&) {
	MCE_WARN("FeedItemManagerI::GetFeedDataByIdsWithUid. uid:" << uid << " fids:" << PrintHelper::Print(fids)
	<< " first:" << first << " stype:" << stype << " this function should not be called");
	FeedDataSeq res;//接口没调用
	return res;
//  MCE_INFO("FeedItemManagerI::GetFeedDataByIdsWithUid. uid:" << uid << " fids size:" << fids.size() <<" first" << first << " stype:" << stype);
//  if (!MergeConfig::instance().IsComment(stype)
//      && !MergeConfig::instance().IsMain(stype)) {
//    MCE_DEBUG("redirect to GetFeedDataByIds");
//    return GetFeedDataByIds(fids);
//  }
//  MCE_DEBUG("Are merge stype");
//
//  float load_cost/*, build_cost, merge_cost*/;
//  TimeStat ts;
//  int index = GetIndex(uid);
//  bool load = true;
//  vector<vector<vector<SharedFeedPtr> > > feeds;
//  {
//    IceUtil::Mutex::Lock lock(mutexs_[index]);
//    //EraseUserFeed(uid);//TODO 待删除
//    const UserFeed* user_feed = GetUserFeed(uid);
//    if (user_feed) {
//      load = false;
//    }
//  }
//
//  TimeStat ts1;
//  if (load) {
//    LoadFeed(uid);
//  }
//  int /*unviewed_count = 0, */edm_count = 0;
//  load_cost = ts1.Get();
//  ts1.reset();
//
//  {
//    IceUtil::Mutex::Lock lock(mutexs_[index]);
//    const UserFeed* user_feed = GetUserFeed(uid);
//    if (user_feed) {
//      int unviewed_count;
//      bool hot = true;
//      vector<vector<vector<SharedFeedPtr> > > random;
//      feeds = user_feed->unique_feed_list_.GetGatheredFeed(uid, 0, 250, hot, random,
//          unviewed_count, edm_count);
//      //      feeds = user_feed->live_feed_.GetGatheredFeed(uid, begin, limit, hot,
//      //          random, unviewed_count, edm_count);
//    }
//  }
//  set<Ice::Long> fid_set(fids.begin(), fids.end());
//
//  int x = -1;
//  for (size_t i = 0; i != feeds.size(); ++i) {
//    for (size_t j = 0; j != feeds.at(i).size(); ++j) {
//      for (size_t k = 0; k != feeds.at(i).at(j).size(); ++k) {
//        if (fid_set.count(feeds.at(i).at(j).at(k)->feed_id())) {
//          x = i;
//          goto finish;
//        }
//      }
//    }
//  }
//
//  finish:
//
//  MCE_DEBUG("GetFeedDataByIdsWithUid. x:"<< x);
//  if (x == -1) {
//    MCE_DEBUG("GetFeedDataByIdsWithUid. can not find");
//    return FeedDataSeq();
//  }
//
//  vector<vector<vector<SharedFeedPtr> > > new_feeds;
//  MyUtil::LongSeq feed_ids;
//
//  for (size_t j = 0; j != feeds.at(x).size(); ++j) {
//    MyUtil::LongSeq tmp_ids;
//    bool is_first = false;
//    for (size_t k = 0; k != feeds.at(x).at(j).size(); ++k) {
//      Ice::Long feedid = feeds.at(x).at(j).at(k)->feed_id();
//      if (feedid != first) {
//        tmp_ids.push_back(feedid);
//      } else {
//        is_first = true;
//      }
//    }
//    if (!is_first) {
//      feed_ids.insert(feed_ids.end(), tmp_ids.begin(), tmp_ids.end());
//      vector<vector<SharedFeedPtr> > tmp;
//      tmp.push_back(feeds.at(x).at(j));
//      new_feeds.push_back(tmp);
//    }
//  }
//  MCE_DEBUG("GetFeedDataByIdsWithUid.  feed_ids:" << PrintHelper::Print(feed_ids));
//
//  FeedContentDict fdict;
//  try {
//    TimeStat ts;
//    fdict = FeedContentAdapter::instance().getFeedDict(feed_ids);
//  } catch (Ice::Exception& e) {
//    MCE_WARN("FeedItemManagerI::GetFeedDataByIdsWithUid --> call FeedContent.getFeedDict err:" << e);
//    return FeedDataSeq();
//  }
//
//  FeedDataSeq res;
//  for (size_t i = 0; i < new_feeds.size(); i++) {
//    FeedDataPtr tmp = BuildAFeed(uid, new_feeds.at(i), fdict);
//    res.push_back(tmp);
//  }

}

FeedItemSeq FeedItemManagerI::GetOriginalFeedData(int uid, const Ice::Current&) {
	TimeStat ts;

	UserInfoPtr uinfo = LocateUserInfo(uid);
	float locatec = ts.Get();

//	uinfo->LoadFeedCache();

	FeedItemSeq res = uinfo->GetOrigFeedItems();
	float get = ts.Get();

	MCE_INFO("FeedItemManagerI::GetOriginalFeedData. uid:" << uid << " size:" << res.size()
			<< " locate:" << locatec  << " get:" << get);
	return res;
}

int FeedItemManagerI::GetUnviewedCount(int uid, int type, const Ice::Current& cur) {
	UserInfoPtr uinfo = LocateUserInfo(uid);
	if(!uinfo){
		MCE_INFO("FeedItemManagerI::GetUnviewedCount. uid:" << uid << " type:" << type << " locate uinfo fail " << GetIp(cur));
		return 0;
	}
	//现在type没用
	set<int> types;
	types.insert(type);
	int cnt = uinfo->GetUnviewCount(types);
	MCE_INFO("FeedItemManagerI::GetUnviewedCount. uid:" << uid << " type:" << type << " cnt:" << cnt <<" " << GetIp(cur));
	return cnt;
}
map<int,int>	FeedItemManagerI::GetCountMapByStype(int uid, const vector<int> & stypes, bool unviewed, const Ice::Current& cur ){
	UserInfoPtr uinfo = LocateUserInfo(uid);
	if(!uinfo){
		MCE_INFO("FeedItemManagerI::GetCountMapByStype. uid:" << uid << " types:" << PrintHelper::Print(stypes) << " locate uinfo fail " << GetIp(cur));
		return map<int,int>();
	}
	set<int> typeset(stypes.begin(),stypes.end());
	map<int,int> cntmap = uinfo->GetCountByStypeMerged(typeset,unviewed);
	MCE_INFO("FeedItemManagerI::GetCountMapByStype. uid:" << uid << " types:" << PrintHelper::Print(stypes) << " cntmap:" << cntmap.size() <<" " << GetIp(cur));
	return cntmap;
}

FeedItemSeqSeq FeedItemManagerI::GetFeedItemSeqSeqByUsers(const IntSeq& uids,
    const Ice::Current&) {
	MCE_INFO("FeedItemManagerI::GetFeedItemSeqSeqByUsers. uids:" << PrintHelper::Print(uids));
  return FeedItemSeqSeq();
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

void FeedItemManagerI::load(int uid, const Ice::Current& cur) {
//	MCE_INFO("FeedItemManagerI::load. input: uid:" << uid);
	static int level = 0;
	if(uid > -100 && uid <= 0 ){
		MCE_INFO("FeedItemManagerI::load.old level:" << level << " new level:" << -uid );
		level = -uid;
		return;
	}

	if(uid % 20 != 0){
		return ;
	}

	for(int i =0; i < level; ++i){
		MCE_INFO("FeedItemManagerI::load --> " << uid << " level:" << level );
		GetHotFeedData(uid, 0, 20);
	}

//	if(uid /100 % 10 < level){
//		MCE_INFO("FeedItemManagerI::load --> " << uid << " level:" << level  );
//		GetHotFeedData(uid, 0, 20);
//	}
}
void StatTimer::handle(){
		MCE_INFO("@@@@ StatTimer." << UserInfoPool::instance().ToString()
				<< " " << SharedFeedCache::instance().ToString());
}

