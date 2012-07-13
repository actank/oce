#include "FeedAssistantReplicaAdapter.h"

using namespace xce::feed;


FeedAssistantReplicaAdapter::~FeedAssistantReplicaAdapter()
{

}

FeedAssistantPrx FeedAssistantReplicaAdapter::getManager(Ice::Long source )
{
   return getProxy(source);
	//return locate<FeedAssistantPrx> (_managers, "FA", source, TWO_WAY);
}

FeedAssistantPrx FeedAssistantReplicaAdapter::getManagerOneway(Ice::Long source)
{
  return getProxyOneway(source);
	//return locate<FeedAssistantPrx> (_managersOneway, "FA",source, ONE_WAY);
}

void FeedAssistantReplicaAdapter::removeFeedById(Ice::Long feedId)
{
	getManagerOneway(feedId)->removeFeedById(feedId);
}

void FeedAssistantReplicaAdapter::removeFeedBySource(Ice::Long source, int stype)
{
	getManagerOneway(source)->removeFeedBySource(source, stype);
}

void FeedAssistantReplicaAdapter::removeFeedByParent(Ice::Long parent, int ptype)
{
	getManagerOneway(parent)->removeFeedByParent(parent, ptype);
}

void FeedAssistantReplicaAdapter::removeFeed(Ice::Long source, int stype, int actor)
{
	getManagerOneway(source)->removeFeed(source, stype, actor);
}


void FeedAssistantReplicaAdapter::removeFeedReply(Ice::Long source, int stype, int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply)
{
	getManagerOneway(source)->removeFeedReply(source, stype, actor, removeReplyId, reply);
}


void FeedAssistantReplicaAdapter::readNews(int user,Ice::Long feed)
{

	getManagerOneway(feed)->readNews(user,feed);

}

void FeedAssistantReplicaAdapter::readAllNews(int user)
{
	getManagerOneway(user)->readAllNews(user);
}

void FeedAssistantReplicaAdapter::dispatch(const FeedSeedPtr& seed)
{
	//getManagerOneway(seed->type & 0x00ffffff)->dispatch(seed);

	getManagerOneway(seed->source)->dispatch(seed);
}

void FeedAssistantReplicaAdapter::dispatchEDM(const FeedSeedPtr& seed)
{
	//getManagerOneway(seed->type & 0x00ffffff)->dispatchEDM(seed);
	getManagerOneway(seed->source)->dispatchEDM(seed);
}

void FeedAssistantReplicaAdapter::dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply){
	//getManagerOneway(seed->type & 0x00ffffff)->dispatchWithReply(seed,reply);
	getManagerOneway(seed->source)->dispatchWithReply(seed,reply);
}


void FeedAssistantReplicaAdapter::addFeedReply(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply)
{
	getManagerOneway(source)->addFeedReply(source, stype, actor,  reply);
}

Ice::Long FeedAssistantReplicaAdapter::createFeedId(){
  return getManager(0)->createFeedId();
}




