/*
 * FeedAssistantNReplicaAdapter.cpp
 *
 *  Created on: Jan 15, 2010
 *      Author: antonio
 */

#include "FeedAssistantNReplicaAdapter.h"

using namespace xce::feed;


//FeedAssistantNReplicaAdapter::FeedAssistantNReplicaAdapter()
//{
//	_manager = locate<FeedAssistantPrx>("FA", TWO_WAY);
//	_managerOneway = locate<FeedAssistantPrx>("FA", ONE_WAY);
//}

FeedAssistantNReplicaAdapter::~FeedAssistantNReplicaAdapter()
{

}

FeedAssistantPrx FeedAssistantNReplicaAdapter::getManager(Ice::Long source )
{
   return getProxy(source);
	//return locate<FeedAssistantPrx> (_managers, "FA", source, TWO_WAY);
}

FeedAssistantPrx FeedAssistantNReplicaAdapter::getManagerOneway(Ice::Long source)
{
  return getProxyOneway(source);
	//return locate<FeedAssistantPrx> (_managersOneway, "FA",source, ONE_WAY);
}

void FeedAssistantNReplicaAdapter::removeFeedById(Ice::Long feedId)
{
	getManagerOneway(feedId)->removeFeedById(feedId);
}

void FeedAssistantNReplicaAdapter::removeFeedBySource(Ice::Long source, int stype)
{
	getManagerOneway(source)->removeFeedBySource(source, stype);
}

void FeedAssistantNReplicaAdapter::removeFeedByParent(Ice::Long parent, int ptype)
{
	getManagerOneway(parent)->removeFeedByParent(parent, ptype);
}

void FeedAssistantNReplicaAdapter::removeFeed(Ice::Long source, int stype, int actor)
{
	getManagerOneway(source)->removeFeed(source, stype, actor);
}


void FeedAssistantNReplicaAdapter::removeFeedReply(Ice::Long source, int stype, int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply)
{
	getManagerOneway(source)->removeFeedReply(source, stype, actor, removeReplyId, reply);
}

void FeedAssistantNReplicaAdapter::removeFeedReplyById(int stype, Ice::Long feedid, Ice::Long removeReplyId, const FeedReplyPtr& reply)
{
	getManagerOneway(feedid)->removeFeedReplyById(stype, feedid, removeReplyId, reply);
}

void FeedAssistantNReplicaAdapter::readNews(int user,Ice::Long feed)
{

	getManagerOneway(feed)->readNews(user,feed);

}

void FeedAssistantNReplicaAdapter::readAllNews(int user)
{
	getManagerOneway(user)->readAllNews(user);
}

void FeedAssistantNReplicaAdapter::dispatch(const FeedSeedPtr& seed)
{
	//getManagerOneway(seed->type & 0x00ffffff)->dispatch(seed);

	getManagerOneway(seed->source)->dispatch(seed);
}

void FeedAssistantNReplicaAdapter::dispatchEDM(const FeedSeedPtr& seed)
{
	//getManagerOneway(seed->type & 0x00ffffff)->dispatchEDM(seed);
	getManagerOneway(seed->source)->dispatchEDM(seed);
}

void FeedAssistantNReplicaAdapter::dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply){
	//getManagerOneway(seed->type & 0x00ffffff)->dispatchWithReply(seed,reply);
	getManagerOneway(seed->source)->dispatchWithReply(seed,reply);
}


void FeedAssistantNReplicaAdapter::addFeedReply(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply)
{
	getManagerOneway(source)->addFeedReply(source, stype, actor,  reply);
}

void FeedAssistantNReplicaAdapter::addFeedReplyById(int stype, Ice::Long feedid, const FeedReplyPtr & reply)
{
	getManagerOneway(feedid)->addFeedReplyById(stype, feedid,  reply);
}
void FeedAssistantNReplicaAdapter::addFeedReplyWithConfig(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply,
    const map<string,string> & reply_config)
{
  getManagerOneway(source)->addFeedReplyWithConfig(source, stype, actor,  reply,reply_config);
}

void FeedAssistantNReplicaAdapter::cacheSync(Ice::Long source, int stype, int actor){
	getManagerOneway(source)->cacheSync(source,stype,actor);
}

Ice::Long FeedAssistantNReplicaAdapter::getFeedId(Ice::Long source, int stype, int actor){
  return getManager(source)->getFeedId(source, stype, actor);
}

void FeedAssistantNReplicaAdapter::dispatchAgain(Ice::Long source, int stype, int actor, int toid){
  getManagerOneway(source)->dispatchAgain(source,stype,actor,toid);
}
void FeedAssistantNReplicaAdapter::ReplaceXML(Ice::Long source, int stype, int actor,const string & xml ){
	getManagerOneway(source)->ReplaceXML(source,stype,actor,xml);
}
void FeedAssistantNReplicaAdapter::DirectedSend(Ice::Long source, int stype, int actor,const vector<int> & targets){
  getManagerOneway(source)->DirectedSend(source,stype,actor,targets);
}

