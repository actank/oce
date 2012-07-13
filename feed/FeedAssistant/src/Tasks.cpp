/*
 * Tasks.cpp
 *
 *  Created on: Dec 24, 2009
 *      Author: antonio
 */

#include "Tasks.h"
#include "DbHelper.h"
#include "FeedAssistantI.h"
#include "FeedContentAdapter.h"
#include "FeedMiniAdapter.h"
//#include "FeedItemAdapter.h"
#include "FeedDispatcherAdapter.h"
#include "FeedNewsAdapter.h"
#include "FeedCacheLogicAdapter.h"

using namespace xce::feed;

void RemoveFeedByIdTask::handle() {
	FeedIndexPtr index = DbHelper::instance().getFeedIndex(_feedId);
	if (!index) {
		return;
	}
	MyUtil::LongSeq ids;
	ids.push_back(_feedId);
	DbHelper::instance().removeFeedIndex(ids);
	DbHelper::instance().removeFeedContent(ids);
	DbHelper::instance().removeFeedMini(index->actor, index->stype,
			index->miniMerge);

	IndexCache::instance().removeFeedByIds(ids);
	FeedContentAdapter::instance().removeFeed(_feedId);

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
	DbHelper::instance().removeFeedIndex(ids);
	DbHelper::instance().removeFeedContent(ids);
	for (size_t i = 0; i < indexSeq.size(); ++i) {
		DbHelper::instance().removeFeedMini(indexSeq.at(i)->actor, indexSeq.at(
				i)->stype, indexSeq.at(i)->miniMerge);
	}
	IndexCache::instance().removeFeedByIds(ids);
	FeedContentAdapter::instance().removeFeeds(ids);

}

void RemoveFeedBySourceStypeActorTask::handle() {
	FeedIndexSeq indexSeq = DbHelper::instance().getFeedIndex(_stype, _source,
			_actor);
	if (indexSeq.empty()) {
		return;
	}
	MyUtil::LongSeq ids;
	for (size_t i = 0; i < indexSeq.size(); ++i) {
		ids.push_back(indexSeq.at(i)->feed);
	}
	DbHelper::instance().removeFeedIndex(ids);
	DbHelper::instance().removeFeedContent(ids);
	for (size_t i = 0; i < indexSeq.size(); ++i) {
		DbHelper::instance().removeFeedMini(indexSeq.at(i)->actor, indexSeq.at(
				i)->stype, indexSeq.at(i)->miniMerge);
	}
	IndexCache::instance().removeFeedByIds(ids);
	FeedContentAdapter::instance().removeFeeds(ids);

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
	DbHelper::instance().removeFeedIndex(ids);
	DbHelper::instance().removeFeedContent(ids);
	for (size_t i = 0; i < indexSeq.size(); ++i) {
		DbHelper::instance().removeFeedMini(indexSeq.at(i)->actor, indexSeq.at(
				i)->stype, indexSeq.at(i)->miniMerge);
	}
	IndexCache::instance().removeFeedByIds(ids);
	FeedContentAdapter::instance().removeFeeds(ids);

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
		FeedMiniAdapter::instance().setReadById(_user, _feedId);
	} else {
		//		DbHelper::instance().removeAllFeedMini(_user);
		FeedMiniAdapter::instance().setReadAll(_user);
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
		//FeedItemAdapter::instance().setReadById(_user, _feedId);
	  try{
      FeedNewsAdapter::instance().setReadById(_user, _feedId);
    }catch(Ice::Exception& e){
      MCE_WARN("ReadNewsTask::handle --> call FeedItemTest::setReadById err, " << e);
    }
  
    try{
      FeedCacheLogicAdapter::instance().remove(_user, _feedId, true);
    }catch(Ice::Exception& e){
      MCE_WARN("ReadNewsTask::handle --> call FeedCacheLogic::remove err, " << e);
    }

  } else {
		//FeedItemAdapter::instance().setReadAll(_user);
	  try{
      FeedNewsAdapter::instance().setReadAll(_user);
    }catch(Ice::Exception& e){
      MCE_WARN("ReadNewsTask::handle --> call FeedItemTest::setReadAll err, " << e);
    }
    try{
      FeedCacheLogicAdapter::instance().removeAll(_user, true);
    }catch(Ice::Exception& e){
      MCE_WARN("ReadNewsTask::handle --> call FeedCacheLogic::removeAll err, " << e);
    }

  }
}

