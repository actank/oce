/*
 * FeedAssistantI.cpp
 *
 *  Created on: Dec 24, 2009
 *      Author: antonio
 */

#include "ServiceI.h"
#include "FeedAssistantI.h"
#include "Tasks.h"
#include "DbHelper.h"
//#include "FeedContentAdapter.h"
//#include "FeedAssistantNReplicaAdapter.h"
//#include "FeedAssistantTestAdapter.h"
#include "FeedAssistantNReplicaAdapter.h"
//#include "FeedAssistantNAdapter.h"
#include "FeedStatAdapter.h"
#include "UserPassportAdapter.h"
#include "UserDangerAdapter.h"
#include "FeedItemFacadeReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <sys/resource.h>

using namespace com::xiaonei::xce;
using namespace xce::adapter::userpassport;
using namespace xce::feed;
using namespace xce::userdanger;
using namespace std;
using namespace MyUtil;
using namespace Ice;
using namespace ::xce::serverstate;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedAssistantI::instance(),
      service.createIdentity(FEED_ASSISTANT, ""));
 
  IS_ONLINE_ENV = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(
      "FeedAssistant.IS_ONLINE_ENV", 1);
 
  TaskManager::instance().scheduleRepeated(new StatTimer());
  FeedAssistantI::instance().ReloadAllConfig();
  //TaskManager::instance().scheduleRepeated(new LoadBlockUserTimer());

 ////////// HA  ////////////////
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FeedAssistant.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedAssistant.Interval", 5);
  ServerStateSubscriber::instance().initialize("ControllerFeedAssistantR",&FeedAssistantI::instance(),mod,interval, new XceFeedControllerChannel);
  MCE_INFO("mod:" << mod << " interval:" << interval);

  struct rlimit rl;
  getrlimit(RLIMIT_CORE, &rl);
  MCE_INFO("MyUtil::initialize:: --> orgin core limit:" << rl.rlim_max << " " << rl.rlim_cur);
  rl.rlim_cur = RLIM_INFINITY;
  setrlimit(RLIMIT_CORE, &rl);
  getrlimit(RLIMIT_CORE, &rl);
  MCE_INFO("MyUtil::initialize:: --> now core limit:" << rl.rlim_max << " " << rl.rlim_cur);
}
//---------------------------------------------------------------------------
void IndexCache::add(const FeedIndexPtr& index) {
  if (!index) {
    return;
  }

  //Refresh the container
  IceUtil::Mutex::Lock lock(_mutex);
  std::pair<IndexContenter::iterator, bool> p = _container.push_front(index); //push front

  if (!p.second) {
    _container.relocate(_container.begin(), p.first); /* put in front */
    return;
  } else {
    _kick();
  }
}
void IndexCache::add(const FeedIndexSeq& indexSeq) {
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    add(indexSeq.at(i));
  }
}
void IndexCache::removeFeedByIds(const MyUtil::LongSeq& feeds) {
  IceUtil::Mutex::Lock lock(_mutex);

  IdIndex& index = _container.get<1> ();
  for (size_t i = 0; i < feeds.size(); ++i) {
    index.erase(feeds.at(i));
  }
}

FeedIndexSeq IndexCache::getFeedIndexSeq(Ice::Long source, Ice::Int stype,
    Ice::Int actor) {
  IceUtil::Mutex::Lock lock(_mutex);
  SourceStypeActorIndex& index = _container.get<2> ();

  pair<SourceStypeActorIndex::iterator, SourceStypeActorIndex::iterator> p =
      index.equal_range(boost::make_tuple(source, stype, actor));

  FeedIndexSeq seq;
  while (p.first != p.second) {
    seq.push_back(*p.first);
    ++p.first;
  }
  return seq;
}

int IndexCache::size() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _container.size();
}

void IndexCache::_kick() {

  SeqIndex& index = _container.get<0> ();
  while (_container.size() > CACHE_SIZE) {
    SeqIndex::iterator it = index.end();
    --it;
    Ice::Long source = (*it)->source;
    Ice::Int stype = (*it)->stype;
    Ice::Int actor = (*it)->actor;

    SourceStypeActorIndex & index2 = _container.get<2> ();
    pair<SourceStypeActorIndex::iterator, SourceStypeActorIndex::iterator> p =
        index2.equal_range(boost::make_tuple(source, stype, actor));
    index2.erase(p.first, p.second);
  }
}

