/*
 * Tasks.cpp
 *
 *  Created on: Dec 24, 2009
 *      Author: antonio
 */

#include "Tasks.h"
#include <boost/lexical_cast.hpp> 
#include "DbHelper.h"
#include "FeedAssistantI.h"
#include "FeedContentAdapter.h"
#include "FeedMiniAdapter.h"
#include "FeedItemAdapter.h"
#include "FeedDispatcherAdapter.h"
#include "FeedAssistantNAdapter.h"
#include "FeedInteractionNAdapter.h"
#include "FeedMemcProxy/client/feed_memc_client.h"
#include "UserNetworkAdapter.h"
#include "UserStateAdapter.h"
#include "FeedSchoolAdapter.h"
#include "FeedGroupAdapter.h"
//#include "FeedItemTestAdapter.h"

#include "PhotoDataGatheringAdapter.h"
#include "pugixml.hpp"
#include "Common.h"

using namespace pugi;
using namespace xce::feed;
using namespace mop::hi::svc::adapter;
using namespace xce::adapter::userstate;

void ConfigCache::Add(const Ice::Long feed_id,
    const map<string, string> & config) {
  ItemPtr item = new Item();
  item->feed_id = feed_id;
  item->config = config;

  IceUtil::RWRecMutex::WLock lock(mutex_);
  SIndex & sindex = pool_.get<seqtag> ();
  std::pair<SIndex::iterator, bool> p = sindex.push_front(item);
  if (!p.second) { //失败
    //MCE_INFO("ConfigCache::Add --> feed_id = " << feed_id << ",insert fail,already exist");
  } else { //成功
    //MCE_INFO("ConfigCache::Add --> feed_id = " << feed_id << ",insert success,Kick ...");
    Kick();
  }
}

map<string, string> ConfigCache::Get(const Ice::Long feed_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  FIndex & findex = pool_.get<feedtag> ();
  FIndex::iterator fit = findex.find(feed_id);
  if (fit != findex.end()) {
    //MCE_DEBUG("ConfigCache::Get --> feed_id = " << feed_id << ",get success , " << PrintHelper());
    return (*fit)->config;
  } else {
    //MCE_DEBUG("ConfigCache::Get --> feed_id = " << feed_id << ",miss in cache , "<< PrintHelper());
    return map<string, string> ();
  }

}

string ConfigCache::PrintHelper() {
  SIndex & sindex = pool_.get<seqtag> ();
  SIndex::iterator sit = sindex.begin();
  ostringstream oss;
  oss << "ConfigCache feedids = (";
  int max = 10, count = 0;
  for (; sit != sindex.end(); ++sit) {
    oss << (*sit)->feed_id << ",";
    ++count;
    if (count >= max) {
      oss << " ... ";
      break;
    }
  }
  oss << ")";
  return oss.str();
}

void ConfigCache::Kick() { //不加锁
  //MCE_INFO("ConfigCache::Kick --> before kick,cache size = " << pool_.size());
  while (pool_.size() > (unsigned) kSize_) {
    pool_.pop_back();
  }
  //MCE_INFO("ConfigCache::Kick --> after  kick,cache size = " << pool_.size());
}

size_t ConfigCache::Size() {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  return pool_.size();
}
//////////////////////////////////////////////////////////////////////////////////////////
void RemoveFeedByIdTask::handle() {
  FeedIndexPtr index = DbHelper::instance().getFeedIndex(_feedId);
  if (!index) {
    MCE_INFO("RemoveFeedByIdTask::handle --> get index fail. feedId = " << _feedId);
    return;
  } else {
    MCE_INFO("RemoveFeedByIdTask::handle --> get index sucess. feedId = " << _feedId << ", source = " << index-> source << ",stype = " << index->stype << ",actor = " << index->actor);
  }
  MyUtil::LongSeq ids;
  ids.push_back(_feedId);
  int actor = index->actor;
  DbHelper::instance().removeFeedIndex(ids, actor);
  DbHelper::instance().removeFeedContent(ids, actor);
  DbHelper::instance().removeFeedMini(index->actor, index->stype,
      index->miniMerge);

  IndexCache::instance().removeFeedByIds(ids);
  try {
    //FeedContentAdapter::instance().removeFeed(_feedId);
  } catch (Ice::Exception e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
  }

  try {
    //if(_feedId%10 < 3){
    MCE_INFO("RemoveFeedByIdTask:: --> FeedMemcClient SetEmptyFeed feed:" << _feedId);
    bool res = FeedMemcClient::instance().SetEmptyFeed(_feedId);
    if (!res) {
      MCE_INFO("RemoveFeedByIdTask:: --> FeedMemcClient SetEmptyFeed FAIED!!! line"<<__LINE__);
    }
    //}
  } catch (std::exception& e) {
    MCE_WARN("RemoveFeedByIdTask::handle --> call Memc err, " << e.what());
  } catch (...) {
    MCE_WARN("RemoveFeedByIdTask::handle --> call Memc err, unknown");
  }

  try {
    FeedAssistantNAdapter::instance().cacheSync(index->source, index->stype,
        index->actor);
  } catch (Ice::Exception e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
  }

}

void RemoveFeedBySourceStypeTask::handle() {
  FeedIndexSeq indexSeq = DbHelper::instance().getFeedIndexBySource(_stype,
      _source);
  if (indexSeq.empty()) {
    return;
  }
  MyUtil::LongSeq ids;
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    ids.push_back(indexSeq.at(i)->feed);
  }
  int actor = indexSeq.at(0)->actor;
  DbHelper::instance().removeFeedIndex(ids, actor);
  DbHelper::instance().removeFeedContent(ids, actor);
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    DbHelper::instance().removeFeedMini(indexSeq.at(i)->actor,
        indexSeq.at(i)->stype, indexSeq.at(i)->miniMerge);
  }
  IndexCache::instance().removeFeedByIds(ids);
  try {
    //FeedContentAdapter::instance().removeFeeds(ids);
  } catch (Ice::Exception e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
  }

  try {
    for (size_t i = 0; i < ids.size(); i++) {
      //if(ids.at(i)%10 < 3){
      MCE_INFO("RemoveFeedBySourceStypeTask --> FeedMemcClient SetEmptyFeed feed:" << ids.at(i));
      bool res = FeedMemcClient::instance().SetEmptyFeed(ids.at(i));
      if (!res) {
        MCE_INFO("RemoveFeedBySourceStypeTask --> FeedMemcClient SetEmptyFeed FAIED!!! line"<<__LINE__);
      }
      //}
    }
  } catch (std::exception& e) {
    MCE_WARN("RemoveFeedBySourceStypeTask::handle --> call Memc err, " << e.what());
  } catch (...) {
    MCE_WARN("RemoveFeedBySourceStypeTask::handle --> call Memc err, unknown");
  }

  map<string, FeedIndexPtr> tmp;
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    ostringstream os;
    os << indexSeq.at(i)->source << "," << indexSeq.at(i)->stype << ","
        << indexSeq.at(i)->actor;
    tmp[os.str()] = indexSeq.at(i);
  }
  for (map<string, FeedIndexPtr>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    try {
      FeedAssistantNAdapter::instance().cacheSync(it->second->source,
          it->second->stype, it->second->actor);
    } catch (Ice::Exception & e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
      MCE_WARN("RemoveFeedBySourceStypeTask::handle --> call FeedAssistantNAdapter source:"<<it->second->source << ", stype:"<< it->second->stype << ", actor:"<< it->second->actor <<", err:"<<e);
    }
  }
}

