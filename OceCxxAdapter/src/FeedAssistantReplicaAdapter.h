#ifndef FEEDASSISTANT_REPLICA_ADAPTER_H_
#define FEEDASSISTANT_REPLICA_ADAPTER_H_

#include "RFeed.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce{
namespace feed{

using namespace MyUtil;

class FeedAssistantReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<FeedAssistantPrx>,
   public MyUtil::Singleton<FeedAssistantReplicaAdapter> 
{
public:
  FeedAssistantReplicaAdapter() : MyUtil::ReplicatedClusterAdapterI<FeedAssistantPrx> ("ControllerFeedAssistantR",120,300,new XceFeedControllerChannel()){
      }

	~FeedAssistantReplicaAdapter();
	void removeFeedById(Ice::Long feedId);
	void removeFeedBySource(Ice::Long source, int stype);
	void removeFeedByParent(Ice::Long parent, int ptype);
	void removeFeed(Ice::Long source, int stype, int actor);


	void removeFeedReply(Ice::Long source, int stype, int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply);
	//void removeFeedReplyById(int stype, Ice::Long feedid, Ice::Long removeReplyId, const FeedReplyPtr& reply);



	void readNews(int user, Ice::Long feed);
	void readAllNews(int user);
	//void readMini(int user, Ice::Long feedid);

	void dispatch(const FeedSeedPtr& seed);
	void dispatchEDM(const FeedSeedPtr& seed);
	void dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply);

	void addFeedReply(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply);
	Ice::Long createFeedId();
	//void addFeedReplyById(int stype, Ice::Long feedid, const FeedReplyPtr & reply);
  //void addFeedReplyWithConfig(Ice::Long source, Ice::Int stype,
   //   Ice::Int actor, const FeedReplyPtr& reply, const map<string,string> & reply_config);

	//void cacheSync(Ice::Long source, int stype, int actor);

	//Ice::Long getFeedId(Ice::Long source, int stype, int actor);

	//void dispatchAgain(Ice::Long source, int stype, int actor, int toid);

	//void ReplaceXML(Ice::Long source, int stype, int actor,const string & xml );

	 //void DirectedSend(Ice::Long source, int stype, int actor,const vector<int> & targets);

protected:
	FeedAssistantPrx getManager(Ice::Long source);
	FeedAssistantPrx getManagerOneway(Ice::Long source);

};


};
};

#endif 
