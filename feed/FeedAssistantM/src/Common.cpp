/*
 * Common.cpp
 *
 *  Created on: Jan 14, 2011
 *      Author: yejingwei
 */
#include "Common.h"
#include <map>
#include <string>
#include <vector>
#include "RFeed.h"
#include "FeedAssistantI.h"
#include "DbHelper.h"
#include "Tasks.h"
#include "FeedMemcProxy/client/feed_memc_client.h"
using namespace std;
using namespace xce::feed;
bool IndexLocator::LocateIndex(long source, int stype, int actor,
    FeedIndexSeq & res) {
  res =
      IndexCache::instance().getFeedIndexSeq(source, stype, actor);
  MyUtil::LongSeq feedIds;
  if (res.empty()) {
    res = DbHelper::instance().getFeedIndex(stype, source, actor);
    if (res.empty()) {
      MCE_INFO("IndexLocator::Locate.fail get feed, source:"<<source<<" actor:"<<actor << " stype:"<<stype);
      return false;
    }
    for (size_t h = 0; h < res.size(); ++h) {
      feedIds.push_back(res.at(h)->feed);
    }
    IndexCache::instance().add(res);
  }
  return true;
}
bool IndexLocator::LocateFids(long source, int stype, int actor,
    vector<long> & fids) {
  FeedIndexSeq indexSeq;
  if (!LocateIndex(source, stype, actor, indexSeq)) {
    return false;
  }
  for (size_t h = 0; h < indexSeq.size(); ++h) {
    fids.push_back(indexSeq.at(h)->feed);
  }
  return true;
}

FeedConfigPtr FeedConfigLocator::Locate(long feedid) {
  FeedConfigPtr config = new FeedConfig();
  bool from_db = false;
  map<string, string> props = ConfigCache::instance().Get(feedid);
  if (props.empty()) {
    from_db = true;
    props = DbHelper::instance().getFeedConfigProp(feedid);
  } else {
  }
  return FeedConfigBuilder::Build(props);
}

FeedConfigPtr FeedConfigBuilder::Build(const map<string,string>& props) {
	FeedConfigPtr config;
  map<string, string>::const_iterator iter = props.find("expr");
  if (iter == props.end()) { //没有找到表达式
    return NULL;
  }
  config->expr = iter->second;
  iter = props.find("publish_mini");
  if (iter == props.end()) {
    return NULL;
  }
  config->sendNewsConfig = (iter->second.compare("1") == 0);
  iter = props.find("publish_news");
  if (iter == props.end()) {
    return NULL;
  }
  config->sendNewsConfig = (iter->second.compare("1") == 0);
  iter = props.find("send_config");
  if (iter == props.end()) {
    return NULL;
  }
  config->sendConfig = (iter->second.compare("1") == 0);
  iter = props.find("update_time");
  if (iter == props.end()) {
    return NULL;
  }
  config->updateTime = (iter->second.compare("1") == 0);
	return config;
}

ReplyDataPtr ReplyDataBuilder::Build(long source, int stype, int actor,
    FeedReplyPtr & reply) {
  ReplyDataPtr replyData = new ReplyData();
  replyData->source = source;
  replyData->stype = stype;
  replyData->actor = actor;
  replyData->replyCount = reply->count;

  replyData->xml = (!(reply->newReply.empty()) ? reply->newReply
      : reply->oldReply);
  replyData->replyId = (!(reply->newReply.empty()) ? reply->newReplyId
      : reply->oldReplyId);
  replyData->time = (int) time(NULL);
  return replyData;
}

bool MemcHandler::UpdateReply(const vector<long>& fids,const FeedReplyPtr & reply){
  unsigned suc_count = 0;
  try {
     for (size_t i = 0; i < fids.size(); i++) {
       //if(feedIds.at(i)%10 < 3){
       FeedContentPtr content = FeedMemcClient::instance().GetFeed(fids.at(i));
       if (content) {
         content->reply = reply;
         bool res = FeedMemcClient::instance().SetFeed(content);
         if (!res) {
           MCE_INFO("MemcHandler::UpdateReply. FeedMemcClient SetFeed FAIED!!! ");
         } else{
           ++suc_count;
         }
       } else {
         MCE_INFO("MemcHandler::UpdateReply. FeedMemcClient GetFeed FAIED!!! Can not add reply to memcache");
       }
     }
   } catch (std::exception& e) {
     MCE_WARN("MemcHandler::UpdateReply " << e.what());
     return false;
   } catch (...) {
     MCE_WARN("MemcHandler::UpdateReply");
     return false;
   }
   return suc_count == fids.size() ? true :false;
}


