#include "WTransitForIqAdapter.h"

using namespace talk::adapter;
using namespace MyUtil;

WTransitManagerPrx WTransitForIqAdapter::getManagerOneway(int id) {
	return getProxy(id);
}
WTransitManagerPrx WTransitForIqAdapter::getManager(int id) {
	return getProxyOneway(id);
}

/*void WTransitForIqAdapter::deliverNotify(Ice::Int userid,const string& title, const string& msg){
	getManagerOneway(userid)->deliverNotify(userid, title, msg);
} */

void WTransitForIqAdapter::deliverFeed(const FeedSeedPtr& feed, const MyUtil::IntSeq& target) {
	getManagerOneway(feed->actor)->deliverFeed(feed,target);
}

void WTransitForIqAdapter::deliverFeedReply( const ReplyDataPtr& replyData, const MyUtil::IntSeq& target){
	getManagerOneway(replyData->actor)->deliverFeedReply(replyData,target);
}

void WTransitForIqAdapter::loadOfflineFeed(const JidPtr& jid, Ice::Long feedIndex,Ice::Long replyIndex, int iqId)
{
	// if(jid->userId == 128487631 || jid->userId == 227366242 || jid->userId == 200600528 || jid->userId == 228132541 ||jid->userId == 227805680){
	// 	_channel->uncheckedCast<WTransitForIqManagerPrx>(_channel->locate(
	// 							"M", "@WTransitForIq11", ONE_WAY))->loadOfflineFeed(jid,beginIndex,iqId);
	// 	return;
	// }
	getManagerOneway(jid->userId)->loadOfflineFeed(jid,feedIndex,replyIndex,iqId);
}

//void WTransitForIqAdapter::deliverReply(const mop::hi::svc::dispatch::NotePtr& note){
	//getManagerOneway(note->toId)->deliverReply(note);
//}
void WTransitForIqAdapter::deliverNotifyWebpager(const MyUtil::IntSeq& toids, const string& content){
	map<int, vector<int> > splits;
	for(int i=0; i<toids.size(); i++){
		splits[toids.at(i)%cluster()].push_back(toids.at(i));
	}
	map<int, vector<int> >::iterator it = splits.begin();
	for(; it!=splits.end(); ++it){
		getManagerOneway(it->first)->deliverNotifyWebpager(it->second, content);
	}
}
void WTransitForIqAdapter::deliverNotifyClient(const MyUtil::IntSeq& toids, const string& content){
	map<int, vector<int> > splits;
	for(int i=0; i<toids.size(); i++){
		splits[toids.at(i)%cluster()].push_back(toids.at(i));
	}
	map<int, vector<int> >::iterator it = splits.begin();
	for(; it!=splits.end(); ++it){
		getManagerOneway(it->first)->deliverNotifyClient(it->second, content);
	}
}
void WTransitForIqAdapter::deliverNotifyPhone(const MyUtil::IntSeq& toids, const string& content){
	map<int, vector<int> > splits;
	for(int i=0; i<toids.size(); i++){
		splits[toids.at(i)%cluster()].push_back(toids.at(i));
	}
	map<int, vector<int> >::iterator it = splits.begin();
	for(; it!=splits.end(); ++it){
		getManagerOneway(it->first)->deliverNotifyPhone(it->second, content);
	}
}

void WTransitForIqAdapter::deliverAppNotice(int toid, int appid, const string& appName, const string& appIcon, const string& msg){
	getManagerOneway(toid)->deliverAppNotice(toid, appid, appName, appIcon, msg);
}

void WTransitForIqAdapter::loadOfflineFeedReply(const JidPtr& jid, int stype, int owner, Ice::Long source, const string& iqId){
	if(jid){
		getManagerOneway(jid->userId)->loadOfflineFeedReply(jid, stype, owner, source, iqId);
	}
}

map<Ice::Long, int> WTransitForIqAdapter::getFeedReplyCount(int userid, const vector<Ice::Long>& feeds){
	return getManager(userid)->getFeedReplyCount(feeds);
}

void WTransitForIqAdapter::deliverFeedReplyCount(const JidPtr& jid, const vector<Ice::Long>& feeds, const string& iqid, const string& xmlns){
  return getManagerOneway(jid->userId)->deliverFeedReplyCount(jid, feeds, iqid, xmlns);
}