void FeedDispatchTask::handle() {

	MCE_INFO("FeedDispatchTask::handle actor="<<_seed->actor);
	if (!_seed) {
		MCE_DEBUG("FeedDispatchTask::handle --> seed is null");
		return;
	}
	_seed->feed = DbHelper::instance().createId();
	MCE_DEBUG("FeedDispatchTask::handle --> feed="<<_seed->feed<<"   time="<<_seed->time);

	//for a bug , 701 and 709 set publish_new and publish_mini to 1
	int stype = _seed->type & 0xffff;
	if (stype == 701 || stype == 709 ) {
		_seed->extraProps["publish_news"] = "1";
		_seed->extraProps["publish_mini"] = "1";
	}
	//------------------

	FeedConfigPtr config = new FeedConfig();
	config->expr = _seed->extraProps["expr"];
	config->updateTime = _seed->extraProps["update_time"] == "1";
	config->sendConfig = _seed->extraProps["send_config"] == "1";
	config->sendNewsConfig = _seed->extraProps["publish_news"] == "1";
	config->sendMiniConfig = _seed->extraProps["publish_mini"] == "1";

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

	FeedIndexSeq indexSeq = IndexCache::instance().getFeedIndexSeq(
			index->source, index->stype, index->actor);
	if (indexSeq.size() != 0) {
		IndexCache::instance().add(index);
	}

	if (!(index->stype == 8189 || index->stype == 1702 || (index->stype >= 8001
			&& index->stype <= 8005))) {
		DbHelper::instance().addFeedIndex(index);
	}

	//  To content
	if (config->sendNewsConfig || config->sendMiniConfig) {
		DbHelper::instance().addFeedContent(_seed);
	}

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
		} catch (std::exception & e) {
			MCE_WARN("FeedDispatchTask::handle --> cast base_weight_id err:"<<e.what());
		}
	}
	data->weight = (baseWeightId << 16) & 0x00ff0000;
	_seed->baseWeight = data->weight;

	//add to FeedContentAdapter
	try {
		FeedContentAdapter::instance().addFeedContent(data);
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedDispatchTask::handle --> addFeedContent  feedId:"<<data->feed<<", err:"<<e);
	}
	//for ad. feed, reload first;
	if (index->stype >= 8001 && index->stype <= 8005) {
		try {
			//FeedItemAdapter::instance().getFeedData(data->actor, 0, 1);
		} catch (Ice::Exception& e) {
			MCE_WARN("FeedDispatchTask::handle --> getFeedData  actor:"<<data->actor<<", err:"<<e);
		}
	}

	MCE_INFO("FeedDispatchTask::handle --> fid:" << _seed->feed << ", newsMerge:"<<_seed->newsMerge<<", miniMerge:"<<_seed->miniMerge
			<< ", source:"<<_seed->source<<", psource:"<<_seed->psource<<", actor:"<<_seed->actor<<", time:"<<_seed->time
			<< ", stype:"<<index->stype<< ", ptype:"<<_seed->ptype << ", seed->type:"<< _seed->type << ", news_merge_type:"
			<< int((_seed->type >> 30)& 0x00000003 )<< ", mini_merge_type:"<< int((_seed->type & 0x3FFFFFFF) >> 28) << ", weight:" << _seed->baseWeight
			<< ", updateTime:"<<config->updateTime << ", send_config:"<<config->sendConfig<<", news_publish:"
			<<config->sendNewsConfig <<", mini_publish:"<< config->sendMiniConfig << ", expr:"<< config->expr);

	// To Mini DB
	if (config->sendMiniConfig) {
		DbHelper::instance().addFeedMini(_seed);
	}
	if (config->sendNewsConfig || config->sendMiniConfig) {
		FeedDispatcherAdapter::instance().dispatch(_seed, config);
	} else {
		MCE_WARN("FeedDispatchTask::handle --> not dispatch  feed:"<<_seed->feed << ", news_config:"<<config->sendNewsConfig<<", mini_config:"<<config->sendMiniConfig);
	}

	//延迟发送reply部分
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
			FeedContentAdapter::instance().replaceFeedReply(feedIds, _reply);
		} catch (Ice::Exception& e) {
			MCE_WARN("ReplyDispatchTask::handle --> call FeedContentAdapter replaceFeedReply  err:"<<e);
		}

		TaskManager::instance().schedule(new ReplyDelayDispatchTimer(replyData,
				config));
	}

}

void ReplyDispatchTask::handle() {
	FeedIndexSeq indexSeq = IndexCache::instance().getFeedIndexSeq(_source,
			_stype, _actor);
	MyUtil::LongSeq feedIds;
	for (size_t i = 0; i < indexSeq.size(); ++i) {
		feedIds.push_back(indexSeq.at(i)->feed);
	}
	if (indexSeq.empty()) {

		indexSeq = DbHelper::instance().getFeedIndex(_stype, _source, _actor);
		if (indexSeq.empty()) {
			MCE_INFO("ReplyDispatchTask::handle ---> can not get feed, source:"<<_source<<" actor:"<<_actor << " stype:"<<_stype);
			return;
		}
		for (size_t h = 0; h < indexSeq.size(); ++h) {
			feedIds.push_back(indexSeq.at(h)->feed);
		}
		IndexCache::instance().add(indexSeq);
	}
	try {
		FeedContentAdapter::instance().replaceFeedReply(feedIds, _reply);
	} catch (Ice::Exception& e) {
		MCE_WARN("ReplyDispatchTask::handle --> call FeedContentAdapter replaceFeedReply  err:"<<e);
	}

	FeedConfigPtr config = new FeedConfig();
	map<string, string> props;
	props = DbHelper::instance().getFeedConfigProp(feedIds.at(0));

	map<string, string>::iterator iter;
	iter = props.find("expr");
	if (iter == props.end()) {
		//		MCE_DEBUG("FeedAssistantI::addFeedReply  --> actor:"<<actor<<"\t    prop miss: expr");
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
		FeedDispatcherAdapter::instance().dispatchReply(replyData, config);
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedAssistantI::addFeedReply --> call FeedDispatcherAdapter dispatchReply  err:"<<e);
	}
}

void ReplyDelayDispatchTimer::handle() {
	try {
		if (_reply->xml.empty()) {
			MCE_WARN("ReplyDelayDispatchTask::handle --> xml empty" << _reply->actor << " " << _reply->source << " " << _reply->stype);
		}
		FeedDispatcherAdapter::instance().dispatchReply(_reply, _config);
	} catch (Ice::Exception& e) {
		MCE_WARN("ReplyDelayDispatchTimer::handle --> call FeedDispatcherAdapter dispatchReply  err:"<<e);
	}
}

void LoadBlockUserTimer::handle(){
	FeedAssistantI::instance().loadBlockUsers();
}
