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
#include "pugixml.hpp"
#include <boost/lexical_cast.hpp>
#include "XceLoggerAdapter.h"

using namespace com::xiaonei::service;
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



bool ReplyLogger::AddReply(Ice::Long fid,Ice::Long source, Ice::Int stype,
    Ice::Int actor, const FeedReplyPtr& reply){
  string replyer;
  try{
    if(!reply || !reply->newReplyId || reply->newReply.empty()){
      return false;
    }
    string re_str = reply->newReply;
    pugi::xml_document doc;
    if(!doc.load(re_str.c_str())){
      MCE_WARN("ReplyLogger::AddReply. load error. source: " << source << " stype: " << stype << " actor: " << actor);
      return false;
    }
    pugi::xml_node id_node = doc.child("f").child("reply").child("from").child("id");
    replyer = id_node.first_child().value();
    MCE_INFO("ReplyLogger::AddLog. #reply# source: " << source << " stype: " << stype << " actor: " << actor << " replyer: " << replyer);
    ostringstream os;
  	time_t seconds;
  	seconds = time (NULL);
    os << "r " << seconds << " "<<  replyer << " " << actor << " " << stype << " " << source << " " << fid;
		if(!logger_){
			MCE_WARN("ReplyLogger::AddReply. logger is null @@@@@@@@@@@@@@@@@@@@@@@@");
			return true;
		}
    logger_->addLog(path_, os.str());
    return true;
  }catch (Ice::Exception& e) {
    MCE_WARN("ReplyLogger::AddLog. error. source: " << source << " stype: " << stype << " actor: " << actor << " replyuserid: " << replyer << " error: " <<e);
    return false;
  }catch(...){
    MCE_WARN("ReplyLogger::AddLog. unkown error. source: " << source << " stype: " << stype << " actor: " << actor << " replyuserid: " << replyer << " error: " );
		return false;
	}
}

void ReplyLogger::Init(){
  logger_ = XceLoggerAdapter::instance().getLogger(1, 32768);
	if(!logger_){
		MCE_WARN("ReplyLogger::Init. logger is null @@@@@@@@@@@@@@@@@@@@@@@@");
		return;
	}
  logger_->initialize();
  path_.push_back("feed");
  path_.push_back("reply");
	MCE_INFO("ReplyLogger::Init. done");
}


int ReplyXmlParser::ParseReplier(const string & xml){
	try{
		pugi::xml_document doc;
		if(!doc.load(xml.c_str())){
			MCE_WARN("ReplyXmlParser::ParseReplier.");
			return 0;
		}
		pugi::xml_node id_node = doc.child("f").child("reply").child("from").child("id");
		int replier = boost::lexical_cast<int>(id_node.first_child().value());
		return replier;
	}catch (Ice::Exception& e) {
		MCE_WARN("ReplyXmlParser::ParseReplier. error. source: " << " error: " <<e);
	}catch (std::exception& e) {
		MCE_WARN("ReplyXmlParser::ParseReplier  err, " << e.what());
	}catch(...){
		MCE_WARN("ReplyXmlParser::ParseReplier  err ...");
	}
	return 0;
}
