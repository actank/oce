#include "FeedConfigAdapter.h"
#include "FeedDispatcher.h"
#include "FeedDeliver.h"
#include "Date.h"
#include "util/cpp/String.h"
#include "ExpressionCache.h"
#include "TaskManager.h"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <exception>
#include <stdexcept>
#include "WTransitAdapter.h"
#include "FriendRankCacheAdapter.h"
#include "UserStateAdapter.h"
#include "UserConfigAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <ostream>
#include <iostream>
#include <sstream>
#include "FeedNaiveBayesReplicaAdapter.h"
//#include <FeedFocusInvertAdapter.h>
#include <FeedFocusInvertReplicaAdapter.h>
#include "../../FeedMemcProxy/client/feed_interaction_client_n.h"
//#include "PersistenceHelper.h"
#include <sys/resource.h>

using namespace xce::socialgraph;
using namespace xce::feed;
using namespace xce::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace boost;
using namespace ::xce::serverstate;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedDispatcherNI::instance(),
      service.createIdentity(FEED_DISPATCHER, ""));//catagory是空的

  TaskManager::instance().config(ThreadPoolConfig(1, 10));
  TaskManager::instance().scheduleRepeated(new PrintTask());
  TaskManager::instance().scheduleRepeated(&LoadTask::instance());

  //  TaskManager::instance().scheduleRepeated(
  //      &StatTimer::instance());
  LoadTask::instance().handle();
  FeedDeliver::instance().initLogger();
  MCE_INFO("MyUtil::initialize. kFactorTypes " << kFactorTypes);

  //------------------For HA------------------ 
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt(
      "FeedDispatcher.Mod");
  int interval = 5;
  ServerStateSubscriber::instance().initialize("ControllerFeedDispatcherR",&FeedDispatcherNI::instance(), mod, interval, new XceFeedControllerChannel());

//  struct rlimit rl;
//  getrlimit(RLIMIT_CORE, &rl);
//  MCE_INFO("MyUtil::initialize:: --> orgin core limit:" << rl.rlim_max << " " << rl.rlim_cur);
//  rl.rlim_cur = RLIM_INFINITY;
//  setrlimit(RLIMIT_CORE, &rl);
//  getrlimit(RLIMIT_CORE, &rl);
//  MCE_INFO("MyUtil::initialize:: --> now core limit:" << rl.rlim_max << " " << rl.rlim_cur);

}

//--------------------------------------------------------------------------
FeedDispatcherNI::FeedDispatcherNI() {
}

void FeedDispatcherNI::dispatch(const FeedSeedPtr& seed,
    const FeedConfigPtr& config, const Ice::Current& cur) {
  if (!seed) {
    MCE_WARN("FeedDispatcherNI::dispatch --> seed is empty");
    return;
  }
  if (!config) {
    MCE_WARN("FeedDispatcherNI::dispatch --> config is empty");
    return;
  }
  MCE_INFO("dispatch --> feed: " << seed->feed << ",actor:"
      << seed->actor << ",stype:" << (seed->type & 0xffff) << ",source:" << seed->source
      << ",news_merge:" << seed->newsMerge << ",mini_merge:" << seed->miniMerge
      << ",expr:" << config->expr << ",sendConfig:" << config->sendConfig
      << ",newsSendConfig:" << config->sendNewsConfig << ",miniSendConfig:"
      << config->sendMiniConfig << " " << GetIp(cur));
//  if(seed->xml.empty()){
//    MCE_INFO("FeedDispatcherNI::dispatch. xml empty fid:" << seed->feed << " " << GetIp(cur));
//  }

  TaskManager::instance().execute(new DispatchTask(seed, config));
}

void FeedDispatcherNI::dispatchReply(const ReplyDataPtr& replyData,
    const FeedConfigPtr& config, const Ice::Current&) {
  if (!replyData) {
    MCE_WARN("FeedDispatcherNI::dispatchReply --> replyData is empty");
    return;
  }
  if (!config) {
    MCE_WARN("FeedDispatcherNI::dispatchReply --> config is empty");
    return;
  }
  /*
   *    class ReplyData{
   long source;
   int stype;
   int actor;
   int replyCount;
   long replyId;
   int time;
   string xml;
   };
   */

  MCE_INFO("dispatcherReply --> actor" << replyData->actor << ",stype"
      << replyData->stype << ",source:" << replyData->source << ",reply_count:"
      << replyData->replyCount << ",reply_id" << replyData->replyId << ",expr:"
      << config->expr);

  TaskManager::instance().execute(new DispatchReplyTask(replyData, config));
}

