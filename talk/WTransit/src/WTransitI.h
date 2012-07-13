#ifndef _WTRANSITI_H_
#define _WTRANSITI_H_

#include "TransitAider.h"
#include "FeedPassage.h"
//#include "ReplyPassage.h"
#include "NoticePassage.h"
#include "FeedReplyPassage.h"

#include "WTransit.h"
#include "Singleton.h"
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>

#include <IceUtil/Shared.h>
#include "TalkProxy.h"
#include "RFeed.h"
#include "ServiceI.h"
#include "TalkCommon.h"
#include "TalkUtil/src/pugixml.hpp"
#include "Pipe.h"


namespace talk {
namespace wtransit {

using namespace MyUtil;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace xce::feed;
using namespace xce::notify;
using namespace pugi;


class PipeFactoryI : public PipeFactory {
public:
	virtual PipePtr create(const string& type, int index) {
		//if (type == "Notify") {
		//	return new NotifyPipe(type,index);
		//} else 
		if (type == "Feed") {
			//return new FeedPipe(type, index);
		} 
		//else if (type == "Reply"){
		//	return new ReplyPipe(type,index);
		//} 
		else if (type == "FeedReply"){
			//return new FeedReplyPipe(type,index);
		} else {
			//return new NoticePipe(type,index);
		}

	}
	;
};

class LoadFeedTask : public MyUtil::Task {
	JidPtr _jid;
	Ice::Long _feedIndex;
	Ice::Long _replyIndex;
	int _iqId;
public:
	LoadFeedTask(const JidPtr& jid, Ice::Long feedIndex , Ice::Long replyIndex, int iqId) :
		Task(2011), _jid(jid), _feedIndex(feedIndex), _replyIndex(replyIndex), _iqId(iqId) {
	}
	;
	virtual void handle();
};



class CallFeedCacheAndContentTask: public MyUtil::Task {
	JidPtr _jid;
	Ice::Long _feedIndex;
	Ice::Long _replyIndex;
	int _iqId;
	
public:
	CallFeedCacheAndContentTask(const JidPtr& jid, Ice::Long feedIndex , Ice::Long replyIndex, int iqId) :
		Task(2011), _jid(jid), _feedIndex(feedIndex), _replyIndex(replyIndex), _iqId(iqId) {
	}
	;
	virtual void handle();
};



class CallFeedCacheTask: public MyUtil::Task {
	JidPtr _jid;
	Ice::Long _feedIndex;
	Ice::Long _replyIndex;
	int _iqId;
	
public:
	CallFeedCacheTask(const JidPtr& jid, Ice::Long feedIndex , Ice::Long replyIndex, int iqId) :
		Task(2011), _jid(jid), _feedIndex(feedIndex), _replyIndex(replyIndex), _iqId(iqId) {
	}
	;
	virtual void handle();
};


class CallNotifyGateTask: public MyUtil::Task {
	JidPtr _jid;
	Ice::Long _feedIndex;
	Ice::Long _replyIndex;
	int _iqId;
public:
	CallNotifyGateTask(const JidPtr& jid, Ice::Long feedIndex , Ice::Long replyIndex, int iqId) :
		Task(2011), _jid(jid), _feedIndex(feedIndex), _replyIndex(replyIndex), _iqId(iqId) {
	}
	;
	virtual void handle();
};


class loadOfflineFeedReplyTask : public MyUtil::Task{
public:
	loadOfflineFeedReplyTask(const JidPtr& jid, int stype, int actor, Ice::Long source, const string& iqId):
	_jid(jid), _stype(stype), _actor(actor), _source(source), _iqId(iqId){}
	void handle();
private:
	JidPtr 	_jid;
	int 	_stype;
	int 	_actor;
	Ice::Long _source;
	string	_iqId;
};

class loadFeedReplyCountTask : public MyUtil::Task{
public:
  loadFeedReplyCountTask(const JidPtr& jid, const LongSeq& feeds, const string& iqid, const string& xmlns):_jid(jid), _feeds(feeds), _iqid(iqid), _xmlns(xmlns){
  };
  virtual void handle();
private:
  JidPtr _jid;
  LongSeq _feeds;
  string _iqid;
  string _xmlns;
};

class deliverNotifyWebpagerTask: public MyUtil::Task{
	public:
	  deliverNotifyWebpagerTask(const MyUtil::IntSeq& toids, const string& content):Task(-1), _toids(toids),_content(content){
			  };
	virtual void handle();
	private:
	MyUtil::IntSeq _toids;
	string _content;
};

class deliverNotifyClientTask: public MyUtil::Task{
	public:
		deliverNotifyClientTask(const MyUtil::IntSeq& toids, const string& content):Task(-1), _toids(toids),_content(content){
		};
		virtual void handle();
	private:
		MyUtil::IntSeq _toids;
		string _content;
};

class deliverNotifyPhoneTask: public MyUtil::Task{
	public:
		deliverNotifyPhoneTask(const MyUtil::IntSeq& toids, const string& content):Task(-1), _toids(toids),_content(content){
		};
		virtual void handle();
	private:
		MyUtil::IntSeq _toids;
		string _content;
};

class deliverNotifyGroupTask : public MyUtil::Task{
public:
	deliverNotifyGroupTask(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content, const char& endpoint):Task(121), _groupids(groupids), _touids(touids), _blockuids(blockuids), _content(content), _endpoint(endpoint){
	};
	virtual void handle();
private:
	MyUtil::IntSeq _groupids;
	MyUtil::IntSeq _touids;
	MyUtil::IntSeq _blockuids;
	string _content;
	char _endpoint;
};


class WTransitManagerI : public WTransitManager,
	public MyUtil::Singleton<WTransitManagerI>
{
public:

	void PrintLog(const string& method, Ice::Long objid, const MyUtil::IntSeq& target);	

	//static const Ice::Long baseReplyId = 5000000000000000000;
	static const Ice::Long baseFeedId = 6100000000000000000;
	//virtual void deliverNotify(Ice::Int userid, const string& title,
	//		const string& msg, const Ice::Current& = Ice::Current());
	FeedContentDict GetFeedContents(const MyUtil::LongSeq& feedids);
	void ClientEncode(string& status);
	void LoadMissFeedContent(FeedContentDict& contents, MyUtil::LongSeq& feedids);
	virtual void deliverFeed(const FeedSeedPtr& feed, const MyUtil::IntSeq& target, const Ice::Current& = Ice::Current());
	virtual void deliverFeedReply( const ReplyDataPtr& replyData, const MyUtil::IntSeq& target,
								  const Ice::Current& = Ice::Current());
	virtual void loadOfflineFeedReply(const JidPtr& jid, int stype, int actor, Ice::Long source, const string& iqId, const Ice::Current& = Ice::Current());
	virtual void loadOfflineFeed(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex,
								 Ice::Int iqId, const Ice::Current& = Ice::Current());
	map<Ice::Long, int> getFeedReplyCount(const LongSeq& feeds, const Ice::Current& = Ice::Current());
	//virtual void deliverReply(const NotifyContentPtr& content, const Ice::Current& = Ice::Current());
	virtual void deliverNotifyWebpager(const MyUtil::IntSeq& toids, const string& content, const Ice::Current& = Ice::Current());
	virtual void deliverNotifyClient(const MyUtil::IntSeq& toids, const string& content, const Ice::Current& = Ice::Current());
	virtual void deliverNotifyPhone(const MyUtil::IntSeq& toids, const string& content, const Ice::Current& = Ice::Current());
	virtual void deliverAppNotice(Ice::Int toid,Ice::Int appid,const string& appName, const string& appIcon, const string& msg, const Ice::Current& = Ice::Current());
  virtual void deliverFeedReplyCount(const JidPtr& jid, const LongSeq& feeds, const string& iqid, const string& xmlns, const Ice::Current& = Ice::Current());


	virtual void deliverGroupNotifyPager(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content, const Ice::Current& = Ice::Current());
	virtual void deliverGroupNotifyClient(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content, const Ice::Current& = Ice::Current());
	virtual void deliverGroupNotifyPhone(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content, const Ice::Current& = Ice::Current());

	void CallFeedCache(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex, int iqId, const Ice::Current& = Ice::Current());
	void CallFeedCacheAndContent(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex, int iqId, const Ice::Current& = Ice::Current());
	void CallNotifyGate(const JidPtr& jid, Ice::Long feedIndex, Ice::Long replyIndex, int iqId, const Ice::Current& = Ice::Current());


  bool isDeliver2Client(int stype);
	
	void SwapFeedSeq(FeedWithTargetSeq& seq);
	void SwapFeedReplySeq(FeedReplyWithTargetSeq& seq);
	string GetServerProperties(const string& key);
private:
	IceUtil::Mutex _seqmutex;
	IceUtil::Mutex _promutex;
	FeedWithTargetSeq _feedseq;
	FeedReplyWithTargetSeq _feedreplyseq;
	map<string, string> _properties;
};


	

	
};

};




#endif /* _WTRANSITI_H_ */