void RemoveFeedBySourceStypeActorTask::handle() {
  FeedIndexSeq indexSeq = DbHelper::instance().getFeedIndex(_stype, _source,
      _actor);
  if (indexSeq.empty()) {
    MCE_INFO("RemoveFeedBySourceStypeActorTask::handle -->  getFeedIndex result empty ,stype = " << _stype << ",source = " << _source << ",actor = " << _actor);
    return;
  } else {
    MCE_INFO("RemoveFeedBySourceStypeActorTask::handle -->  getFeedIndex result sucess, size = " << indexSeq.size()
        << ",stype = " << _stype << ",source = " << _source << ",actor = " << _actor);
  }
  MyUtil::LongSeq ids;
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    ids.push_back(indexSeq.at(i)->feed);
  }
  int actor = indexSeq.at(0)->actor;
  DbHelper::instance().removeFeedIndex(ids, actor);
  DbHelper::instance().removeFeedContent(ids, actor);
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    DbHelper::instance().removeFeedMini(indexSeq.at(i)->actor,
        indexSeq.at(i)->stype, indexSeq.at(i)->miniMerge);
  }
  IndexCache::instance().removeFeedByIds(ids);
  try {
    //FeedContentAdapter::instance().removeFeeds(ids);
  } catch (Ice::Exception & e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
  }
  try {
    for (size_t i = 0; i < ids.size(); i++) {
      //if(ids.at(i)%10 < 3){
      MCE_INFO("RemoveFeedBySourceStypeActorTask --> FeedMemcClient SetEmptyFeed feed:" << ids.at(i));
      bool res = FeedMemcClient::instance().SetEmptyFeed(ids.at(i));
      if (!res) {
        MCE_INFO("RemoveFeedBySourceStypeActorTask::handle --> FeedMemcClient SetEmptyFeed FAIED!!! line"<<__LINE__);
      }
      //}
    }
  } catch (std::exception& e) {
    MCE_WARN("RemoveFeedBySourceStypeActorTask::handle --> call Memc err, " << e.what());
  } catch (...) {
    MCE_WARN("RemoveFeedBySourceStypeActorTask::handle --> call Memc err, unknown");
  }
  try {
    FeedAssistantNAdapter::instance().cacheSync(_source, _stype, _actor);
  } catch (Ice::Exception & e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
  }
}

void RemoveFeedByParentTask::handle() {
  FeedIndexSeq indexSeq = DbHelper::instance().getFeedIndexByPsource(_ptype,
      _psource);
  if (indexSeq.empty()) {
    return;
  }
  MyUtil::LongSeq ids;
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    ids.push_back(indexSeq.at(i)->feed);
  }
  int actor = indexSeq.at(0)->actor;
  DbHelper::instance().removeFeedIndex(ids, actor);
  DbHelper::instance().removeFeedContent(ids, actor);
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    DbHelper::instance().removeFeedMini(indexSeq.at(i)->actor,
        indexSeq.at(i)->stype, indexSeq.at(i)->miniMerge);
  }
  IndexCache::instance().removeFeedByIds(ids);

  try {
    //FeedContentAdapter::instance().removeFeeds(ids);
  } catch (Ice::Exception & e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
  }

  try {
    for (size_t i = 0; i < ids.size(); i++) {
      //if(ids.at(i)%10 < 3){
      MCE_INFO("RemoveFeedByParentTask --> FeedMemcClient SetEmptyFeed feed:" << ids.at(i));
      bool res = FeedMemcClient::instance().SetEmptyFeed(ids.at(i));
      if (!res) {
        MCE_INFO("RemoveFeedByParentTask::handle --> FeedMemcClient SetEmptyFeed FAIED!!! line"<<__LINE__);
      }
      //}
    }
  } catch (std::exception& e) {
    MCE_WARN("RemoveFeedByParent::handle --> call Memc err, " << e.what());
  } catch (...) {
    MCE_WARN("RemoveFeedByParent::handle --> call Memc err, unknown");
  }

  map<string, FeedIndexPtr> tmp;
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    ostringstream os;
    os << indexSeq.at(i)->source << "," << indexSeq.at(i)->stype << ","
        << indexSeq.at(i)->actor;
    tmp[os.str()] = indexSeq.at(i);
  }
  for (map<string, FeedIndexPtr>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    try {
      FeedAssistantNAdapter::instance().cacheSync(it->second->source,
          it->second->stype, it->second->actor);
    } catch (Ice::Exception & e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
      MCE_WARN("RemoveFeedByParentTask::handle --> call FeedAssistantNAdapter source:"<<it->second->source << ", stype:"<< it->second->stype << ", actor:"<< it->second->actor <<", err:"<<e);
    }
  }
}

void ReadMiniTask::handle() {
  if (_feedId > 0) {
    //		FeedIndexPtr index = DbHelper::instance().getFeedIndex(_feedId);
    //		if (!index) {
    //			return;
    //		}
    //		DbHelper::instance().removeFeedMini(_user, index->stype,
    //				index->miniMerge);
    //		FeedMiniAdapter::instance().setRead(_user, index->stype,
    //				index->miniMerge);
    try {
      FeedMiniAdapter::instance().setReadById(_user, _feedId);
    } catch (Ice::Exception & e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    }

  } else {
    //		DbHelper::instance().removeAllFeedMini(_user);
    try {
      FeedMiniAdapter::instance().setReadAll(_user);
    } catch (Ice::Exception & e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    }
  }
}