static string MakeExpression(LoadType type, int id) {
  ostringstream oss;
  switch (type) {
  case Friend:
    oss << "f";
    break;
  case App:
    oss << "a";
    break;
  case AppFan:
    oss << "A";
    break;
  case Page:
    oss << "p";
    break;
  case HighSchool:
    oss << "s";
    break;
  case Follow:
    oss << "v";
    break;
  case MiniGroup:
    oss << "g";
    break;
  case XiaoZu:
    oss << "z";
    break;
  case MiniSite:
    oss << "x";
    break;
  default:
    throw logic_error("FeedDispatcherNI::Reload ");
  }
  oss << "(" << id << ")";
  return oss.str();
}

void FeedDispatcherNI::Reload(LoadType type, int id, const Ice::Current&) {
  string expr = MakeExpression(type, id);
  //  ostringstream oss;
  //  switch (type) {
  //  case Friend:
  //    oss << "f";
  //    break;
  //  case App:
  //    oss << "a";
  //    break;
  //  case AppFan:
  //    oss << "A";
  //    break;
  //  case Page:
  //    oss << "p";
  //    break;
  //  case HighSchool:
  //    oss << "s";
  //    break;
  //  case Follow:
  //    oss << "v";
  //    break;
  //  case MiniGroup:
  //    oss << "g";
  //    break;
  //  case XiaoZu:
  //    oss << "z";
  //    break;
  //  default:
  //    throw logic_error("FeedDispatcherNI::Reload ");
  //  }
  //  oss << "(" << id << ")";
  MCE_INFO("FeedDispatcherNI::Reload --> factor:" << expr );
  ExprCache::instance().InvalidateFactor(expr);
}

void FeedDispatcherNI::ReloadBatch(LoadType type, const vector<int> & ids,
    const Ice::Current&) {
  for (unsigned i = 0; i != ids.size(); ++i) {
    string expr = MakeExpression(type, ids.at(i));
    MCE_INFO("FeedDispatcherNI::ReloadBatch --> factor:" << expr );
    ExprCache::instance().InvalidateFactor(expr);
  }
}

void FeedDispatcherNI::addBuddyNotify(int host, int guest, const Ice::Current&) {
  //  AppendNewsFeed(host, guest, 10);
  //  AppendNewsFeed(guest, host, 10);
  MCE_INFO("FeedDispatcherNI::addBuddyNotify. host:" << host << " guest:" << guest);
  AppendNewsFeed(host, guest, 600, 1);//日志
  AppendNewsFeed(host, guest, 700, 2);//照片
  AppendNewsFeed(host, guest, 500, 1);//状态类
  AppendNewsFeed(host, guest, 100, 2);//分享

  AppendNewsFeed(guest, host, 600, 1);//日志
  AppendNewsFeed(guest, host, 700, 2);//照片
  AppendNewsFeed(guest, host, 500, 1);//状态类
  AppendNewsFeed(guest, host, 100, 2);//分享
}
void FeedDispatcherNI::joinPageNotify(int page, int fans, const Ice::Current&) {
  MCE_INFO("FeedDispatcherNI::joinPageNotify. page:" << page << " fans:" << fans);
  AppendNewsFeed(page, fans, 5);
}

void FeedDispatcherNI::guideBuddyReplyNotify(int newUser, int buddy,
    const Ice::Current& current) {
  MCE_INFO("FeedDispatcherNI::guideBuddyReplyNotify --> newUser:"<<newUser << ", buddy:"<<buddy << " " << current.con->toString());
  AppendNewsFeed(buddy, newUser, 600, 1);//日志
  AppendNewsFeed(buddy, newUser, 700, 2);//照片
  AppendNewsFeed(buddy, newUser, 500, 1);//状态类
  AppendNewsFeed(buddy, newUser, 100, 2);//分享
}

