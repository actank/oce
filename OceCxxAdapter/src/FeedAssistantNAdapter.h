/*
 * FeedAssistantNAdapter.h
 *
 *  Created on: Jan 15, 2010
 *      Author: antonio
 */

#ifndef FEEDASSISTANTNADAPTER_H_
#define FEEDASSISTANTNADAPTER_H_

#include "RFeed.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce{
namespace feed{

using namespace MyUtil;

class FeedAssistantNAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedChannel, FeedAssistantNAdapter>
{
public:
	FeedAssistantNAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	};
	~FeedAssistantNAdapter();
	void removeFeedById(Ice::Long feedId);
	void removeFeedBySource(Ice::Long source, int stype);
	void removeFeedByParent(Ice::Long parent, int ptype);
	void removeFeed(Ice::Long source, int stype, int actor);


	void removeFeedReply(Ice::Long source, int stype, int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply);
	void removeFeedReplyById(int stype, Ice::Long feedid, Ice::Long removeReplyId, const FeedReplyPtr& reply);



	void readNews(int user, Ice::Long feed);
	void readAllNews(int user);
	//void readMini(int user, Ice::Long feedid);

	void dispatch(const FeedSeedPtr& seed);
	void dispatchEDM(const FeedSeedPtr& seed);
	void dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply);

	void addFeedReply(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply);
	void addFeedReplyById(int stype, Ice::Long feedid, const FeedReplyPtr & reply);
  void addFeedReplyWithConfig(Ice::Long source, Ice::Int stype,
      Ice::Int actor, const FeedReplyPtr& reply, const map<string,string> & reply_config);

	void cacheSync(Ice::Long source, int stype, int actor);

	Ice::Long getFeedId(Ice::Long source, int stype, int actor);

	void dispatchAgain(Ice::Long source, int stype, int actor, int toid);

	void ReplaceXML(Ice::Long source, int stype, int actor,const string & xml );

	 void DirectedSend(Ice::Long source, int stype, int actor,const vector<int> & targets);

protected:
	virtual string name() {
		return "FeedAssistantN";
	}
	virtual string endpoints() {
		return "@FeedAssistantN";
	}
	virtual size_t cluster() {
		return 10;
	}

	FeedAssistantPrx getManager(Ice::Long source);
	FeedAssistantPrx getManagerOneway(Ice::Long source);

	vector<FeedAssistantPrx> _managersOneway;
	vector<FeedAssistantPrx> _managers;
};


};
};

#endif /* FEEDASSISTANTNADAPTER_H_ */
