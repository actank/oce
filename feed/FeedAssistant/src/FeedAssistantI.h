/*
 * FeedAssistantI.h
 *
 *  Created on: Sep 25, 2009
 *      Author: louis36
 */

#ifndef FEEDASSISTANTI_H_
#define FEEDASSISTANTI_H_

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

#include "QueryRunner.h"
#include "RFeed.h"
#include "Singleton.h"
#include "ObjectCache.h"
#include "FeedCreatorI.h"

namespace xce {
namespace feed {

using namespace MyUtil;
using boost::multi_index_container;
using namespace boost::multi_index;

const std::string FEED_ASSISTANT = "FA";

const int CACHE_SIZE = 1000000;
static bool IS_ONLINE_ENV = true;



class IndexCache: public MyUtil::Singleton<IndexCache> {
public:
	void add(const FeedIndexPtr& index);
	void add(const FeedIndexSeq& indexSeq);//feeds是所有source,stype,actor相同的数据
	void removeFeedByIds(const MyUtil::LongSeq& feeds);

	FeedIndexSeq getFeedIndexSeq(Ice::Long source, Ice::Int stype,
			Ice::Int actor);
	int size();
private:
	void _kick();

	IceUtil::Mutex _mutex;
	struct source_stype_composite_key: composite_key<FeedIndexPtr,
			BOOST_MULTI_INDEX_MEMBER(FeedIndex, Ice::Long, source),
			BOOST_MULTI_INDEX_MEMBER(FeedIndex, Ice::Int, stype),
			BOOST_MULTI_INDEX_MEMBER(FeedIndex, Ice::Int, actor)//,
	> {
	};

	typedef boost::multi_index_container<FeedIndexPtr,
			indexed_by<sequenced<> , hashed_unique<BOOST_MULTI_INDEX_MEMBER(
					FeedIndex, Ice::Long, feed)> , hashed_non_unique<
					source_stype_composite_key> > > IndexContenter;
	IndexContenter _container;