void FeedDispatcherNI::buddyApplyNotify(int host, int guest,
    const Ice::Current& current) {
  bool
      is_guide =
          !(xce::adapter::userstate::UserStateAdapter::instance().getUserState(
              host)->isGuideDone());
  if (!is_guide) {
    return;
  }
  UserConfigInfoPtr user_config = UserConfigAdapter::instance().getUserConfig(
      guest);
  if (user_config->basicConfig() == 0) {
    return;
  }
  MCE_INFO("FeedDispatcherNI::buddyApplyNotify --> host:" << host << " guest:" << guest);
  AppendNewsFeed(guest, host, 600, 1);//日志
  AppendNewsFeed(guest, host, 700, 2);//照片
  AppendNewsFeed(guest, host, 500, 1);//状态类
  AppendNewsFeed(guest, host, 100, 2);//分享 

}

void FeedDispatcherNI::AppendNewsFeed(int from, int to, int limit) {
  ostringstream expr;
  expr << "u(" << to << ")";

  FeedConfigPtr config = new FeedConfig();
  config->expr = expr.str();
  config->sendConfig = 0; //设定为不发送
  config->sendMiniConfig = 0;
  config->sendNewsConfig = 1;
  config->updateTime = 1;

  ostringstream tab;
  tab << "feed_mini_" << from % 100;
  Statement sql;
  sql << "select * from " << tab.str() << " where user=" << from
      << " order by time desc limit " << limit;//按时间排序，最多只有limit个
  mysqlpp::StoreQueryResult res =
      QueryRunner("feed_db", CDbRServer, tab.str()).store(sql);

  if (!res) {
    return;
  }

  for (size_t i = 0; i < res.num_rows(); ++i) {

    FeedSeedPtr seed = new FeedSeed;

    mysqlpp::Row row = res.at(i);
    seed->feed = row["feed"];
    seed->actor = row["user"];
    seed->baseWeight = 0;
    seed->miniMerge = row["mini_merge"];
    seed->newsMerge = seed->miniMerge;
    seed->psource = 0;
    seed->ptype = 0;
    seed->source = row["source"];
    seed->time = (time_t) mysqlpp::DateTime(row["time"]);
    seed->type = row["type"];

    dispatch(seed, config);
  }
}

void FeedDispatcherNI::AppendNewsFeed(int from, int to, int type, int limit) {
  ostringstream expr;
  expr << "u(" << to << ")";

  FeedConfigPtr config = new FeedConfig();
  config->expr = expr.str();
  config->sendConfig = 0;
  config->sendMiniConfig = 0;
  config->sendNewsConfig = 1;
  config->updateTime = 1;

  ostringstream tab;
  tab << "feed_mini_" << from % 100;
  Statement sql;
  sql << "select * from " << tab.str() << " where user=" << from
      << " and big_type = " << type << " order by time desc limit " << limit;
  mysqlpp::StoreQueryResult res =
      QueryRunner("feed_db", CDbRServer, tab.str()).store(sql);

  if (!res) {
    return;
  }

  for (size_t i = 0; i < res.num_rows(); ++i) {

    FeedSeedPtr seed = new FeedSeed;

    mysqlpp::Row row = res.at(i);
    seed->feed = row["feed"];
    seed->actor = row["user"];
    seed->baseWeight = 0;
    seed->miniMerge = row["mini_merge"];
    seed->newsMerge = seed->miniMerge;
    seed->psource = 0;
    seed->ptype = 0;
    seed->source = row["source"];
    seed->time = (time_t) mysqlpp::DateTime(row["time"]);
    seed->type = row["type"];

    dispatch(seed, config);
  }
}

//--------------------------------------------------------------------------
int DispatchTask::GetAppId(){
  MyUtil::Str2StrMap::iterator i = feed_->extraProps.find("AppId");
  if(i == feed_->extraProps.end()){
    return 0;
  }
  int app_id = 0;
  try{
    app_id = lexical_cast<int>(i->second);
  } catch (Ice::Exception& e) {
    MCE_WARN("DispatchTask::GetAppId exception :" <<e);
    return 0;
  } catch (std::exception& e) {
    MCE_WARN("DispatchTask::GetAppId exception :" << e.what());
    return 0;
  }
  return app_id;
}