//---------------------------------------------------------------------------

/*
 * remove need delete feed_index, feed_content, feed_mini
 *
 *
 */

void FeedAssistantI::removeFeedById(Ice::Long feedId,
    const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::removeFeedById --> feedId = " << feedId << " " << current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().removeFeedById(feedId);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::removeFeedById --> feedId = " << feedId << " err, " << e);
  }
  //TaskManager::instance().execute(new RemoveFeedByIdTask(feedId));
}
void FeedAssistantI::removeFeedBySource(Ice::Long source, Ice::Int stype,
    const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::removeFeedBySource --> source = " << source << "stype = " << stype << " " << current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().removeFeedBySource(source, stype);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::removeFeedBySource --> source = " << source << "stype = " << stype << " err, " << e);
  }
  //	TaskManager::instance().execute(new RemoveFeedBySourceStypeTask(source,
  //			stype));
}
void FeedAssistantI::removeFeedByParent(Ice::Long parent, Ice::Int ptype,
    const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::removeFeedByParent --> parent = " << parent << ",ptype = " << ptype << " " << current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().removeFeedByParent(parent, ptype);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::removeFeedByParent --> parent = " << parent << ",ptype = " << ptype << " err, " << e);
  }
  //	TaskManager::instance().execute(new RemoveFeedByParentTask(parent, ptype));
}
void FeedAssistantI::removeFeed(Ice::Long source, Ice::Int stype,
    Ice::Int actor, const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::removeFeed --> source = " << source << ",stype = " << stype << ",actor = " << actor << " " << current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().removeFeed(source, stype, actor);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::removeFeed --> source = " << source << ",stype = " << stype << ",actor = " << actor << " err, " << e);
  }
  //	TaskManager::instance().execute(new RemoveFeedBySourceStypeActorTask(
  //			source, stype, actor));
}

void FeedAssistantI::removeFeedReply(Ice::Long source, Ice::Int stype,
    Ice::Int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply,
    const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::removeFeedReply --> source = " << source << ",stype = " << stype
      << ",actor = " << actor << ",removeReplyId = "<< removeReplyId << " " << current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().removeFeedReply(source, stype, actor,
        removeReplyId, reply);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::removeFeedReply --> source = " << source << ",stype = " << stype << " err, " << e);
  }
  /*
   MCE_INFO("FeedAssistantI::_replaceFeedReply  source="<<source<<"  stype="<<stype<<" actor="<<actor);
   FeedIndexSeq indexSeq = IndexCache::instance().getFeedIndexSeq(source,
   stype, actor);
   MyUtil::LongSeq feedIds;
   for (size_t i = 0; i < indexSeq.size(); ++i) {
   feedIds.push_back(indexSeq.at(i)->feed);
   }
   if (indexSeq.empty()) {

   indexSeq = DbHelper::instance().getFeedIndex(stype, source, actor);
   if (indexSeq.empty()) {
   MCE_INFO("FeedAssistantI::removeFeedReply ---> can not get feed, source:"<<source<<" actor:"<<actor << " stype:"<<stype);
   return;
   }
   for (size_t h = 0; h < indexSeq.size(); ++h) {
   feedIds.push_back(indexSeq.at(h)->feed);
   }
   IndexCache::instance().add(indexSeq);
   }
   try {
   FeedContentAdapter::instance().replaceFeedReply(feedIds, reply);
   } catch (Ice::Exception& e) {
   MCE_WARN("FeedAssistantI::_replaceFeedReply --> call FeedContentAdapter replaceFeedReply  err:"<<e);
   }
   //todo  remove notify to IM
   */
}

void FeedAssistantI::removeFeedReplyById(int stype, Ice::Long feedid,
    Ice::Long removeReplyId, const FeedReplyPtr& reply,
    const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::removeFeedReplyById --> stype = " << stype << ",feedid = " << feedid << ",removeReplyId = " << removeReplyId << " " << current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().removeFeedReplyById(stype, feedid,
        removeReplyId, reply);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::removeFeedReplyById --> stype = " << stype << ",feedid = " << feedid << ",removeReplyId = " << removeReplyId << " err, " << e);
  }
}

