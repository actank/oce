
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


  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  int index = props->getPropertyAsIntWithDefault("Service.ServerIndex", 0);
//  if(index == 50) index = 0;//TODO 改成index
//  EDMFeedManager::instance().setIndex(index);
//  TaskManager::instance().execute(new EdmLoadTask());

//  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
//  int index = props->getPropertyAsIntWithDefault("Service.ServerIndex", 0);
//  TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());
//  TaskManager::instance().execute(new LoadTask());
//  TaskManager::instance().scheduleRepeated(&ShiftStatTimer::instance());
  TaskManager::instance().scheduleRepeated(&StatTimer::instance());
  TaskManager::instance().schedule(&MaxFidUploader::instance());
	GetLogger::instance().Init();
}

static string GetIp(const Ice::Current& cur){
	if(!cur.con)
		return "";
	string ip = cur.con->toString();
	size_t found = ip.find('\n');
	if (found == string::npos)
		return ip;
	string res ( ip.begin() + found + 1,ip.end());
	return res;
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

void FeedItemManagerI::CheckLoad(int uid, vector<Ice::Long> & fids, FeedContentDict & dict) {
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

  MCE_INFO("FeedItemManagerI::GetFeedContentDict. uid:" << req.uid_
  		<< " request:" << fids.size()
  		<< " load_db:" << load_db
  		<< " memcnt:" << memcnt << " dbcnt:" << dbcnt
  		<< " total:" << fdict.size()
  		<< " memcost:" << memcost << " dbcost:" << dbcost
  );
  return make_pair(memcost,dbcost);
//  MCE_INFO("show fids:" << PrintHelper::Print(fids));
//  if(fids.size() > 100){
//  	MCE_WARN("@@@@@@@@@@ TOOLARGE. uid:" << req.uid_);
//  	PrintHelper::Print(req.item3d_);
//  }
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

set<long> GetNullFeeds(FeedContentDict & fdict){
	set<long> fids;
	BOOST_FOREACH(FeedContentDict::value_type & p,fdict){
		if(!p.second) fids.insert(p.first);
	}
	return fids;
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
	tc.loadtime = ts.Get();

	uinfo->GetFeeds(req);
	tc.mergetime = ts.Get();

	FeedContentDict fdict;
	GetFeedContentDict(req,fdict);
	tc.contenttime = ts.Get();

	FeedBuilderB builder(fdict,req.showed_comment_map_,req.sort_);
	res = builder.BuildFeedData(req);
	tc.buildtime = ts.Get();

	uinfo->SetMaxRead(req.GetMaxFid());
	uinfo->SetReadByIdsExactly(builder.GetNullFids());
//	req.ClearEdms();
	tc.othertime = ts.Get();

	//if(uid == 255233137){
	//	PrintHelper::Print(req.item3d_,uid);
	//}
	
//	req.DumpComments();
//	MCE_INFO("### uid:" << uid << " null fids:" << PrintHelper::Print(GetNullFids(fdict)));
//	MaxFidUploader::instance().Add(uid,uinfo->GetMaxRead());
	uinfo->UploadMaxRead();
	uinfo->RefreshPreMaxRead();
	uinfo->AddRefresh(req.GetNowTime(),req.GetMaxFid());
//	req.SetRefreshes(uinfo->GetRefreshes());
	

	MCE_INFO("FeedItemManagerI::GetFeedDataImpl --> uid:" << uid << " begin:" << begin << " limit:" << limit
			<< " types:" << types.size() << " bystype:" << stype << " hot:" << hot << " feedsize:" << uinfo->Size()
			<< " res:" << builder.GetSucc() 
			<< tc.ToString()
			<< " fdict:" << fdict.size()
			<< " splitter:" << builder.GetSplitter()
			<< " nullfids:" << PrintHelper::Print(builder.GetNullFids())
			<< " types:" << PrintHelper::Print(types)
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
    const Ice::Current&) {
	//if(fitem.time > 0){
	//	MCE_INFO("FeedItemManager::AddFeed. fid:" << fitem.feed << " time:" << fitem.time << " id2weight:" << id2weight.size()
	//			<< " .not add");
	//	return;
	//}
	MCE_INFO("FeedItemManager::AddFeed. begin. fid:" << fitem.feed
			<< " actor:" << fitem.actor
			<< " time:" << fitem.time );

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
			Trace t("FeedItemManagerI::AddFeed");
		}
		uids.push_back(uid);
	}
	EDMFeedManager::instance().addEDMFeed(fitem,uids);

	MCE_INFO("FeedItemManager::AddFeed. fid:" << fitem.feed << " stype:" << hdl->GetStype() 
			<< " actor:" << fitem.actor	
			<< " time:" << fitem.time 
			<< " uids:" << PrintHelper::Print(uids) << " uids in mem:" << PrintHelper::Print(uids_mem));
	return;
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
}

FeedItemSeq FeedItemManagerI::GetOriginalFeedData(int uid, const Ice::Current&) {
	TimeStat ts;

	UserInfoPtr uinfo = LocateUserInfo(uid);
	float locatec = ts.Get();

//	uinfo->LoadFeedCache();
	float maxc = ts.Get();

//	FeedItemSeq res = uinfo->GetOrigFeedItems();
	FeedItemSeq res;
	float get = ts.Get();

	MCE_INFO("FeedItemManagerI::GetOriginalFeedData. uid:" << uid << " size:" << res.size()
			<< " cost-> locate:" << locatec << " max:" << maxc << " get:" << get);
	return res;
}

int FeedItemManagerI::GetUnviewedCount(int uid, int type, const Ice::Current& cur) {
	MCE_INFO("FeedItemManagerI::GetUnviewedCount. uid:" << uid << " type:" << type << " " << GetIp(cur));

	UserInfoPtr uinfo = LocateUserInfo(uid);
	if(!uinfo){
		return 0;
	}
	//现在type没用
	set<int> types;
	types.insert(type);
//	return uinfo->GetUnviewCount(types);
	return 1;
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

