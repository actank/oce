/*
 * FeedAssistantNAdapter.cpp
 *
 *  Created on: Jan 15, 2010
 *      Author: antonio
 */

#include "FeedAssistantNAdapter.h"

using namespace xce::feed;


//FeedAssistantNAdapter::FeedAssistantNAdapter()
//{
//	_manager = locate<FeedAssistantPrx>("FA", TWO_WAY);
//	_managerOneway = locate<FeedAssistantPrx>("FA", ONE_WAY);
//}

FeedAssistantNAdapter::~FeedAssistantNAdapter()
{

}

FeedAssistantPrx FeedAssistantNAdapter::getManager(Ice::Long source )
{
	return locate<FeedAssistantPrx> (_managers, "FA", source, TWO_WAY);
}

FeedAssistantPrx FeedAssistantNAdapter::getManagerOneway(Ice::Long source)
{
	return locate<FeedAssistantPrx> (_managersOneway, "FA",source, ONE_WAY);
}

void FeedAssistantNAdapter::removeFeedById(Ice::Long feedId)
{
	getManagerOneway(feedId)->removeFeedById(feedId);
}

void FeedAssistantNAdapter::removeFeedBySource(Ice::Long source, int stype)
{
	getManagerOneway(source)->removeFeedBySource(source, stype);
}

void FeedAssistantNAdapter::removeFeedByParent(Ice::Long parent, int ptype)
{
	getManagerOneway(parent)->removeFeedByParent(parent, ptype);
}

void FeedAssistantNAdapter::removeFeed(Ice::Long source, int stype, int actor)
{
	getManagerOneway(source)->removeFeed(source, stype, actor);
}


void FeedAssistantNAdapter::removeFeedReply(Ice::Long source, int stype, int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply)
{
	getManagerOneway(source)->removeFeedReply(source, stype, actor, removeReplyId, reply);
}

void FeedAssistantNAdapter::removeFeedReplyById(int stype, Ice::Long feedid, Ice::Long removeReplyId, const FeedReplyPtr& reply)
{
	getManagerOneway(feedid)->removeFeedReplyById(stype, feedid, removeReplyId, reply);
}

void FeedAssistantNAdapter::readNews(int user,Ice::Long feed)
{

	getManagerOneway(feed)->readNews(user,feed);

}

void FeedAssistantNAdapter::readAllNews(int user)
{
	getManagerOneway(user)->readAllNews(user);
}

void FeedAssistantNAdapter::dispatch(const FeedSeedPtr& seed)
{
	//getManagerOneway(seed->type & 0x00ffffff)->dispatch(seed);
	getManagerOneway(seed->source)->dispatch(seed);
}

void FeedAssistantNAdapter::dispatchEDM(const FeedSeedPtr& seed)
{
	//getManagerOneway(seed->type & 0x00ffffff)->dispatchEDM(seed);
	getManagerOneway(seed->source)->dispatchEDM(seed);
}

void FeedAssistantNAdapter::dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply){
	//getManagerOneway(seed->type & 0x00ffffff)->dispatchWithReply(seed,reply);
	getManagerOneway(seed->source)->dispatchWithReply(seed,reply);
}


void FeedAssistantNAdapter::addFeedReply(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply)
{
	getManagerOneway(source)->addFeedReply(source, stype, actor,  reply);
}

void FeedAssistantNAdapter::addFeedReplyById(int stype, Ice::Long feedid, const FeedReplyPtr & reply)
{
	getManagerOneway(feedid)->addFeedReplyById(stype, feedid,  reply);
}
void FeedAssistantNAdapter::addFeedReplyWithConfig(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply,
    const map<string,string> & reply_config)
{
  getManagerOneway(source)->addFeedReplyWithConfig(source, stype, actor,  reply,reply_config);
}

void FeedAssistantNAdapter::cacheSync(Ice::Long source, int stype, int actor){
	getManagerOneway(source)->cacheSync(source,stype,actor);
}

Ice::Long FeedAssistantNAdapter::getFeedId(Ice::Long source, int stype, int actor){
  return getManager(source)->getFeedId(source, stype, actor);
}

void FeedAssistantNAdapter::dispatchAgain(Ice::Long source, int stype, int actor, int toid){
  getManagerOneway(source)->dispatchAgain(source,stype,actor,toid);
}
void FeedAssistantNAdapter::ReplaceXML(Ice::Long source, int stype, int actor,const string & xml ){
	getManagerOneway(source)->ReplaceXML(source,stype,actor,xml);
}
void FeedAssistantNAdapter::DirectedSend(Ice::Long source, int stype, int actor,const vector<int> & targets){
  getManagerOneway(source)->DirectedSend(source,stype,actor,targets);
}