	typedef IndexContenter::nth_index<0>::type SeqIndex;
	typedef IndexContenter::nth_index<1>::type IdIndex;
	typedef IndexContenter::nth_index<2>::type SourceStypeActorIndex;

};

class FeedAssistantI: public virtual FeedCreator,
		public MyUtil::Singleton<FeedAssistantI> {

public:

	FeedAssistantI() {
		InitNotSavingStype();
	}
	virtual void removeFeedById(Ice::Long feedId, const Ice::Current& current =
			Ice::Current());
	virtual void removeFeedBySource(Ice::Long source, Ice::Int stype,
			const Ice::Current& current = Ice::Current());
	virtual void removeFeedByParent(Ice::Long parent, Ice::Int ptype,
			const Ice::Current& current = Ice::Current());
	virtual void removeFeed(Ice::Long source, Ice::Int stype, Ice::Int actor,
			const Ice::Current& current = Ice::Current());

	virtual void removeFeedReply(Ice::Long source, Ice::Int stype,
			Ice::Int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply,
			const Ice::Current& current = Ice::Current());
	virtual void removeFeedReplyById(int stype, Ice::Long feedid,
			Ice::Long removeReplyId, const FeedReplyPtr& reply,
			const Ice::Current& current = Ice::Current());
	virtual void readNews(Ice::Int user, Ice::Long feed,
			const Ice::Current& current = Ice::Current());
	virtual void readAllNews(Ice::Int user, const Ice::Current& current =
			Ice::Current());

	virtual void readMini(Ice::Int user, Ice::Long feed,
			const Ice::Current& current = Ice::Current());
	virtual void readAllMini(Ice::Int user, const Ice::Current& current =
			Ice::Current());

	virtual void dispatch(const FeedSeedPtr& seed, const Ice::Current& current =
			Ice::Current());
	virtual void dispatchEDM(const FeedSeedPtr& seed,
			const Ice::Current& current = Ice::Current());
	virtual void addFeedReplyById(int stype, Ice::Long feedid,
			const FeedReplyPtr& reply, const Ice::Current& current =
					Ice::Current());

	virtual void addFeedReplyWithConfig(Ice::Long source, int stype,
			Ice::Int actor, const FeedReplyPtr& reply,
			const map<string, string> & config, const Ice::Current& current =
					Ice::Current());

	virtual void dispatchWithReply(const FeedSeedPtr& seed,
			const FeedReplyPtr& reply, const Ice::Current& current =
					Ice::Current());
	virtual void addFeedReply(Ice::Long source, Ice::Int stype, Ice::Int actor,
			const FeedReplyPtr& reply, const Ice::Current& current =
					Ice::Current());
	//virtual void addFeedReplyByFeedId(Ice::Long feed, const FeedReplyPtr& reply, const Ice::Current& = Ice::Current()){};

	virtual void cacheSync(Ice::Long source, int stype, int actor,
			const Ice::Current & current = Ice::Current()) {
	}
	;

	virtual Ice::Long getFeedId(Ice::Long source, int stype, int actor,
			const Ice::Current& current = Ice::Current()) {
		return 0;
	}
	;

	virtual Ice::Long createFeedId(const Ice::Current & current =
			Ice::Current());

	virtual void dispatchAgain(Ice::Long source, int stype, int actor,
			int toid, const Ice::Current& current = Ice::Current());

	virtual void ReplaceXML(Ice::Long source, int stype, int actor,
			const string & xml, const Ice::Current& = Ice::Current());//TODO

	// FeedCreator interface
	//-----------------------------------------------------------------------

	virtual bool Create(Ice::Int stype, Ice::Int version,
			const MyUtil::Str2StrMap& props, const Ice::Current& =
					Ice::Current());
	virtual bool CreateWithReply(Ice::Int stype, Ice::Int version,
			const MyUtil::Str2StrMap& props, const FeedReplyPtr& reply,
			const Ice::Current& = Ice::Current());

	virtual void ReloadAllConfig(const Ice::Current& = Ice::Current());
	virtual void ReloadConfig(Ice::Int stype, const Ice::Current& =
			Ice::Current());

	//-----------------------------------------------------------------------
	void dispatchImpl(const FeedSeedPtr& seed, const FeedReplyPtr& reply);
	void addFeedReplyImpl(Ice::Long source, Ice::Int stype, Ice::Int actor,
			const FeedReplyPtr& reply);
	void readNewsImpl(Ice::Int user, Ice::Long feed);

	void loadBlockUsers();
	bool isBlockUser(int user);
	virtual void DirectedSend(Ice::Long source, int stype, int actor,
			const vector<int> & targets, const Ice::Current& = Ice::Current());

private:
	//查找到所有符合条件的新鲜事（source,stype,actor），更新feedcontent，返回id序列中的第一个id，如果没有符合条件的feed，返回-1
	Ice::Long _replaceFeedReply(Ice::Long source, Ice::Int stype,
			Ice::Int actor, const FeedReplyPtr& reply);

	void InitNotSavingStype();

	IceUtil::RWRecMutex _rwMutex;
	set<int> _blockUsers;
	set<int> _notSavingStype;
};

////用于在调用 dispatchWithReply 时将reply部分延迟发送给 dispatcher
//class ReplyDelayDispatchTask: public Timer {
//public:
//	ReplyDelayDispatchTask(const ReplyDataPtr& replyData, const FeedConfigPtr& config):
//		Timer(5*1000),_replyData(replyData),_config(config){
//
//	}
//	virtual void handle();
//private:
//	ReplyDataPtr _replyData;
//	FeedConfigPtr _config;
//};

class StatTimer: public Timer {
public:
	StatTimer() :
		Timer(5000) {
	}
	;
	virtual void handle() {
		MCE_INFO("STAT: index_cache_size:"<<IndexCache::instance().size() << " thread_size:"<< TaskManager::instance().size(0));
	}
};

}
;
}
;

#endif /* FEEDASSISTANTI_H_ */