void ReadNewsTask::handle() {
  if (_feedId > 0) {
    //		FeedIndexPtr index = DbHelper::instance().getFeedIndex(_feedId);
    //		if (!index) {
    //			return;
    //		}
    //		FeedItemAdapter::instance().setRead(_user, index->stype,
    //				index->newsMerge);
    try {
      FeedItemAdapter::instance().setReadById(_user, _feedId);
    } catch (Ice::Exception & e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    }

  } else {
    try {
      FeedItemAdapter::instance().setReadAll(_user);
    } catch (Ice::Exception & e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    }
  }
}

void FeedDispatchTask::CheckSend2School(const FeedConfigPtr& config) {
  if ((_seed->type & 0xffff) / 100 != 1) {
    return;
  }
  //HighSchoolInfoSeq school_seq;
  NetworkBigObjectNPtr info;
  timeval begin, end;
  gettimeofday(&begin, NULL);
  try {
    //school_seq = UserNetworkAdapter::instance().getHighSchoolInfoSeq(_seed->actor);
    info = UserNetworkAdapter::instance().getBigObjectN(_seed->actor);
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("FeedDispatchTask::CheckSend2School --> call UserNetwork err, " << _seed->actor << " " << e);
  }
  gettimeofday(&end, NULL);
  float timeuse = 1000000 * (end.tv_sec - begin.tv_sec) + end.tv_usec
      - begin.tv_usec;
  timeuse /= 1000;
  try {
    for (size_t i = 0; i < info->highschools.size(); i++) {
      mysqlpp::Date d = mysqlpp::Date(time(NULL));
      if (d.year() - info->highschools.at(i)->enrollYear < 3) {
        MCE_INFO("FeedDispatchTask::CheckSend2School --> actor:" << _seed->actor << " highschoolid:" << info->highschools.at(i)->HighSchoolId << " cost:" << timeuse);
        DbHelper::instance().addFeedSchool(
            info->highschools.at(i)->HighSchoolId, _seed);
        FeedItem item;
        MyUtil::Int2IntMap id2Weight;
        item.feed = _seed->feed;
        item.type = _seed->type;
        item.time = _seed->time;
        item.weight = _seed->baseWeight;
        item.merge = _seed->miniMerge;
        item.actor = _seed->actor;
        id2Weight[info->highschools.at(i)->HighSchoolId] = item.weight;
        FeedSchoolAdapter::instance().addFeed(item, config, id2Weight);
      }
    }
    for (size_t i = 0; i < info->universitys.size(); i++) {
      mysqlpp::Date d = mysqlpp::Date(time(NULL));
      if (d.year() - info->universitys.at(i)->enrollYear < 4) {
        MCE_INFO("FeedDispatchTask::CheckSend2School --> actor:" << _seed->actor << " universityid:" << info->universitys.at(i)->universityId << " cost:" << timeuse);
        DbHelper::instance().addFeedSchool(
            info->universitys.at(i)->universityId, _seed);
        FeedItem item;
        MyUtil::Int2IntMap id2Weight;
        item.feed = _seed->feed;
        item.type = _seed->type;
        item.time = _seed->time;
        item.weight = _seed->baseWeight;
        item.merge = _seed->miniMerge;
        item.actor = _seed->actor;
        id2Weight[info->universitys.at(i)->universityId] = item.weight;
        FeedSchoolAdapter::instance().addFeed(item, config, id2Weight);
      }
    }
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("FeedDispatchTask::CheckSend2School --> err, " << e);
  } catch (std::exception& e) {
    MCE_WARN("FeedDispatchTask::CheckSend2School --> write to db err, " << e.what());
  } catch (...) {
    MCE_WARN("FeedDispatchTask::CheckSend2School --> write to db err");
  }
}

