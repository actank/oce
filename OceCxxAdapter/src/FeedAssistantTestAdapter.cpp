#include "FeedAssistantTestAdapter.h"

using namespace xce::feed;


FeedAssistantTestAdapter::FeedAssistantTestAdapter()
{
	_managers.resize(cluster() > 0 ? cluster() : 1);
	_managersOneway.resize(cluster() > 0 ? cluster() : 1);
}

FeedAssistantTestAdapter::~FeedAssistantTestAdapter()
{

}

FeedAssistantPrx FeedAssistantTestAdapter::getManager()
{
	return locate<FeedAssistantPrx> (_managers, "FA", 0, TWO_WAY);
}

FeedAssistantPrx FeedAssistantTestAdapter::getManagerOneway()
{
	return locate<FeedAssistantPrx> (_managersOneway, "FA", 0, ONE_WAY);
}

void FeedAssistantTestAdapter::removeFeedById(Ice::Long feedId)
{
	getManagerOneway()->removeFeedById(feedId);
}

void FeedAssistantTestAdapter::removeFeedBySource(Ice::Long source, int stype)
{
	getManagerOneway()->removeFeedBySource(source, stype);
}

void FeedAssistantTestAdapter::removeFeedByParent(Ice::Long parent, int ptype)
{
	getManagerOneway()->removeFeedByParent(parent, ptype);
}

void FeedAssistantTestAdapter::removeFeed(Ice::Long source, int stype, int actor)
{
	getManagerOneway()->removeFeed(source, stype, actor);
}


void FeedAssistantTestAdapter::removeFeedReply(Ice::Long source, int stype, int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply)
{

	getManagerOneway()->removeFeedReply(source, stype, actor, removeReplyId, reply);

}

void FeedAssistantTestAdapter::readNews(int user,Ice::Long feed)
{

	getManagerOneway()->readNews(user,feed);

}

void FeedAssistantTestAdapter::readAllNews(int user)
{
	getManagerOneway()->readAllNews(user);
}

void FeedAssistantTestAdapter::dispatch(const FeedSeedPtr& seed)
{
	getManagerOneway()->dispatch(seed);
}

void FeedAssistantTestAdapter::dispatchEDM(const FeedSeedPtr& seed)
{
	getManagerOneway()->dispatchEDM(seed);
}
void FeedAssistantTestAdapter::dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply){
  getManagerOneway()->dispatchWithReply(seed,reply);
}
Ice::Long FeedAssistantTestAdapter::createFeedId(){
	return getManager()->createFeedId();
}

void FeedAssistantTestAdapter::addFeedReply(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply)
{
	getManagerOneway()->addFeedReply(source, stype, actor,  reply);
}