void FeedAssistantI::readNews(Ice::Int user, Ice::Long feed,
    const Ice::Current& current) {

  MCE_INFO("FeedAssistantI::readNews --> user:"<<user << " feedid:"<<feed << " " << current.con->toString());
  TaskManager::instance().execute(new ReadNewsTask(user, feed));
}
void FeedAssistantI::readAllNews(Ice::Int user, const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::readAllNews --> user = " << user << " " << current.con->toString());

  TaskManager::instance().execute(new ReadNewsTask(user));
}

void FeedAssistantI::readMini(Ice::Int user, Ice::Long feed,
    const Ice::Current& current) {

  MCE_INFO("FeedAssistantI::readMini --> user:"<<user << " feedid:"<<feed << " " << current.con->toString());
  TaskManager::instance().execute(new ReadMiniTask(user, feed));
}
void FeedAssistantI::readAllMini(Ice::Int user, const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::readAllMini --> user:"<<user << " " << current.con->toString());
  TaskManager::instance().execute(new ReadMiniTask(user));
}

void FeedAssistantI::dispatch(const FeedSeedPtr& seed,
    const Ice::Current& current) {

//  if(seed->actor == 257876975 || seed->actor == 128487631) FeedAssistantTestAdapter::instance().dispatch(seed);
  
  if (seed) {
    bool danger_user = false;
    try {
      danger_user = UserDangerAdapter::instance().isNotFeed(seed->actor);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedAssistantI::dispatch --> call UserDanger err, " << e);
    }
    if (danger_user) {
      MCE_WARN("FeedAssistantI::dispatch --> user:" << seed->actor << " is block");
      return;
    }

    try {
      int safe_status = UserPassportAdapter::instance().getUserPassport(
          seed->actor)->safestatus();
      if (safe_status & 0x7c0) {
        MCE_WARN("FeedAssistantI::dispatch --> block user:"<<seed->actor);
        return;
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedAssistantI::dispatch --> call UserPassportAdapter err:"<<e);
    }
    //------丢掉一半的8189 APP的feed
    static bool send_app_feed = true;
    int stype = seed->type & 0xFFFF;
    if (stype == 8189) {
      send_app_feed = !send_app_feed;
      if (!send_app_feed) {
        MCE_INFO("FeedAssistantI::dispatch, not dispatch  actor:" << seed->actor << " stype:" << stype << " source:" << seed->source << " news_merge:"<< seed->newsMerge);
        return;
      }
    }
    //---------------------------

    seed->feed = -1;

    //		if (seed->actor == 128487631 || seed->actor == 238489851 || (seed->type
    //				& 0xffff) == 8189) {
    seed->feed = DbHelper::instance().createId();
    if (seed->feed <= 0) {
      return;
    }
    if(seed->miniMerge == -1){
    	seed->miniMerge = seed->feed;
    }
    if(seed->newsMerge == -1){
	seed->newsMerge = seed->feed;
    }

    //int stype = (seed->type&0xFFFF);
    Str2StrMap::iterator it = seed->extraProps.find("save_db");
    if(it == seed->extraProps.end()){
	    if (_notSavingStype.count(stype)) {
		    seed->extraProps["save_db"] = "0";
	    } else {
		    seed->extraProps["save_db"] = "1";
	    }
    }

			MCE_INFO("line:" << __LINE__);
    //MCE_INFO("FeedAssistantI::dispatch  --> feed:" << seed->feed ); //<< " actor:" << seed->actor << " stype:" << (seed->type&0xFFFF)
MCE_INFO("FeedAssistantI::dispatch  --> feed:" << seed->feed << " actor:" << seed->actor << " stype:" << ((seed->type)&0xFFFF)
        << " source:" << seed->source << " mergetype:" << (((unsigned int)(seed->type)) >> 30)<< ", save_db:"<< (seed->extraProps["save_db"]));


    try {
      FeedAssistantNReplicaAdapter::instance().dispatch(seed);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedAssistantI::dispatch  --> feed:" << seed->feed << " err, " << e);
    }
    //		} else {
    //			TaskManager::instance().execute(new FeedDispatchTask(seed));
    //		}
    try {
      FeedItemFacadeReplicaAdapter::instance().put(seed);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedAssistantI::dispatch  --> call FeedItemFacade feed:" << seed->feed << " err, " << e);
    }

    try {
      ostringstream os;
      os << "dispatch feed:" << seed->feed << " actor:" << seed->actor
          << " stype:" << (seed->type & 0xFFFF);
      FeedStatAdapter::instance().dispatchRecord(seed->type & 0xFFFF,
          os.str());
    } catch (...) {
      MCE_WARN("FeedAssistantI::dispatch --> invoke FeedStat err");
    }
  }
}

void FeedAssistantI::dispatchEDM(const FeedSeedPtr& seed,
    const Ice::Current& current) {
  if (seed) {
    MCE_INFO("FeedAssistantI::dispatchEDM --> source:" << seed->source << current.con->toString());
    try {
      FeedAssistantNReplicaAdapter::instance().dispatchEDM(seed);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedAssistantI::dispatchEDM --> source:" << seed->source);
    }
  }
}

void FeedAssistantI::dispatchWithReply(const FeedSeedPtr& seed,
    const FeedReplyPtr& reply, const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::dispatchWithReply --> " << current.con->toString());

  if (seed) {
    if (isBlockUser(seed->actor)) {
      MCE_WARN("FeedAssistantI::dispatchWithReply --> user:"<<seed->actor << " is block");
      return;
    }

    try {
      int safe_status = UserPassportAdapter::instance().getUserPassport(
          seed->actor)->safestatus();
      if (safe_status & 0x7c0) {
        MCE_WARN("FeedAssistantI::dispatchWithReply --> block user:"<<seed->actor);
        return;
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedAssistantI::dispatchWithReply --> call UserPassportAdapter err:"<<e);
    }

    seed->feed = -1;
    seed->feed = DbHelper::instance().createId();
    if (seed->feed <= 0) {
      return;
    }

    if(seed->miniMerge == -1){
        seed->miniMerge = seed->feed;
    }
    if(seed->newsMerge == -1){
        seed->newsMerge = seed->feed;
    }

    int stype = (seed->type&0xFFFF);
    Str2StrMap::iterator it = seed->extraProps.find("save_db");
    if(it == seed->extraProps.end()){
      if (_notSavingStype.count(stype)) {
        seed->extraProps["save_db"] = "0";
      } else {
        seed->extraProps["save_db"] = "1";
      }   
    } 

    MCE_INFO("FeedAssistantI::dispatchWithReply  --> feed:" << seed->feed << " actor:" << seed->actor << " stype:" << (seed->type&0xFFFF)
        << " source:" << seed->source << " mergetype:" << (((unsigned int)seed->type) >> 30));
    try {
      FeedAssistantNReplicaAdapter::instance().dispatchWithReply(seed, reply);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedAssistantI::dispatchWithReply  --> feed:" << seed->feed << " err, " << e);
    }
    try {
      ostringstream os;
      os << "dispatchWithReply feed:" << seed->feed << " actor:" << seed->actor
          << " stype:" << (seed->type & 0xFFFF);
      FeedStatAdapter::instance().dispatchRecord(seed->type & 0xFFFF,
          os.str());
      FeedStatAdapter::instance().replyRecord(seed->type & 0xFFFF, os.str());
    } catch (...) {
      MCE_WARN("FeedAssistantI::dispatchWithReply --> invoke FeedStat err");
    }
  }
  //	TaskManager::instance().execute(new FeedDispatchTask(seed, reply));
}

void FeedAssistantI::addFeedReply(Ice::Long source, Ice::Int stype,
    Ice::Int actor, const FeedReplyPtr& reply, const Ice::Current& current) {
  //  MCE_INFO("FeedAssistantI::removeFeedById --> feedId = " << feedId << " " << current.con->toString());

  MCE_INFO("FeedAssistantI::addFeedReply --> add reply " << source << " " << stype << " " << actor<< " " << current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().addFeedReply(source, stype, actor, reply);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::addFeedReply --> add reply " << source << " " << stype << " " << actor<< " err, " << e);
  }
  try {
    ostringstream os;
    os << "reply source:" << source << " actor:" << actor << " stype:" << stype;
    FeedStatAdapter::instance().replyRecord(stype, os.str());
  } catch (...) {
    MCE_WARN("FeedAssistantI::addFeedReply--> invoke FeedStat err");
  }
  //	TaskManager::instance().execute(new ReplyDispatchTask(source, stype, actor,
  //			reply));
}

void FeedAssistantI::addFeedReplyById(int stype, Ice::Long feedid,
    const FeedReplyPtr& reply, const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::addFeedReplyById --> stype = " << stype << ",feedid = " << feedid << " " << current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().addFeedReplyById(stype, feedid, reply);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::addFeedReplyById --> stype = " << stype << ",feedid = " << feedid << " " << " err, " << e);
  }
}