void FeedDispatchTask::handle() {//dispatch 和 dispatchWithReply共同调用

  timeval tv_start, tv_end;
  gettimeofday(&tv_start, NULL);
  //MCE_INFO("FeedDispatchTask::handle actor="<<_seed->actor);
  if (!_seed) {
    MCE_DEBUG("FeedDispatchTask::handle --> seed is null");
    return;
  }
  //	_seed->feed = DbHelper::instance().createId();
  MCE_DEBUG("FeedDispatchTask::handle --> feed="<<_seed->feed<<"   time="<<_seed->time);

  //for a bug , 701 and 709 set publish_new and publish_mini to 1
  int stype = _seed->type & 0xffff;
  if (stype == 701 || stype == 709) {
    _seed->extraProps["publish_news"] = "1";
    _seed->extraProps["publish_mini"] = "1";
  }
  //------------------

  FeedConfigPtr config = new FeedConfig();
  config->expr = _seed->extraProps["expr"];
  config->updateTime = _seed->extraProps["update_time"] == "1";
  //	config->sendConfig = _seed->extraProps["send_config"] == "1";
  config->sendConfig = _seed->extraProps.find("send_config")
      != _seed->extraProps.end();
  config->sendNewsConfig = _seed->extraProps["publish_news"] == "1";
  config->sendMiniConfig = _seed->extraProps["publish_mini"] == "1";

  bool saving = _seed->extraProps["save_db"] != "0";

  ///////////yjw增加/////////////////////////////////
  ConfigCache::instance().Add(_seed->feed, _seed->extraProps);
  //////////////////////////////////////////////////


  FeedIndexPtr index = new FeedIndex;
  index->feed = _seed->feed;
  index->newsMerge = _seed->newsMerge;
  index->miniMerge = _seed-> miniMerge;
  index->source = _seed->source;
  index->psource = _seed->psource;
  index->actor = _seed->actor;
  index->time = _seed->time;
  index->stype = _seed->type & 0xffff;
  index->ptype = _seed->ptype;

  //FeedIndexSeq indexSeq = IndexCache::instance().getFeedIndexSeq(index->source,
  //    index->stype, index->actor);
  //if (indexSeq.size() != 0 && saving) {
  IndexCache::instance().add(index);
  //}
  //
  //  if (!(index->stype == 8189 || index->stype == 1702 || (index->stype >= 8001
  //      && index->stype <= 8005))) {
  timeval s1, s2, e1, e2;
  gettimeofday(&s1, NULL);
  try {
    if (saving) {
      if (!DbHelper::instance().addFeedIndex(index)) { //index写失败后，其他都不写
        MCE_WARN("FeedDispatchTask::handle--> addFeedIndex return 0, feed:" << index->feed);
        return;
      }
    }
  } catch (std::exception& e) {
    MCE_WARN("FeedDispatchTask::handle --> addFeedIndex feed:" << index->feed << " err, " << e.what());
    return;
  }
  gettimeofday(&e1, NULL);

  //  To content
  gettimeofday(&s2, NULL);
  try {
    if (saving && (config->sendNewsConfig || config->sendMiniConfig
        || _seed->extraProps.find("togroup") != _seed->extraProps.end())) {
      DbHelper::instance().addFeedContent(_seed);
    }
  } catch (std::exception& e) {
    MCE_WARN("FeedDispatchTask::handle --> addFeedContent feed:" << index->feed << " err, " << e.what());
  }
  gettimeofday(&e2, NULL);

  float t1 = 1000000 * (e1.tv_sec - s1.tv_sec) + e1.tv_usec - s1.tv_usec;
  float t2 = 1000000 * (e2.tv_sec - s2.tv_sec) + e2.tv_usec - s2.tv_usec;
  t1 /= 1000;
  t2 /= 1000;
  MCE_INFO("FeedDispatchTask::handle --> db cost index:" << t1 << " content:" << t2);

  FeedDataPtr data = new FeedData();
  data->feed = _seed->feed;
  data->source = _seed->source;
  data->actor = _seed->actor;
  data->type = _seed->type;
  data->xml = _seed->xml;
  data->time = _seed->time;

  unsigned int baseWeightId = 0;
  if (_seed->extraProps.find("base_weight_id") != _seed->extraProps.end()) {
    try {
      baseWeightId = boost::lexical_cast<int>(
          _seed->extraProps["base_weight_id"]);
      if (!(_seed->actor >= 600000000 && _seed->actor < 700000000)
          && !UserStateAdapter::instance().getUserState(_seed->actor)->isGuideDone()) {
        //the weight of the new user is need to improved by 2
        baseWeightId += 2;
        baseWeightId = baseWeightId > 20 ? 20 : baseWeightId;
        _seed->extraProps["base_weight_id"] = boost::lexical_cast<std::string>(
            baseWeightId);

        MCE_INFO("FeedAssistantN:FeedDispatchTask::handle-->the new user:"<<_seed->actor << " feed:" << _seed->feed << " stype:" << (_seed->type&0xFFFF) <<" base_weight_id improved by 2");
      }
    } catch (std::exception & e) {
      MCE_WARN("FeedDispatchTask::handle --> cast base_weight_id err:"<<e.what() << " feed:" << _seed->feed <<",actor"<< _seed->actor << " stype:" << (_seed->type&0xFFFF));
    }
  }
  data->weight = (baseWeightId << 16) & 0x00ff0000;
  _seed->baseWeight = data->weight;

  //add to FeedContentAdapter
  try {
    //FeedContentAdapter::instance().addFeedContent(data);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedDispatchTask::handle --> call addFeedContent error :"<<e << " feed:" << _seed->feed << ",actor" << _seed->actor << " stype:" << (_seed->type&0xFFFF));
  }

  try {
//    int actor = index->actor;
//    if (IsTester) {//todo del
    FeedContentPtr content = new FeedContent();
    content->data = data;
    bool res = FeedMemcClient::instance().SetFeed(content);
    if (!res) {
      MCE_INFO("RemoveFeedBySourceStypeActorTask::handle --> FeedMemcClient SetEmptyFeed FAIED!!! line"<<__LINE__);
    }
//    }
  } catch (std::exception& e) {
    MCE_WARN("FeedDispatchTask::handle --> call Memc err, " << e.what());
  } catch (...) {
    MCE_WARN("FeedDispatchTask::handle --> call Memc err, unknown");
  }
  //for ad. feed, reload first;
  if (index->stype >= 8001 && index->stype <= 8005) {
    try {
      //FeedItemAdapter::instance().getFeedData(data->actor, 0, 1);
    } catch (Ice::Exception& e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
      MCE_WARN("FeedDispatchTask::handle --> getFeedData  actor:"<<data->actor<<", err:"<<e);
    }
  }

  MCE_INFO("FeedDispatchTask::handle --> fid:" << _seed->feed << ", newsMerge:"<<_seed->newsMerge<<", miniMerge:"<<_seed->miniMerge
      << ", source:"<<_seed->source<<", psource:"<<_seed->psource<<", actor:"<<_seed->actor<<", time:"<<_seed->time
      << ", stype:"<<index->stype<< ", ptype:"<<_seed->ptype << ", seed->type:"<< _seed->type << ", news_merge_type:"
      << int((_seed->type >> 30)& 0x00000003 )<< ", mini_merge_type:"<< int((_seed->type & 0x3FFFFFFF) >> 28) << ", weight:" << _seed->baseWeight
      << ", updateTime:"<<config->updateTime << ", send_config:"<<config->sendConfig<<", news_publish:"
      <<config->sendNewsConfig <<", mini_publish:"<< config->sendMiniConfig << ", expr:"<< config->expr << " saving:" << saving);

  // To Mini DB
  if (config->sendMiniConfig) {
    DbHelper::instance().addFeedMini(_seed);
    CheckSend2School(config);
  }

  if (_seed->extraProps.find("togroup") != _seed->extraProps.end()) {
    int groupid = 0;
    try {
      groupid = boost::lexical_cast<int>(_seed->extraProps["togroup"]);
    } catch (...) {
      MCE_WARN("FeedDispatchTask::handle --> lexical cast err, " << _seed->extraProps["togroup"]);
    }
    try {
      MCE_INFO("FeedDispatchTask::handle --> send to group, actor:" << _seed->actor << " feed:" <<_seed->feed << " group:" << groupid);
      DbHelper::instance().addFeedGroup(groupid, _seed);
    } catch (...) {
      MCE_WARN("FeedDispatchTask::handle --> add to db err");
    }
    FeedItem item;
    MyUtil::Int2IntMap id2Weight;
    item.feed = _seed->feed;
    item.type = _seed->type;
    item.time = _seed->time;
    item.weight = _seed->baseWeight;
    item.merge = _seed->miniMerge;
    item.actor = _seed->actor;
    id2Weight[groupid] = item.weight;
    try {
      FeedGroupAdapter::instance().addFeed(item, config, id2Weight);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedDispatchTask::handle --> send to group, actor:" << _seed->actor << " feed:" <<_seed->feed << " group:" << groupid << " err, " << e);
    }
  }

  if (config->sendNewsConfig || config->sendMiniConfig) {
    try {
      FeedDispatcherAdapter::instance().dispatch(_seed, config);
    } catch (Ice::Exception & e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    }

  } else {
    MCE_WARN("FeedDispatchTask::handle --> not dispatch  feed:"<<_seed->feed << ", news_config:"<<config->sendNewsConfig<<", mini_config:"<<config->sendMiniConfig);
  }

  //延迟发送reply部分，针对DispatchWithReply
  if (_reply) {
    ReplyDataPtr replyData = new ReplyData();
    replyData->source = _seed->source;
    replyData->stype = _seed->type & 0xffff;
    replyData->actor = _seed->actor;
    replyData->replyCount = _reply->count;
    replyData->time = _seed->time;//(int)time(NULL);


    replyData->xml = (!(_reply->newReply.empty()) ? _reply->newReply
        : _reply->oldReply);
    replyData->replyId = (!(_reply->newReply.empty()) ? _reply->newReplyId
        : _reply->oldReplyId);

    try {
      MyUtil::LongSeq feedIds;
      feedIds.push_back(_seed->feed);
      FeedReplyBufferTimer::instance().changeReply(feedIds, _reply);
      //FeedContentAdapter::instance().replaceFeedReply(feedIds, _reply);
    } catch (Ice::Exception& e) {
      MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
      MCE_WARN("ReplyDispatchTask::handle --> call FeedContentAdapter replaceFeedReply  err:"<<e);
    }
    try {
      //if(_seed->feed%10 < 3){
      FeedContentPtr content = FeedMemcClient::instance().GetFeed(_seed->feed);
      if (content) {
        content->reply = _reply;
        bool res = FeedMemcClient::instance().SetFeed(content);
        if (!res) {
          MCE_INFO("FeedDispatchTask::handle() --> FeedMemcClient SetFeed FAIED!!! line: "<<__LINE__);
        }
      } else {
        MCE_INFO("FeedDispatchTask::handle() --> FeedMemcClient GetFeed FAIED!!! Can not add reply to memcache line: "<<__LINE__);
      }
      //}  
    } catch (std::exception& e) {
      MCE_WARN("FeedDispatchTask::handle --> call Memc err, " << e.what());
    } catch (...) {
      MCE_WARN("FeedDispatchTask::handle --> call Memc err, unknown");
    }

    TaskManager::instance().schedule(new ReplyDelayDispatchTimer(replyData,
        config));
  }
  gettimeofday(&tv_end, NULL);
  float timeuse = 1000000 * (tv_end.tv_sec - tv_start.tv_sec) + tv_end.tv_usec
      - tv_start.tv_usec;
  timeuse /= 1000;
  MCE_INFO("FeedDispatchTask::handle --> cost:" << timeuse << " size:" << TaskManager::instance().size(0));

  //Photo issue

  if (stype == 702) {// 圈人
    MCE_INFO("FeedDispatchTask::handle -->event: tag user,  stype: "<<stype<<" type: "<<_seed->type<<" actor: "<< _seed->actor<<" line: "<<__LINE__);
    Ice::Long photoid = _seed->source;
    int tagid = _seed->actor;
    if (photoid > 0) {
      try {
        PhotoDataGatheringAdapter::instance().IncTagCount(photoid, tagid);
      } catch (Ice::Exception& e) {
        MCE_WARN("FeedDispatchTask::handle -->tag user  err:"<<e);
      }
    } else {
      MCE_INFO("FeedDispatchTask::handle -->event: tag user FAILED!! Can not find photoid: "<<stype<<" type: "<<_seed->type<<" actor: "<< _seed->actor<<" line: "<<__LINE__);
    }

  } else if (stype == 708) {//照片第一次被回复
    Ice::Long photoid = _seed->source;
    if (photoid >= 0) {
      MCE_INFO("FeedDispatchTask::handle -->event: photo is replyed first time,  stype: "<<stype<<" type: "<<_seed->type<<" actor: "<< _seed->actor<<" line: "<<__LINE__);
      try {
        PhotoDataGatheringAdapter::instance().IncCommentCount(photoid);
      } catch (Ice::Exception& e) {
        MCE_WARN("FeedDispatchTask::handle -->event photo is replyed first time  err:"<<e);
      }

    } else {
      MCE_INFO("FeedDispatchTask::handle -->FAILED!! event: photo is replyed first time,  stype: "<<stype<<" actor: "<< _seed->actor<<" line: "<<__LINE__);
    }
  }

}

