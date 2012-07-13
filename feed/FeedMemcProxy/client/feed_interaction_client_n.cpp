
#include "feed_interaction_client_n.h"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "compress_wrap.h"
// #include "feedfeedInteraction.pb.h"

#include "ServiceI.h"
#include "LogWrapper.h"

using namespace boost;
namespace xce {
namespace feed {

//// libmemcached mget() bug : kEmptyFeedInteractionCount 不可以是空串
//static char * kEmptyFeedInteractionCount = "EMPTY_INTERACTION";
const static string kHead = "INTERACTION#";
//const static string kEmptyValue = "EMPTY";

FeedInteractionMemcClientN::FeedInteractionMemcClientN():kHeadSize_(kHead.size()){
	pool_.Init(250);
}

bool FeedInteractionMemcClientN::GetValues(long idx,const vector<string> & keys,
		map<string,string> & kvs,
		map<string,int> & flags){
	pair<int,MemcachedClient *> cli = pool_.GetClient(idx);
	if(!cli.second)
		return false;
	bool suc = cli.second->Get(keys,kvs,flags);
  pool_.ReleaseClient(cli.first, cli.second);
	return suc;
}

bool FeedInteractionMemcClientN::GetFeedInterMap(const set<long> & fids,
		FeedInterMap & res	){
	vector<string> keys;
	BOOST_FOREACH(long fid,fids){
		keys.push_back(MakeKey(fid));
	}
	map<string,string> kvs;
	map<string,int> flags;
	bool succ = GetValues(*(fids.begin()), keys,kvs,flags);
	Parse(kvs,res);
	return succ;
}
bool FeedInteractionMemcClientN::SetFeedInter(long fid,const FeedInterPtr & inter){
	if(!inter)
		return false;
  string key = MakeKey(fid);
  pair<int, MemcachedClient*> cli = pool_.GetClient(fid);
  bool res = cli.second->Set(key, inter->GetOriginalData(), 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}
long FeedInteractionMemcClientN::ParseFid(const string & key){

	int headsize = kHead.size();
	if(key.size() < kHead.size()){
  	MCE_WARN("FeedInteractionMemcClientN::ParseFid. " << __LINE__);
		return -1;
	}
  if (key.compare(0,kHead.size(),kHead) != 0){//TODO
  	MCE_WARN("FeedInteractionMemcClientN::ParseFid. " << __LINE__);
    return -1;
  }

  Ice::Long fid = -1;
  try {
    fid = boost::lexical_cast<Ice::Long>(key.substr(kHeadSize_ ,key.size() - kHeadSize_));
  } catch(boost::bad_lexical_cast &) {
  	MCE_WARN("FeedInteractionMemcClientN::ParseFid. " << __LINE__);
    return -1;
  }
  return fid;
}

string FeedInteractionMemcClientN::MakeKey(long fid){
	string fidstr = boost::lexical_cast<string>(fid);
	return kHead+fidstr;
}

void FeedInteractionMemcClientN::Parse(const map<string,string> & kvs, FeedInterMap & res){
	typedef map<string,string>::value_type vt;
	BOOST_FOREACH(const vt & p,kvs){
		long fid = ParseFid(p.first);
		MCE_INFO("FeedInteractionMemcClientN::Parse. key:" << p.first << " fid:" << fid);
		if(fid < 0){
			continue;
		}
		FeedInterPtr inter(new FeedInter(p.second));
		res[fid] = inter;
	}
}



}
}

//using namespace xce::feed;
//void Test(){
//	FeedInter itr("212");
//  MYLOG("Test. " << __LINE__);
//	FeedInteractionMemcClientN::instance().SetFeedInter(1000,&itr);
//  MYLOG("Test. " << __LINE__);
//
//}
//int main(int argc, char* argv[]){
//	Test();
//	MYLOG("abc");
//	return 1;
//}