void FeedAssistantI::addFeedReplyWithConfig(Ice::Long source, Ice::Int stype,
    Ice::Int actor, const FeedReplyPtr& reply,
    const map<string, string> & config, const Ice::Current& current) {
  map<string, string>::const_iterator ci = config.find("minigroupid");// TODO (yjw) check the key
  string minigroupid;
  if (ci != config.end()) {
    minigroupid = ci->second;
  } else {
    minigroupid = "UNDEFINED";
  }

  MCE_INFO("FeedAssistantI::addFeedReplyWithConfig --> add reply " << source << " " << stype << " " << actor<< " " << " minigroupid:"<< minigroupid<< current.con->toString());
  FeedAssistantNReplicaAdapter::instance().addFeedReplyWithConfig(source, stype,
      actor, reply, config);
  //try {
  //  ostringstream os;
  //  os << "reply source:" << source << " actor:" << actor << " stype:" << stype << " minigroupid:"<< minigroupid;
  //  FeedStatAdapter::instance().replyRecord(stype, os.str());
  //} catch (...) {
  //  MCE_WARN("FeedAssistantI::addFeedReply--> invoke FeedStat err");
  //}
}

void FeedAssistantI::dispatchAgain(Ice::Long source, int stype, int actor,
    int toid, const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::dispatchAgain --> source = " << source << ",stype = " << stype << ",actor = " << actor << ",toid = " << toid << " " << current.con->toString());
  if (toid <= 0) {
    return;
  }
  try {
    FeedAssistantNReplicaAdapter::instance().dispatchAgain(source, stype, actor, toid);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::dispatchAgain --> source = " << source << ",stype = " << stype << ",actor = " << actor << ",toid = " << toid << " err, " << e);
  }
}