void ReplyDispatchTask::handle() { //FeedAssistantI::addFeedReply调用
  MCE_INFO("ReplyDispatchTask::handle --> " << _actor);
  TimeStat tts(true);
  FeedIndexSeq indexSeq = IndexCache::instance().getFeedIndexSeq(_source,
      _stype, _actor);
  MyUtil::LongSeq feedIds;
  MyUtil::Long2IntMap interactions;
  for (size_t i = 0; i < indexSeq.size(); ++i) {
    feedIds.push_back(indexSeq.at(i)->feed);
    interactions[indexSeq.at(i)->feed] = 1;
  }
  TimeStat ts;
  if (indexSeq.empty()) {

    indexSeq = DbHelper::instance().getFeedIndex(_stype, _source, _actor);
    if (indexSeq.empty()) {
      MCE_INFO("ReplyDispatchTask::handle ---> can not get feed, source:"<<_source<<" actor:"<<_actor << " stype:"<<_stype);
      return;
    }
    for (size_t h = 0; h < indexSeq.size(); ++h) {
      feedIds.push_back(indexSeq.at(h)->feed);
      interactions[indexSeq.at(h)->feed] = 1;
    }
    IndexCache::instance().add(indexSeq);
  }
  MCE_INFO("@@@@@@@@ get index " << _actor << " cost" << ts.getTime());
  ts.reset();
  try {
    FeedReplyBufferTimer::instance().changeReply(feedIds, _reply);
    //FeedContentAdapter::instance().replaceFeedReply(feedIds, _reply);
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("ReplyDispatchTask::handle --> call FeedContentAdapter replaceFeedReply  err:"<<e);
  }
  MCE_INFO("@@@@@@@@ replace feed reply " << _actor << " cost" << ts.getTime());
  ts.reset();

  try {
    for (size_t i = 0; i < feedIds.size(); i++) {
      //if(feedIds.at(i)%10 < 3){
      FeedContentPtr content =
          FeedMemcClient::instance().GetFeed(feedIds.at(i));
      if (content) {
        content->reply = _reply;
        bool res = FeedMemcClient::instance().SetFeed(content);
        if (!res) {
          MCE_INFO("ReplyDispatchTask::handle() --> FeedMemcClient SetFeed FAIED!!! line: "<<__LINE__);
        }
      } else {
        MCE_INFO("ReplyDispatchTask::handle() --> FeedMemcClient GetFeed FAIED!!! Can not add reply to memcache line: "<<__LINE__);
      }
      //}
    }
  } catch (std::exception& e) {
    MCE_WARN("ReplyDispatchTask::handle --> call Memc err, " << e.what());
  } catch (...) {
    MCE_WARN("ReplyDispatchTask::handle --> call Memc err, unknown");
  }
  MCE_INFO("@@@@@@@@ set memcached " << _actor << " cost" << ts.getTime());
  ts.reset();
  try {
    FeedInteractionNAdapter::instance().IncBatch(INTERACTION_REPLY,
        interactions);
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("ReplyDispatchTask::handle ---> call FeedInteraction err, " << e);
  }
  MCE_INFO("@@@@@@@@ up interaction " << _actor << " cost" << ts.getTime());

  if (feedIds.empty()) {
    MCE_WARN("ReplayDispatchTask::handle --> can not find feed id,  source:"<<_source<<" actor:"<<_actor << " stype:"<<_stype);
    return;
  }

  long feedid = feedIds.at(0);

  FeedConfigPtr config = new FeedConfig();

  ts.reset();
  bool from_db = false;
  map<string, string> props = ConfigCache::instance().Get(feedid);
  if (props.empty()) {
    from_db = true;
    //MCE_INFO("ReplyDispatchTask::handle() --> feedid = " << feedid << ", miss in ConfigCache , to find in db ...")
    props = DbHelper::instance().getFeedConfigProp(feedid);
    //ConfigCache::instance().Add(feedid,props);
  } else {
    //MCE_INFO("ReplyDispatchTask::handle() --> feedid = " << feedid << ", hit in ConfigCache , props.size = " << props.size());
  }
  MCE_INFO("@@@@@@@@ get expr " << _actor << " cost" << ts.getTime());

  MCE_INFO("ReplyDispatchTask::handle --->@ get expr, feed:" << feedIds.at(0) << " source:"<<_source<<" actor:"<<_actor << " stype:"<<_stype<< " exprsize:" << props.size() << " from_db:" << from_db);

  map<string, string>::iterator iter;
  iter = props.find("expr");
  if (iter == props.end()) { //没有找到表达式
    //Ice::Long feedid = feedIds.at(0);
    MCE_WARN("FeedAssistantI::addFeedReply  --> actor:"<<_actor<<"\t    prop miss: expr");
    //因为有cache，所以不用下面的定时器
    //    TaskManager::instance().schedule(new ReplyDispatchAgainTimer(_source,
    //        _stype, _actor, _reply, feedid));

    return;
  }
  config->expr = iter->second;

  iter = props.find("publish_mini");
  if (iter == props.end()) {
    //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: publish_mini");
    return;
  }
  config->sendNewsConfig = (iter->second.compare("1") == 0);

  iter = props.find("publish_news");
  if (iter == props.end()) {
    //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: publish_news");
    return;
  }
  config->sendNewsConfig = (iter->second.compare("1") == 0);

  iter = props.find("send_config");
  if (iter == props.end()) {
    //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: send_config");
    return;
  }
  config->sendConfig = (iter->second.compare("1") == 0);

  iter = props.find("update_time");
  if (iter == props.end()) {
    //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: update_time");
    return;
  }
  config->updateTime = (iter->second.compare("1") == 0);

  if (_reply->newReply.empty() && _reply->oldReply.empty()) {
    MCE_INFO("ReplyDispatchTask::handle --> xml empty actor:"
        << _actor << " source:" << _source << " stype:" << _stype
        << " newReply:" << _reply->newReply << " oldReply:" << _reply->oldReply
        << " newReplyId:" << _reply->newReplyId << " oldReplyId:" << _reply->oldReplyId << " count:" << _reply->count);
  }

  ReplyDataPtr replyData = new ReplyData();
  replyData->source = _source;
  replyData->stype = _stype;
  replyData->actor = _actor;
  replyData->replyCount = _reply->count;

  replyData->xml = (!(_reply->newReply.empty()) ? _reply->newReply
      : _reply->oldReply);
  replyData->replyId = (!(_reply->newReply.empty()) ? _reply->newReplyId
      : _reply->oldReplyId);
  replyData->time = (int) time(NULL);

  try {
    /*if(replyData->xml.empty()){
     MCE_INFO("FeedAssistantI::addFeedReply --> xml empty " << replyData->actor << " " << replyData->source << " " << replyData->stype);
     }*/
    MCE_INFO("ReplyDispatchTask::handle--> dispatch " << replyData->actor << " " << replyData->source << " " << replyData->stype);
    FeedDispatcherAdapter::instance().dispatchReply(replyData, config);
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("FeedAssistantI::addFeedReply --> call FeedDispatcherAdapter dispatchReply  err:"<<e);
  }
  //Photo issue
  if (replyData->stype == 701 || replyData->stype == 709) {
    Ice::Long photoid = replyData->source;
    if (photoid >= 0) {
      MCE_INFO("ReplyDispatchTask::handle  --> IncCommentCount photo id: "<<photoid<<" line: "<<__LINE__);
      try {
        PhotoDataGatheringAdapter::instance().IncCommentCount(photoid);
      } catch (Ice::Exception& e) {
        MCE_WARN("ReplyDispatchTask::handle -->event photo is replyed first time  err:"<<e);
      }
    } else {
      MCE_INFO("ReplyDispatchTask::handle  --> can not find photo id:  line: "<<__LINE__);
    }
  }
  MCE_INFO("@@@@@@@@ " << _actor << " total cost:" << tts.getTime() << " size:" << TaskManager::instance().size(REPLY_TASK_LEVEL));
}