void PrintProp(int actor,const MyUtil::Str2StrMap & props){
  ostringstream oss;
  oss << "PrintProp uid:" << actor;
  for(Str2StrMap::const_iterator i = props.begin(); i != props.end();++i){
    oss << " " << i->first << ":" << i->second;
  }
  MCE_INFO(oss.str());
}

bool IsTester(Ice::Int uid){
  return (uid == 347348947 || uid == 242332421 || uid == 220678672);
}

bool IsTestToId(int uid){
	return 		uid == 239556923||
			uid == 		221489137||
			uid == 		238489851||
			uid == 		240599893||
			uid == 		128487631||
			uid == 	225092473||
			uid == 		241610785||
			uid == 	220678672||
			uid == 	322856518||
			uid == 	34344507||
			uid == 	343518722||
			uid == 	347348947||
			uid == 	347349486||
			uid == 	200000032;
}
map<int,int> DispatchTask::GetBayesScores(int actor,const set<int> & target,int stype){

	map<int, int> scores;
	try {
		if (!target.empty() && target.size() < 3000) {
			vector<int> target_v;
			for (set<int>::const_iterator it = target.begin(); it != target.end(); ++it) {
				target_v.push_back(*it);
//				if (*it % 10 < 7 || IsTestToId(*it)) {
//					target_v.push_back(*it);
//				}
			}
			if(!target_v.empty()){
			scores = FeedNaiveBayesAdapter::instance().GetScoreDict(feed_->actor,
					target_v, stype);
			}
			ostringstream os;
			int total_score=0;
			for (map<int, int>::iterator it = scores.begin(); it != scores.end(); ++it) {
//				os << " " << it->first << ":" << it->second;
				total_score += it->second;
			}

			if(actor / 100 % 10 ==0){
			MCE_INFO("DispatchTask::GetBayesScores.Call Bayes. "
					<< " fid:" << feed_->feed << " actor:" << feed_->actor
					<< " target.size:" << target.size()
					<< " target_v.size:" << target_v.size()
					<< " scores size:" << scores.size()
					<< " average score:" << float(total_score) / scores.size()
//					<< " scores:" << os.str()
					);
			}
			return scores;
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("DispatchTask::GetBayesScores  call Bayes Exception"
				<< " fid:" << feed_->feed << " actor:" << feed_->actor
				<< " error:  "<< e);
	}
	return scores;
}

void DispatchTask::GetFocusSource(const FeedSeedPtr & seed,map<int,int> & rank){
	vector<int> sources;
	int actor = seed->actor;
	int stype = seed->type & 0xFFFF;
	long fid = seed->feed;
	int oldsz = rank.size();
	TimeStat ts;
	try{
		sources = FeedFocusInvertReplicaAdapter::instance().GetSources(actor);
		//sources = FeedFocusInvertAdapter::instance().GetSources(actor);
	} catch (Ice::Exception& e) {
		MCE_WARN("DispatchTask::GetFocusSource. actor:" << actor << " fid:" << fid << " stype:" << stype << " error:  "<< e);
		return;
	}catch(...){
		MCE_WARN("DispatchTask::GetFocusSource. actor:" << actor << " fid:" << fid << " stype:" << stype << " unknown error" );
		return;
	}
	BOOST_FOREACH(int source,sources){
		rank[source] = 100;
	}
	MCE_INFO("DispatchTask::GetFocusSource. actor:" << actor << " fid:" << fid << " stype:" << stype
			<< " sources:" << sources.size() << " oldrank:" << oldsz << " newranksize:" << rank.size()
			<< " cost:" << ts.getTime()
	);
}

void ChangeExpr(int actor,int stype,string & expr){
	if(!(stype == 2901 || stype == 2902 || stype == 2906)){
		return;
	}
	int cnt = count(expr.begin(),expr.end(),'f');
	if(cnt == 1){
		ostringstream oss;
		oss << "&f(" << actor << ")";
		expr += oss.str();
	}
}


void DispatchTask::handle () {
  DispatchCost cost;

  TimeStat t1,t2;
  int stype = feed_->type & 0x0000ffff;
  int bigtype = stype / 100;//
  int appid = feed_->newsMerge;//对于特定的type，newsMerge代表appid

  if(stype == 8190){ //对于81XX的新鲜事，psource为appid
    appid = GetAppId();
  }

  if (config_->sendConfig) { //使用数据库里，用户的设置
    //1
    try {
      //MCE_DEBUG("DispatchTask::handle --> Invoke --> ConfigManagerAdapter::instance().GetSendConfig(), actor: " << feed_->actor << ",stype:"<<bigtype << ",appid:"<<appid);
      bool p = ConfigManagerAdapter::instance().GetSendConfig(//现在的SendConfig中只需要查询news的设置
          feed_->actor, bigtype, appid);
      if (!p) {
        config_->sendNewsConfig = p;
        config_->sendMiniConfig = p;//根据产品的需求，miniconfig和newsconfig相等
      }
      MCE_INFO("DispatchTask::handle. call FeedConfig source:" << feed_->source << " type:" << feed_->type << " actor:" << feed_->actor << " sendNewsConfig = " << config_->sendNewsConfig << ", p:"<<p << " appid:" << appid);
    } catch (Ice::Exception& e) {
      MCE_WARN("DispatchTask::handle.ConfigManagerAdapter::instance().GetSendConfig().uid:<< "<< feed_->actor << " fid:" << feed_->feed << "exception "<<e);
    }
  }
  cost.send_cfg_ = t1.getTime();

  set<int> target;
  if (config_->sendNewsConfig) { //需要发送News
    //    if (1) { //TODO del
    MCE_DEBUG("DispatchTask::handle --> sendNewsConfig = true");
    //2
    string oldexpr = config_->expr;
//    ChangeExpr(feed_->actor,stype,config_->expr);
//    MCE_INFO("DispatchTask::handle --> ChangeExpr. fid:" << feed_->feed << " actor = " << feed_->actor << " oldexpr:" << oldexpr << " newexpr:" << config_->expr);
    try {
      target = ExprCache::instance().Expr2Ids(config_->expr, feed_->actor);
      MCE_DEBUG("DispatchTask::handle --> After ExprCache.Expr2Ids ,the target : size = " << target.size());
    } catch (Ice::Exception& e) {
      MCE_WARN("DispatchTask::handle --> ExprCache::instance().Expr2Ids() --> exception :" <<e);
    } catch (std::exception& e) {
      MCE_WARN("DispatchTask::handle --> ExprCache::instance().Expr2Ids() --> exception :" << e.what());
    }
    cost.expr_ = t1.getTime();
    if (config_->sendMiniConfig) {
      int stype = feed_->type & 0xFFFF;
      if (!(stype >= 2901 && stype <= 2904)) {
        target.insert(feed_->actor);
      }
    }
    //(feed_->actor>600000000&&feed_->actor<700000000)
    //3
    //target id可能有很多，分批发送
    const static unsigned int kMaxSize = 5000;
    vector<int> in, partIn, partOut;//in为全部的target，partIn为分批发送的target，大小最大为kMaxSize,
    //partOut为部分的返回的不需要发送target
    in.assign(target.begin(), target.end());
    int i = 0;//表示第几批
    size_t not_recv_cnt = 0;
    for (vector<int>::iterator vit = in.begin(); vit < in.end(); vit
        += kMaxSize, ++i) {
      //MCE_DEBUG("DispatchTask::handle --> INVOKE --> ConfigManagerAdapter::instance().FilterRecvConfig --> part :" << i);
      partIn.assign(vit, vit + kMaxSize < in.end() ? vit + kMaxSize : in.end());
      try {
        partOut = ConfigManagerAdapter::instance().FilterRecvConfig(partIn,
            bigtype, appid);
        // MCE_DEBUG("PartIn size = " << partIn.size() << ", PartOut ids size = " << partOut.size()<< ", bigtype:"<<bigtype << ",appid:" << appid);
        not_recv_cnt += partOut.size();
        for (vector<int>::iterator vit = partOut.begin(); vit != partOut.end(); ++vit) {
          target.erase(*vit);
        }
      } catch (Ice::Exception& e) {
        MCE_WARN("DispatchTask::handle --> Invoke --> ConfigManagerAdapter::instance().uid : "<< feed_->actor << " fid:" << feed_->feed <<" exception "<<e);
      }
    }
    cost.recv_cfg_ = t1.getTime();
    MCE_INFO("DispatchTask::handle --> feedId = " << feed_->feed << ",actor = " << feed_->actor <<
        ",expression = " << config_->expr << ", filter ids size = " << not_recv_cnt << ", result size : " << target.size()
        << " appid:" << appid);
  } else {
    MCE_DEBUG("DispatchTask::handle. sendNewsConfig is false");
  }
  //  if(target.empty()){
  //    return;
  //  }

  //4
  if (bigtype == 29 && (!target.empty())) {
    /*set<int> tmpids;
     if(stype == 2901){
     for(set<int>::iterator it=target.begin(); it!=target.end(); ++it){
     if((*it)%10 == 7){
     tmpids.insert(*it);
     }
     }
     }*/
    //if(stype != 2901){
    //  SpecialFilter::instance().Filt(target);
    //}
    //target.insert(tmpids.begin(), tmpids.end());
  }

  //  if(target.empty()){
  //    return;
  //  }

  //5
  map<int, int> rank;

  try {
    if (!target.empty() && target.size() < 3000) {
    	int total_score=0;
      rank = FriendRankCacheAdapter::instance().GetRevRank(feed_->actor);
			for (map<int, int>::iterator it = rank.begin(); it != rank.end(); ++it) {
//				os << " " << it->first << ":" << it->second;
				total_score += it->second;
			}
			MCE_INFO("DispatchTask::handle. rank .fid:"<< feed_->feed << " average:" << float(total_score)/ rank.size());
    }

  } catch (Ice::Exception& e) {
    MCE_WARN("DispatchTask::handle  Exception:  "<<e);
  }
  cost.friend_ = t1.getTime();

  TimeStat tb;
  map<int, int> scores = GetBayesScores(feed_->actor, target, stype);
	for (map<int, int>::iterator it = scores.begin(); it != scores.end(); ++it) {
		rank[it->first] = it->second;
	}

	MCE_INFO("DispatchTask::handle. call bayes. bayescost:" << tb.getTime());

//	GetFocusSource(feed_,rank);
  
  //6
  //if((feed_->actor==32715654||feed_->actor==234159611||feed_->actor==255329305||feed_->actor==255414584||feed_->actor==256110032||feed_->actor==257876975) &&
  FeedDeliver::instance().deliver(feed_, config_, target, rank,cost);
  
  FeedItem item;
  item.feed = feed_->feed;
  item.type = feed_->type;
  item.time = feed_->time;
  item.weight = feed_->baseWeight;
  item.merge = feed_->newsMerge;
  item.actor = feed_->actor;
  string ext;
  int aid = GetAppId();
  if(aid > 0){
    ostringstream os;
    os << "AppId=" << aid;
    ext = os.str();
  }
  //PersistenceHelper::write(item, ext);
  cost.total_ = t2.getTime();
  InteractionUploader::instance().UploadRange(feed_->feed,target.size());
  MCE_INFO("DispatchTask::handle. fid:"<< feed_->feed <<" source:" << feed_->source << " type:" << stype << " actor:" << feed_->actor
      <<",expr:"<<config_->expr<<",target.size:"<<target.size() << ", friendRank.size:"<< rank.size()
      << " #cost# " << cost.ToString())
  //  string e = config_->expr.empty() ? string(" NULL"):string(config_->expr.begin(), config_->expr.begin() +1);
  //  e = "D_" + e;
  //  StatTimer::instance().Update(feed_->actor,e,t1.getTime(),target.size());
}
//--------------------------------------------------------------------------------------------------
void DispatchReplyTask::handle() {
  if (reply_->actor > 600000000 && reply_->actor < 700000000) {
    MCE_INFO("DispatchReplyTask::handle. not send page reply to IM. source:" << reply_->source << " stype:" << reply_->source << " actor:" << reply_->actor);
    return;
  }
  TimeStat t1;
  //计算表达式
  set<int> target;
  try {
    target = ExprCache::instance().Expr2Ids(config_->expr, reply_->actor);
  } catch (Ice::Exception& e) {
    MCE_WARN("DispatchReplyTask::handle --> ExprCache::instance().Expr2Ids() --> exception :" <<e);
  } catch (std::exception& e) {
    MCE_WARN("DispatchReplyTask::handle --> ExprCache::instance().Expr2Ids() --> exception :" << e.what());
  }
  MCE_INFO("DispatchReplyTask::handle --> expr:" << config_->expr << " size:" << target.size());

  //发送给WTransit
  //  try {
  //    MyUtil::IntSeq target_ids;;
  //    target_ids.assign(target.begin(), target.end());
  //    target_ids.push_back(reply_->actor);//把自己也加入

  MyUtil::IntSeq to_ids;
  to_ids.push_back(reply_->actor);
  for (set<int>::iterator it = target.begin(); it != target.end();) {

    to_ids.push_back(*it++);
    if (to_ids.size() == 40000 || it == target.end()) {
      try {
        talk::adapter::WTransitAdapter::instance().deliverFeedReply(reply_,
            to_ids);
        MCE_INFO("DispatchReplyTask::handle --> Reply to IM, actor:"<< reply_->actor << ",source:" <<reply_->source << ",stype:"<< reply_->stype << ", toid.size:"<<to_ids.size());
        to_ids.clear();
      } catch (Ice::Exception & e) {
        MCE_WARN("DispatchReplyTask::handle --> call WTransit err:"<<e);
        to_ids.clear();
      }
    }
  }

  //  string e = config_->expr.empty() ? string(" NULL"):string(config_->expr.begin(), config_->expr.begin() +1);
  //  e = "R_" + e;
  //  StatTimer::instance().Update(reply_->actor,e,t1.getTime(),target.size());

  //  } catch (Ice::Exception& e) {
  //    MCE_WARN("DispatchReplyTask::handle  Exception:  "<<e);
  //  }

}

//string StatTimer::Dump(){
//  map<string,Record> stat;
//  stat.swap(stat_);
//  ostringstream oss;
//  oss << "index:" << idx_;
//  long targetall = 0,countall = 0;;
//  for(map<string,Record>::iterator i = stat.begin(); i != stat.end();++i){
//    oss << " #type#:" << i->first << " count:" << i->second.count_ << " time:" << i->second.time_ << " target:" << i->second.target_;
//    targetall += i->second.target_;
//    countall += i->second.count_;
//  }
//  oss << " countall:" << countall << " targetall:" << targetall;
//  return oss.str();
//}
string DispatchCost::ToString() {
  ostringstream oss;
//  float send_cfg_,  expr_, recv_cfg_, friend_,  mini_,  log, wtran,news_,interaction,photo;
  oss << " send_cfg:" << send_cfg_ << " expr:" << expr_ << " recv_cfg:" << recv_cfg_ << " friend:" << friend_
      << " mini:" << mini_ << " log:" << log_ << " wtran:" << wtran_ << " news:" << news_
      << " interaction:" << interaction_ << " photo:" << photo_ << " total:" << total_ << " range:" << range_;
  return oss.str();
}


void InteractionUploader::UploadRange(long fid,int r){
	FeedInterPtr inter(new FeedInter);
	inter->SetRange(r);
	try{
		FeedInteractionMemcClientN::instance().SetFeedInter(fid,inter);
  }catch (Ice::Exception& e) {
    MCE_WARN("InteractionUploader::UploadRange. fid:" << fid<< " range:" << r << " err:"<<e);
  } catch (std::exception& e) {
    MCE_WARN("InteractionUploader::UploadRange. fid:" << fid<< " range:" << r << " err:"<<e.what());
  }
}

void InteractionUploader::UploadReply(long fid,int r){
	set<long> fids;
	fids.insert(fid);
	FeedInterMap intermap;
	FeedInteractionMemcClientN::instance().GetFeedInterMap(fids,intermap);
	FeedInterPtr inter;
	FeedInterMap::iterator it = intermap.find(fid);
	if(it != intermap.end() ){
		inter = it->second;
	}
	if(!inter){
		inter = shared_ptr<FeedInter>(new FeedInter);
	}
	inter->SetReply(r);
	FeedInteractionMemcClientN::instance().SetFeedInter(fid,inter);
}