void FeedAssistantI::loadBlockUsers() {
  set<int> ids;
  try {
    DbHelper::instance().loadDangerUserList(ids);
  } catch (...) {
    MCE_WARN("FeedAssistantI::loadBlockUsers --> load block user err");
  }
  if (ids.empty()) {
    return;
  }
  IceUtil::RWRecMutex::WLock lock(_rwMutex);
  _blockUsers.swap(ids);
}
bool FeedAssistantI::isBlockUser(int user) {
  IceUtil::RWRecMutex::RLock lock(_rwMutex);
  if (_blockUsers.count(user)) {
    return true;
  } else {
    return false;
  }
}

Ice::Long FeedAssistantI::createFeedId(const Ice::Current &) {
  return DbHelper::instance().createId();
}

void FeedAssistantI::ReplaceXML(Ice::Long source, int stype, int actor,
    const string & xml, const Ice::Current& current) {
  MCE_INFO("FeedAssistantI::ReplaceXML --> source = " << source << ",stype = " << stype << ",actor = " << actor << ", "<< current.con->toString());
  try {
    FeedAssistantNReplicaAdapter::instance().ReplaceXML(source, stype, actor, xml);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::ReplaceXML --> source = " << source << ",stype = " << stype << ",actor = " << actor << ", err, " << e);
  }
}//TODO


//void FeedAssistantI::addFeedReplyByFeedId(Ice::Long feed,
//		const FeedReplyPtr& reply, const Ice::Current& current) {
//}
//;