//---------------------------------------------------------------------------

void ReplyDispatchWithConfigTask::handle() {
  MCE_INFO("ReplyDispatchWithConfigTask::handle --> "
      "source:" <<_source << " stype:" << _stype <<" actor:" << _actor);
  if (!IndexLocator::LocateFids(_source, _stype, _actor, fids_)
      || fids_.empty()) {
    MCE_WARN("ReplyDispatchWithConfigTask::handle. Locate fids fail");
    return;
  }
  RefleshMiniGroupTime();
  if (1) {//调老接口
    TaskManager::instance().execute(new ReplyDispatchTask(_source, _stype,
        _actor, _reply));
  } else {
    SetInteraction();
    MemcHandler::UpdateReply(fids_, _reply);
    SetReplyBufferTimer();
    SendToFeedDispatcher();
    SendToPhotoMem();
  }
}

int ReplyDispatchWithConfigTask::ParseMiniGroupId() {
  map<string, string>::iterator i = config_.find("minigroupid");
  if (i == config_.end()) {
    MCE_DEBUG("ReplyDispatchWithConfigTask::ParseMiniGroupId. No key:minigroupid" );
    return 0;
  } else {
    try {
      MCE_DEBUG("Key = " << i->second);
      return boost::lexical_cast<long>(i->second);
    } catch (boost::bad_lexical_cast & e) {
      MCE_INFO("ReplyDispatchWithConfigTask::ParseMiniGroupId" << e.what() << ",minigroupid:" << i->second);
      return 0;
    }
  }
}

