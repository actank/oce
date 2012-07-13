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



namespace xce {
namespace feed {


using namespace MyUtil;
using boost::multi_index_container;
using namespace boost::multi_index;

const std::string FEED_ASSISTANT = "FA";



const int CACHE_SIZE = 20000000;

class IndexCache:public MyUtil::Singleton<IndexCache>
{
public:
	void add(const FeedIndexPtr& index);
	void add(const FeedIndexSeq& indexSeq);//feeds是所有source,stype,actor相同的数据
	void removeFeedByIds(const MyUtil::LongSeq& feeds);
	void removeFeedBySourceStypeActor(Ice::Long source, int stype, int actor);

	FeedIndexSeq getFeedIndexSeq(Ice::Long source, Ice::Int stype, Ice::Int actor);
	FeedIndexPtr getFeedIndex(Ice::Long feedid);
	int size();
private:
	void _kick();

	IceUtil::Mutex _mutex;
	struct source_stype_composite_key: composite_key<FeedIndexPtr,
   		    BOOST_MULTI_INDEX_MEMBER(FeedIndex,Ice::Long,source),
			BOOST_MULTI_INDEX_MEMBER(FeedIndex,Ice::Int,stype),
			BOOST_MULTI_INDEX_MEMBER(FeedIndex,Ice::Int,actor)//,
	> {
	};

	typedef boost::multi_index_container<
			FeedIndexPtr,
			indexed_by<
				sequenced<> ,
				hashed_unique<
					BOOST_MULTI_INDEX_MEMBER(FeedIndex, Ice::Long, feed)> ,
				ordered_non_unique<source_stype_composite_key> > > IndexContenter;
	IndexContenter _container;

	typedef IndexContenter::nth_index<0>::type SeqIndex;
	typedef IndexContenter::nth_index<1>::type IdIndex;
	typedef IndexContenter::nth_index<2>::type SourceStypeActorIndex;

};




class FeedAssistantI: public FeedAssistant, public MyUtil::Singleton<
		FeedAssistantI> {

public:

	virtual void removeFeedById(Ice::Long feedId, const Ice::Current& =
			Ice::Current());
	virtual void removeFeedBySource(Ice::Long source, Ice::Int stype,
			const Ice::Current& = Ice::Current());
	virtual void removeFeedByParent(Ice::Long parent, Ice::Int ptype,
			const Ice::Current& = Ice::Current());
	virtual void removeFeed(Ice::Long source, Ice::Int stype, Ice::Int actor,
			const Ice::Current& = Ice::Current());

	virtual void removeFeedReply(Ice::Long source, Ice::Int stype, Ice::Int actor,
								 Ice::Long removeReplyId, const FeedReplyPtr& reply,
								 const Ice::Current& = Ice::Current());
	virtual void removeFeedReplyById(int stype, Ice::Long feedid, Ice::Long removeReplyId, const FeedReplyPtr& reply, const Ice::Current& = Ice::Current());
	virtual void readNews(Ice::Int user, Ice::Long feed, const Ice::Current& =
			Ice::Current());
	virtual void readAllNews(Ice::Int user, const Ice::Current& =
			Ice::Current());

	virtual void readMini(Ice::Int user, Ice::Long feed, const Ice::Current& = Ice::Current());
	virtual	void readAllMini(Ice::Int user, const Ice::Current& = Ice::Current());

	virtual void dispatch(const FeedSeedPtr& seed, const Ice::Current& =
			Ice::Current());
	virtual void addFeedReplyById(int stype, Ice::Long feedid, const FeedReplyPtr& reply, const Ice::Current& = Ice::Current());


	virtual void dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply, const Ice::Current& = Ice::Current());
  virtual void addFeedReplyWithConfig(Ice::Long source, Ice::Int stype,
      Ice::Int actor, const FeedReplyPtr& reply, const map<string,string> & reply_config, const Ice::Current& current = Ice::Current());
	virtual void addFeedReply(Ice::Long source, Ice::Int stype, Ice::Int actor,
							  const FeedReplyPtr& reply,
							  const Ice::Current& = Ice::Current());
	virtual void addFeedReplyByFeedId(Ice::Long feed, const FeedReplyPtr& reply, const Ice::Current& = Ice::Current()){};
	virtual void cacheSync(Ice::Long source, int stype, int actor, const Ice::Current & = Ice::Current());
	
	virtual void dispatchEDM(const FeedSeedPtr& seed, const Ice::Current& = Ice::Current());

	virtual Ice::Long getFeedId(Ice::Long source, int stype, int actor, const Ice::Current& = Ice::Current());

	virtual Ice::Long createFeedId(const Ice::Current& = Ice::Current()){
		return 0;
	}	

	virtual void dispatchAgain(Ice::Long source, int stype, int actor, int toid, const Ice::Current& = Ice::Current());
	virtual void dispatchNewsAgain(Ice::Long feedid, const string& expr, const Ice::Current& = Ice::Current());

	virtual void ReplaceXML(Ice::Long source, int stype, int actor,const string & xml, const Ice::Current& = Ice::Current());


	void dispatchImpl(const FeedSeedPtr& seed, const FeedReplyPtr& reply);
	void addFeedReplyImpl(Ice::Long source, Ice::Int stype, Ice::Int actor, const FeedReplyPtr& reply);
	void readNewsImpl(Ice::Int user, Ice::Long feed);
	
	virtual void SetAsync(int async,const Ice::Current& = Ice::Current());


	//Ice::Long getFeedId(Ice::Long source, const Ice::Current & = Ice::Current());
private:
	//查找到所有符合条件的新鲜事（source,stype,actor），更新feedcontent，返回id序列中的第一个id，如果没有符合条件的feed，返回-1
	Ice::Long _replaceFeedReply(Ice::Long source, Ice::Int stype, Ice::Int actor,
						 const FeedReplyPtr& reply);
private:

	IceUtil::Mutex _mutex;
	static const int EDM_MUTEX_COUNT = 100;
	IceUtil::Mutex _EDMMutexs[EDM_MUTEX_COUNT];
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

class StatTimer: public Timer{
public:
	StatTimer():Timer(5000){};
	virtual void handle();
};

	
}
;
}
;

#endif /* FEEDASSISTANTI_H_ */