void FeedAssistantI::InitNotSavingStype() {

  _notSavingStype.insert(401);
//  _notSavingStype.insert(801);
  _notSavingStype.insert(802);
  _notSavingStype.insert(1001);
  _notSavingStype.insert(1008);
  _notSavingStype.insert(1801);
  _notSavingStype.insert(1802);
  _notSavingStype.insert(1803);
  _notSavingStype.insert(1804);
  _notSavingStype.insert(1805);
  _notSavingStype.insert(1808);
  _notSavingStype.insert(2102);
  _notSavingStype.insert(2103);
  _notSavingStype.insert(2601);
  _notSavingStype.insert(2602);
  _notSavingStype.insert(2701);
  _notSavingStype.insert(3001);
  _notSavingStype.insert(3002);
  _notSavingStype.insert(3003);
  _notSavingStype.insert(4201);
  _notSavingStype.insert(5041);
  _notSavingStype.insert(5301);
  _notSavingStype.insert(5302);
  _notSavingStype.insert(5201);
  //	_notSavingStype.insert(8001);
  //	_notSavingStype.insert(8002);
  //	_notSavingStype.insert(8003);
  //	_notSavingStype.insert(8004);
  //_notSavingStype.insert(8005);
  _notSavingStype.insert(8006);
  _notSavingStype.insert(8007);
  _notSavingStype.insert(8008);
  _notSavingStype.insert(8009);
  _notSavingStype.insert(8010);
  _notSavingStype.insert(8182);
}

void FeedAssistantI::DirectedSend(Ice::Long source, int stype, int actor,
    const vector<int> & targets, const Ice::Current&) {
  try {
    FeedAssistantNReplicaAdapter::instance().DirectedSend(source, stype, actor, targets);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedAssistantI::DirectedSend error. source:" << source << " stype:" << stype << " actor:" << actor << " target size:" << targets.size() << " err, " << e);
  }
}


//---------------------------------------------------------------------------
bool FeedAssistantI::Create(Ice::Int stype, Ice::Int version,
		const MyUtil::Str2StrMap& props, const Ice::Current& current) {

	for(Str2StrMap::const_iterator it = props.begin(); it != props.end(); ++it){
		MCE_DEBUG("key:" << it->first << " -> v:" << it->second);
	}
	return CreateWithReply(stype, version, props, 0, current);
}