bool ReplyDispatchWithConfigTask::SetFeedMemc() {
  try {
    for (size_t i = 0; i < fids_.size(); i++) {
      //if(feedIds.at(i)%10 < 3){
      FeedContentPtr content = FeedMemcClient::instance().GetFeed(fids_.at(i));
      if (content) {
        content->reply = _reply;
        bool res = FeedMemcClient::instance().SetFeed(content);
        if (!res) {
          MCE_INFO("ReplyDispatchTask::handle() --> FeedMemcClient SetFeed FAIED!!! line: "<<__LINE__);
          return false;
        } else {
          return true;
        }
      } else {
        MCE_INFO("ReplyDispatchTask::handle() --> FeedMemcClient GetFeed FAIED!!! Can not add reply to memcache line: "<<__LINE__);
        return false;
      }
    }
  } catch (std::exception& e) {
    MCE_WARN("ReplyDispatchTask::handle --> call Memc err, " << e.what());

  } catch (...) {
    MCE_WARN("ReplyDispatchTask::handle --> call Memc err, unknown");
  }
  return false;
}

bool ReplyDispatchWithConfigTask::SetReplyBufferTimer() {
  try {
    FeedReplyBufferTimer::instance().changeReply(fids_, _reply);
    return true;
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("ReplyDispatchTask::handle --> call FeedContentAdapter replaceFeedReply  err:"<<e);
    return false;
  }
}
bool ReplyDispatchWithConfigTask::SendToFeedDispatcher() {
  FeedConfigPtr config = FeedConfigLocator::Locate(fids_.at(0));
  if (!GetReplyData()) {
    return false;
  }
  try {
    FeedDispatcherAdapter::instance().dispatchReply(reply_data_, config);
    return true;
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("FeedAssistantI::addFeedReply --> call FeedDispatcherAdapter dispatchReply  err:"<<e);
    return false;
  }
}
ReplyDataPtr ReplyDispatchWithConfigTask::GetReplyData() {
  reply_data_ = ReplyDataBuilder::Build(_source, _stype, _actor, _reply);
  return reply_data_;
}

bool ReplyDispatchWithConfigTask::SetInteraction() {
  MyUtil::Long2IntMap interactions;
  for (size_t i = 0; i < fids_.size(); ++i) {
    interactions[fids_.at(i)] = 1;
  }
  try {
    FeedInteractionNAdapter::instance().IncBatch(INTERACTION_REPLY,
        interactions);
    return true;
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("ReplyDispatchTask::SetInteraction ---> call FeedInteraction err, " << e);
  }
  return false;
}

bool ReplyDispatchWithConfigTask::SendToPhotoMem() {
  bool suc = false;
  if (!GetReplyData()) {
    return suc;
  }
  if (reply_data_->stype == 701 || reply_data_->stype == 709) {
    Ice::Long photoid = reply_data_->source;
    if (photoid >= 0) {
      MCE_INFO("ReplyDispatchWithConfigTask::SendToPhotoMem  --> IncCommentCount photo id: "<<photoid<<" line: "<<__LINE__);
      try {
        PhotoDataGatheringAdapter::instance().IncCommentCount(photoid);
        suc = true;
      } catch (Ice::Exception& e) {
        MCE_WARN("ReplyDispatchWithConfigTask::SendToPhotoMem -->event photo is replyed first time  err:"<<e);
      }
    } else {
      MCE_INFO("ReplyDispatchWithConfigTask::SendToPhotoMem  --> can not find photo id:  line: "<<__LINE__);
    }
  }
  return suc;
}

bool ReplyDispatchWithConfigTask::RefleshMiniGroupTime() {
  if (config_.empty()) {
    MCE_WARN("ReplyDispatchWithConfigTask::RefleshMiniGroupTime. empty config");
    return false;
  }
  int gid = ParseMiniGroupId();
  if (!gid) {
    MCE_WARN("ReplyDispatchWithConfigTask::RefleshMiniGroupTime. fail Parse minigroupid");
    return false;
  }
  try {
    FeedGroupAdapter::instance().RefreshFeed(gid, fids_.at(0), time(NULL));
  } catch (Ice::Exception& e) {
    MCE_WARN("ReplyDispatchWithConfigTask::RefleshMiniGroupTime. Fail RefreshFeed:"<<e);
  }
  return true;
}

