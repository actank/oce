#include "talk_cache_client.h"
#include "TalkCacheLoaderAdapter.h"
#include <boost/lexical_cast.hpp>
#include "TalkCache.pb.h"

#include "ServiceI.h"
#include "LogWrapper.h"

namespace xce {
namespace talk {

using namespace com::xiaonei::talk;

static void GetUserKey(Ice::Long fid, char * key, size_t len) {
  snprintf(key, len, "TUSR#%ld", fid);  
  key[len - 1] = 0;
}

Ice::Long GetUserId(const char * key, size_t len) {
  if (len < 5 || strncmp("TUSR#", key, 5) != 0) {
    return -1;
  }

  Ice::Long id = -1;
  try {
    id = boost::lexical_cast<Ice::Long>(string(key + 5, len - 5));
  } catch(boost::bad_lexical_cast &) {
    return -1;
  }
  return id;
}

static void MemcUserToTalkUser(const MemcUser & memc_user, TalkUserPtr talk_user) {
  talk_user->id = memc_user.id();
  talk_user->name = memc_user.name();
  talk_user->statusOriginal = memc_user.status_original();
  talk_user->statusShifted = memc_user.status_shifted();
  talk_user->headurl = memc_user.headurl();
  talk_user->tinyurl = memc_user.tinyurl();
}

TalkCacheClient::TalkCacheClient() {
  pool_.Init(100);
}

TalkCacheClient::~TalkCacheClient() {
}


TalkUserPtr TalkCacheClient::GetUserByIdWithLoad(int userid){
	TalkUserPtr ans = GetUser(userid);
	if(!ans){
		try{
			TalkUserPtr u = TalkCacheLoaderAdapter::instance().LoadById(userid);
			if(u){
				ans = u;
			}
		}
		catch(std::exception& e){
	  	MCE_WARN("TalkCacheClient::GetUserByIdWithLoad --> CALL TalkCacheLoader.LoadById err" << e.what());
  	}catch(...){
	  	MCE_WARN("TalkCacheClient::GetUserByIdWithLoad --> CALL TalkCacheLoader.LoadById err");
  	}
	}
	return ans;
}


TalkUserMap TalkCacheClient::GetUserBySeqWithLoad(int uid, const MyUtil::IntSeq& ids) {
	TalkUserMap ans;
	if(ids.empty()){
		return ans;
	}
	ans = GetUsers(ids);
	MCE_INFO("TalkCacheClient::GetUserBySeqWithLoad --> " << uid << " ids:" << ids.size() << " " << ans.size());
	if(ids.size() == ans.size()){
    return ans;
	}
	TalkUserMap loadans;
	IntSeq loadids;
	TalkUserMap::iterator dit;
	for(MyUtil::IntSeq::const_iterator idit = ids.begin(); idit != ids.end(); ++idit){
		dit = ans.find((*idit));
		if(dit == ans.end()){
			loadids.push_back((*idit));
		}
	}
	if(!loadids.empty()){
		try{
			loadans = TalkCacheLoaderAdapter::instance().LoadBySeq(loadids);
		}
		catch(std::exception& e){
			MCE_WARN("TalkCacheClient::GetUserBySeqWithLoad --> CALL TalkCacheLoader.LoadBySeq err" << e.what());
		}catch(...){
			MCE_WARN("TalkCacheClient::GetUserBySeqWithLoad --> CALL TalkCacheLoader.LoadBySeq err");
		}
		if(!loadans.empty()){
			ans.insert(loadans.begin(), loadans.end());
		}
	}
  return ans;
}

TalkUserMap TalkCacheClient::GetUsers(const MyUtil::IntSeq& ids) {
  float cost_mem;
  float cost_decom;
  vector<string> uids;
  for(size_t i=0; i<ids.size(); i++){
    char data[64] = {0};
    snprintf(data, 64, "TUSR#%d", ids.at(i));
    uids.push_back(data);
  }
  TalkUserMap dict;
  pair<int, MemcachedClient*> cli = pool_.GetClient(ids.at(0)); 
  if(!cli.second){
    return TalkUserMap();
  }
  map<string, string> res;
  map<string, int> flags;
  TimeStat ts;
  bool succ = cli.second->Get(uids, res, flags);
  cost_mem = ts.getTime();
  ts.reset();
  map<string, string>::iterator it = res.begin();
  for(; it!=res.end(); ++it){
    int uid = GetUserId(it->first.c_str(), it->first.size());
    MemcUser memc_user;
    memc_user.ParseFromArray(it->second.c_str(), it->second.size());
    TalkUserPtr talk_user = new TalkUser();
    MemcUserToTalkUser(memc_user, talk_user);
    dict.insert(make_pair(talk_user->id, talk_user));
  }
  cost_decom = ts.getTime();
  pool_.ReleaseClient(cli.first, cli.second);
  MCE_INFO("TalkCacheClient::GetUsers --> ids:" << ids.size() << " res:" << dict.size() 
          << " cost_mem:" << cost_mem << " cost_par:" << cost_decom << " cost_total:" << (cost_mem+cost_decom));
  return dict;
}

TalkUserPtr TalkCacheClient::GetUser(int user_id) {
  MyUtil::IntSeq ids;
  ids.push_back(user_id);
  TalkUserMap dict = GetUsers(ids);
  if(dict.empty()){
    return 0;
  }
  return dict[user_id];
}

// 返回值, 标识是否压缩了
bool TalkCacheClient::SerializeTalkUser(const TalkUserPtr& talk_user, string * serialized) {
  if (!talk_user) {
    return false;
  }

  MemcUser memc_user;
  // TODO : 哪些字段是optional的? 可以优化
  memc_user.set_id(talk_user->id);
  if(!talk_user->name.empty()){
    memc_user.set_name(talk_user->name);
  }
  if(!talk_user->statusOriginal.empty()){
    memc_user.set_status_original(talk_user->statusOriginal);
  }
  if(!talk_user->statusShifted.empty()){
    memc_user.set_status_shifted(talk_user->statusShifted);
  }
  if(!talk_user->headurl.empty()){
    memc_user.set_headurl(talk_user->headurl);
  }
  if(!talk_user->tinyurl.empty()){
    memc_user.set_tinyurl(talk_user->tinyurl);
  }
  memc_user.SerializeToString(serialized);
  return true;
}

bool TalkCacheClient::SetUser(const TalkUserPtr& user) {
  char key[32];
  GetUserKey(user->id, key, 32);
  string value;
  bool b = SerializeTalkUser(user, &value);
  if(!b){
    return false;
  }
  pair<int, MemcachedClient*> cli = pool_.GetClient(user->id); 
  bool res = cli.second->Set(key, value, 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

}
}