bool FeedAssistantI::CreateWithReply(Ice::Int stype, Ice::Int version,
		const MyUtil::Str2StrMap& props, const FeedReplyPtr& reply,
		const Ice::Current& current) {

	FeedTypeConfigPtr feed_config = FeedCreatorI::instance().GetFeedTypeConfig(stype);
	DataConfigPtr data_config = FeedCreatorI::instance().GetDataConfig(stype, version);

	if (!(feed_config && data_config)) {
		MCE_WARN("FeedAssistantI::CreateWithReply --> stype:" << stype << ", version:" << version << ", FeedTypeConfigNotFoundException, LINE:" << __LINE__);
		throw FeedTypeConfigNotFoundException();
	}
	if(!FeedCreatorI::instance().DataCheck(data_config, props)){
		
		MCE_WARN("FeedAssistantI::CreateWithReply --> stype:" << stype << ", version:" << version << ", FeedTypeConfigNotFoundException, LINE:" << __LINE__);
		throw FeedTypeConfigNotFoundException();
	}
	if (data_config->status == 0 || data_config->status == 2) {
		MCE_WARN("FeedAssistantI::CreateWithReply --> stype:" << stype << ", version:" << version << ", config.stat:" << data_config->status << ", NotAllowedToSendException, LINE:" << __LINE__);
		throw NotAllowedToSendException();
	}

	//build feedseed ----------------------------------------------------------

	FeedSeedPtr seed = new FeedSeed();
	seed->feed = 0;
	Str2StrMap::const_iterator it = props.find(data_config->mini_merge_by);
	if((data_config->mini_merge_by).empty() || (it!= props.end() && it->second == "")){
		seed->miniMerge = -1;
	}else{
		seed->miniMerge = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
	}
	it = props.find(data_config->news_merge_by);
	if((data_config->news_merge_by).empty() || (it!= props.end() && it->second == "")){
		seed->newsMerge = -1;
	}else{
		seed->newsMerge = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
	}
	it = props.find(data_config->source_by);
	seed->source = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
	it = props.find(data_config->psource_by);
	seed->psource = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
	seed->ptype = feed_config->ptype;
	it = props.find(data_config->actor_by);
	seed->actor = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
  
  unsigned int t = 0;
	t |= feed_config->news_merge_type;
	t = t << 2;
	t |= feed_config->mini_merge_type;
	t = t << (2 + 2  + 8);
	t |= version;
	t = t << 16;
	t |= stype;

	seed->type = t;
	//static int m = 0;
	seed->time = time(NULL);// + m++*600 - 10000000;
	seed->baseWeight = (feed_config->weight << 16);

	//AppId=110992,base_weight_id=16,expr=f(106074071),publish_mini=0,publish_news=1,save_db=1,send_config=0,update_time=1,
	MCE_INFO("config --> news_type:" << feed_config->news_merge_type << ", mini:" << feed_config->mini_merge_type);
	
  it = props.find(data_config->togroup_by);
  if(it!= props.end()){
    MCE_INFO("FeedAssistantI::CreateWithReply --> to group " << it->second);
    seed->extraProps["togroup"] = boost::lexical_cast<string>(it->second);
  }
  seed->extraProps["base_weight_id"] = boost::lexical_cast<string>(
			feed_config->weight);
	if (feed_config->is_custom_expr) {
		it = props.find("expr");
		seed->extraProps["expr"] = it!= props.end() ? it->second:"";
	} else {
		seed->extraProps["expr"] = FeedCreatorI::instance().CreateExpr(data_config->dispatch_expr, props);
	}
	if(seed->extraProps["expr"] == ""){
		MCE_WARN("FeedAssistantI::CreateWithReply --> expr is null, is_custom_expr:" << feed_config->is_custom_expr);
		return false;
	}
	seed->extraProps["publish_news"] = feed_config->push_flag & 0x01 ? "1"
			: "0";
	seed->extraProps["publish_mini"] = feed_config->push_flag & 0x02 ? "1"
			: "0";
	seed->extraProps["push_flag"] = boost::lexical_cast<string>(
			feed_config->push_flag);
	//send_config 暂时不再使用了
	seed->extraProps["save_db"] = feed_config->is_persist_content ? "1" : "0";
	seed->extraProps["save_feeddb"] = feed_config->is_persist_feeddb ? "1"
			: "0";
	seed->extraProps["update_time"] = feed_config->is_update_time ? "1" : "0";

	seed->xml = FeedCreatorI::instance().Data2XML(stype,version,props);
	//-------------------------------------------------------------------------
	//return true;
	MCE_INFO("feedid:" << seed->feed);
	MCE_INFO("mini_merge:" << seed->miniMerge);
	MCE_INFO("news_merge:" << seed->newsMerge);
	MCE_INFO("source:" << seed->source);
	MCE_INFO("psource:" << seed->psource);
	MCE_INFO("actor:" << seed->actor);
	MCE_INFO("type:" << seed->type);
	MCE_INFO("stype:" << (seed->type & 0xffff));
	MCE_INFO("version:" << ((seed->type >> 16) & 0xff));
	MCE_INFO("news_merge_type:" << ((((unsigned) seed->type) >> 30 ) & 0x3));
	MCE_INFO("mini_merge_type:" << ((((unsigned) seed->type) >> 28 ) & 0x3));
	MCE_INFO("ptype:" << seed->ptype);
	MCE_INFO("time:" << seed->time);
	MCE_INFO("baseWeight:" << seed->baseWeight);
	for(Str2StrMap::iterator it = seed->extraProps.begin(); it != seed->extraProps.end(); ++it){
		MCE_INFO("props." << it->first << ":" << it->second);
	}
	MCE_INFO("xml:" << seed->xml);

	if (data_config->status == 3 && IS_ONLINE_ENV){
		if (reply) {
			dispatchWithReply(seed, reply, current);
		} else {
			dispatch(seed, current);
		}
		return true;

	}else if ((!IS_ONLINE_ENV) && data_config->status > 0){

		// for 正常
		if (reply) {
			dispatchWithReply(seed, reply, current);
		} else {
			dispatch(seed, current);
		}
		return true;
	}
	MCE_WARN("FeedAssistantI::CreateWithReply --> cat not send , ENV:"<<IS_ONLINE_ENV<<" status:"<<data_config->status<<", stype:" << stype << ", version:" << version);

	return false;
}

//FeedSeedPtr FeedCreatorI::BuildFeedSeed(int stype, int version,
//		const MyUtil::Str2StrMap& props) {
//
//}


void FeedAssistantI::ReloadAllConfig(const Ice::Current&){
	TaskManager::instance().execute(new ReloadAllConfigTask());
}
void FeedAssistantI::ReloadConfig(int stype, const Ice::Current&){
	TaskManager::instance().execute(new ReloadConfigTask(stype));
}