/*void ReplyDispatchAgainTimer::handle() {
 retry_count++;
 FeedConfigPtr config = new FeedConfig();

 map<string, string> props = ConfigCache::instance().Get(_feedId);
 if (props.empty()) {
 props = DbHelper::instance().getFeedConfigProp(_feedId);
 //ConfigCache::instance().Add(_feedId, props);
 }

 MCE_INFO("ReplyDispatchAgainTimer::handle ---> get expr, retry:" << retry_count << " feed:" << _feedId << " source:"<<_source<<" actor:"<<_actor << " stype:"<<_stype
 << " exprsize:" << props.size());

 map<string, string>::iterator iter;
 iter = props.find("expr");
 if (iter == props.end()) {
 //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: expr");
 if (retry_count < 3) {
 if (retry_count == 1) {
 this->delay(2 * 1000);
 } else if (retry_count == 2) {
 this->delay(4 * 1000);
 }
 TaskManager::instance().schedule(this);
 } else {
 MCE_INFO("ReplyDispatchAgainTimer::handle--> not get expr again, feed:"<< _feedId << " source:"<<_source<<" actor:"<<_actor << " stype:"<<_stype);
 }
 return;
 }
 config->expr = iter->second;

 iter = props.find("publish_mini");
 if (iter == props.end()) {
 //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: publish_mini");
 return;
 }
 config->sendNewsConfig = (iter->second.compare("1") == 0);

 iter = props.find("publish_news");
 if (iter == props.end()) {
 //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: publish_news");
 return;
 }
 config->sendNewsConfig = (iter->second.compare("1") == 0);

 iter = props.find("send_config");
 if (iter == props.end()) {
 //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: send_config");
 return;
 }
 config->sendConfig = (iter->second.compare("1") == 0);

 iter = props.find("update_time");
 if (iter == props.end()) {
 //		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: update_time");
 return;
 }
 config->updateTime = (iter->second.compare("1") == 0);

 if (_reply->newReply.empty() && _reply->oldReply.empty()) {
 MCE_INFO("ReplyDispatchAgainTimer::handle --> xml empty actor:"
 << _actor << " source:" << _source << " stype:" << _stype
 << " newReply:" << _reply->newReply << " oldReply:" << _reply->oldReply
 << " newReplyId:" << _reply->newReplyId << " oldReplyId:" << _reply->oldReplyId << " count:" << _reply->count);
 }

 ReplyDataPtr replyData = new ReplyData();
 replyData->source = _source;
 replyData->stype = _stype;
 replyData->actor = _actor;
 replyData->replyCount = _reply->count;

 replyData->xml = (!(_reply->newReply.empty()) ? _reply->newReply
 : _reply->oldReply);
 replyData->replyId = (!(_reply->newReply.empty()) ? _reply->newReplyId
 : _reply->oldReplyId);
 replyData->time = (int) time(NULL);

 try {
 MCE_INFO("ReplyDispatchAgainTimer::handle--> dispatch " << replyData->actor << " " << replyData->source << " " << replyData->stype);
 FeedDispatcherAdapter::instance().dispatchReply(replyData, config);
 } catch (Ice::Exception& e) {
 MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
 MCE_WARN("FeedAssistantI::ReplyDispatchAgainTimer --> call FeedDispatcherAdapter dispatchReply  err:"<<e);
 }
 }*/

void ReplyDelayDispatchTimer::handle() {
  try {
    if (_reply->xml.empty()) {
      MCE_WARN("ReplyDelayDispatchTask::handle --> xml empty" << _reply->actor << " " << _reply->source << " " << _reply->stype);
    }
    FeedDispatcherAdapter::instance().dispatchReply(_reply, _config);
  } catch (Ice::Exception& e) {
    MCE_INFO("ICE_Exception!! err e: "<<e<<"File: "<<__FILE__<<"Line: "<<__LINE__);
    MCE_WARN("ReplyDelayDispatchTimer::handle --> call FeedDispatcherAdapter dispatchReply  err:"<<e);
  }
}

void ReplaceXMLTask::handle() {
  FeedIndexSeq indexSeq = IndexCache::instance().getFeedIndexSeq(_source,
      _stype, _actor);
  MCE_INFO("ReplaceXMLTask::handle --> get from IndexCache, indexSeq size = " << indexSeq.size());
  MyUtil::LongSeq feedIds;
  if (indexSeq.empty()) {
    indexSeq = DbHelper::instance().getFeedIndex(_stype, _source, _actor);
    if (indexSeq.empty()) {
      MCE_INFO("ReplaceXMLTask::handle ---> can not get feed index , source:"<<_source<<" actor:"<<_actor << " stype:"<<_stype);
      return;
    } else {
      IndexCache::instance().add(indexSeq);//放入indexcache
      MCE_INFO("ReplaceXMLTask::handle ---> get feed index from db sucess, source:"<<_source<<" actor:"<<_actor << " stype:"<<_stype << " ,index seq size = " << indexSeq.size());
    }
  }
  for (size_t h = 0; h < indexSeq.size(); ++h) {
    feedIds.push_back(indexSeq.at(h)->feed);
  }
  int actor = indexSeq.at(0)->actor;
  try {
    DbHelper::instance().ReplaceXML(feedIds, _xml, actor);
  } catch (Ice::Exception& e) {
    MCE_WARN("ReplaceXMLTask::handle --> call DbHelper ReplaceXML  err:"<<e);
  }
  try {
    //FeedContentAdapter::instance().ReplaceXML(feedIds, _xml);
    for (size_t i = 0; i < feedIds.size(); i++) {
      //if(feedIds.at(i)%10 < 3){
      FeedContentPtr content =
          FeedMemcClient::instance().GetFeed(feedIds.at(i));
      if (content) {
        content->data->xml = _xml;
        bool res = FeedMemcClient::instance().SetFeed(content);
        if (!res) {
          MCE_INFO("ReplaceXMLTask::handle() --> FeedMemcClient SetFeed FAIED!!! line: "<<__LINE__);
        }
      } else {
        MCE_INFO("ReplaceXMLTask::handle() --> FeedMemcClient GetFeed FAIED!!! Can not add reply to memcache line: "<<__LINE__);
      }
      //}
    }
  } catch (std::exception& e) {
    MCE_WARN("ReplaceXMLTask::handle --> call Memc err, " << e.what());
  } catch (...) {
    MCE_WARN("ReplaceXMLTask::handle --> call Memc err, unknown");
  }
}

void FeedReplyBufferTimer::changeReply(vector<Ice::Long> feeds,
    const FeedReplyPtr& reply) {
  if (!reply) {
    return;
  }
  for (unsigned i = 0; i < feeds.size(); i++) {
    if (feeds.at(i) > 0) {
      IceUtil::Mutex::Lock lock(mutex_);
      replys_[feeds.at(i)] = reply;
    }
  }
}

void FeedReplyBufferTimer::handle() {
  map<Ice::Long, FeedReplyPtr> buffer;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    replys_.swap(buffer);
  }
  MCE_INFO("FeedReplyBufferTimer::handle --> reply size:" << buffer.size());
  if (buffer.empty()) {
    return;
  }
  try {
    DbHelper::instance().UpdateReply(buffer);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedReplyBufferTimer::handle --> call DbHelper err:"<<e);
  }
}
