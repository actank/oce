#include "FeedAssistantAdapter.h"

using namespace xce::feed;


FeedAssistantAdapter::FeedAssistantAdapter()
{
	_managers.resize(cluster() > 0 ? cluster() : 1);
	_managersOneway.resize(cluster() > 0 ? cluster() : 1);
}

FeedAssistantAdapter::~FeedAssistantAdapter()
{

}

FeedAssistantPrx FeedAssistantAdapter::getManager()
{
	return locate<FeedAssistantPrx> (_managers, "FA", 0, TWO_WAY);
}

FeedAssistantPrx FeedAssistantAdapter::getManagerOneway()
{
	return locate<FeedAssistantPrx> (_managersOneway, "FA", 0, ONE_WAY);
}

void FeedAssistantAdapter::removeFeedById(Ice::Long feedId)
{
	getManagerOneway()->removeFeedById(feedId);
}

void FeedAssistantAdapter::removeFeedBySource(Ice::Long source, int stype)
{
	getManagerOneway()->removeFeedBySource(source, stype);
}

void FeedAssistantAdapter::removeFeedByParent(Ice::Long parent, int ptype)
{
	getManagerOneway()->removeFeedByParent(parent, ptype);
}

void FeedAssistantAdapter::removeFeed(Ice::Long source, int stype, int actor)
{
	getManagerOneway()->removeFeed(source, stype, actor);
}


void FeedAssistantAdapter::removeFeedReply(Ice::Long source, int stype, int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply)
{

	getManagerOneway()->removeFeedReply(source, stype, actor, removeReplyId, reply);

}

void FeedAssistantAdapter::readNews(int user,Ice::Long feed)
{

	getManagerOneway()->readNews(user,feed);

}

void FeedAssistantAdapter::readAllNews(int user)
{
	getManagerOneway()->readAllNews(user);
}

void FeedAssistantAdapter::dispatch(const FeedSeedPtr& seed)
{
	getManagerOneway()->dispatch(seed);
}

void FeedAssistantAdapter::dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply){
	getManagerOneway()->dispatchWithReply(seed,reply);
}

void FeedAssistantAdapter::dispatchEDM(const FeedSeedPtr& seed)
{
	getManagerOneway()->dispatchEDM(seed);
}
Ice::Long FeedAssistantAdapter::createFeedId(){
	return getManager()->createFeedId();
}

void FeedAssistantAdapter::addFeedReply(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply)
{
	getManagerOneway()->addFeedReply(source, stype, actor,  reply);
}
