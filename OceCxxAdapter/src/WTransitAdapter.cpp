#include "WTransitAdapter.h"

using namespace talk::adapter;
using namespace MyUtil;

WTransitManagerPrx WTransitAdapter::getManagerOneway(int id, bool first15) {
	/*
	if(first15){
		id %= 15;
	}else{
		id = 15 + (id % 5);
	}*/
	return locate<WTransitManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}
WTransitManagerPrx WTransitAdapter::getManager(int id, bool first15) {
	/*
	if(first15){
		id %= 15;
	}else{
		id = 15 + (id % 5);
	}
	*/
	return locate<WTransitManagerPrx>(_managers, "M", id, TWO_WAY);
}

/*void WTransitAdapter::deliverNotify(Ice::Int userid,const string& title, const string& msg){
	getManagerOneway(userid)->deliverNotify(userid, title, msg);
} */

void WTransitAdapter::deliverGroupNotifyPager(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content){
	int id = 0;
	if(!groupids.empty()){
		id = groupids.at(0);
	}
	else if(!touids.empty()){
		id = touids.at(0);
	}else{
		return;
	}
	getManagerOneway(id)->deliverGroupNotifyPager(groupids, touids, blockuids, content);
}
void WTransitAdapter::deliverGroupNotifyClient(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content){
	int id = 0;
	if(!groupids.empty()){
		id = groupids.at(0);
	}
	else if(!touids.empty()){
		id = touids.at(0);
	}else{
		return;
	}
	getManagerOneway(id)->deliverGroupNotifyClient(groupids, touids, blockuids, content);
}
void WTransitAdapter::deliverGroupNotifyPhone(const MyUtil::IntSeq& groupids, const MyUtil::IntSeq& touids, const MyUtil::IntSeq& blockuids, const string& content){
	int id = 0;
	if(!groupids.empty()){
		id = groupids.at(0);
	}
	else if(!touids.empty()){
		id = touids.at(0);
	}else{
		return;
	}
	getManagerOneway(id)->deliverGroupNotifyPhone(groupids, touids, blockuids, content);
}
void WTransitAdapter::deliverFeed(const FeedSeedPtr& feed, const MyUtil::IntSeq& target) {
	getManagerOneway(feed->actor)->deliverFeed(feed,target);
}

void WTransitAdapter::deliverFeedReply( const ReplyDataPtr& replyData, const MyUtil::IntSeq& target){
	getManagerOneway(replyData->actor)->deliverFeedReply(replyData,target);
}

void WTransitAdapter::loadOfflineFeed(const JidPtr& jid, Ice::Long feedIndex,Ice::Long replyIndex, int iqId)
{
	// if(jid->userId == 128487631 || jid->userId == 227366242 || jid->userId == 200600528 || jid->userId == 228132541 ||jid->userId == 227805680){
	// 	_channel->uncheckedCast<WTransitManagerPrx>(_channel->locate(
	// 							"M", "@WTransit11", ONE_WAY))->loadOfflineFeed(jid,beginIndex,iqId);
	// 	return;
	// }
	int id =4 + jid->userId % 2;
	getManagerOneway(id)->loadOfflineFeed(jid,feedIndex,replyIndex,iqId);
}

//void WTransitAdapter::deliverReply(const mop::hi::svc::dispatch::NotePtr& note){
	//getManagerOneway(note->toId)->deliverReply(note);
//}
void WTransitAdapter::deliverNotifyWebpager(const MyUtil::IntSeq& toids, const string& content){
	map<int, vector<int> > splits;
	for(int i=0; i<toids.size(); i++){
		splits[toids.at(i)%cluster()].push_back(toids.at(i));
	}
	map<int, vector<int> >::iterator it = splits.begin();
	for(; it!=splits.end(); ++it){
		getManagerOneway(it->first)->deliverNotifyWebpager(it->second, content);
	}
}
void WTransitAdapter::deliverNotifyClient(const MyUtil::IntSeq& toids, const string& content){
	map<int, vector<int> > splits;
	for(int i=0; i<toids.size(); i++){
		splits[toids.at(i)%cluster()].push_back(toids.at(i));
	}
	map<int, vector<int> >::iterator it = splits.begin();
	for(; it!=splits.end(); ++it){
		getManagerOneway(it->first)->deliverNotifyClient(it->second, content);
	}
}
void WTransitAdapter::deliverNotifyPhone(const MyUtil::IntSeq& toids, const string& content){
	map<int, vector<int> > splits;
	for(int i=0; i<toids.size(); i++){
		splits[toids.at(i)%cluster()].push_back(toids.at(i));
	}
	map<int, vector<int> >::iterator it = splits.begin();
	for(; it!=splits.end(); ++it){
		getManagerOneway(it->first)->deliverNotifyPhone(it->second, content);
	}
}

void WTransitAdapter::deliverAppNotice(int toid, int appid, const string& appName, const string& appIcon, const string& msg){
	getManagerOneway(toid)->deliverAppNotice(toid, appid, appName, appIcon, msg);
}

void WTransitAdapter::loadOfflineFeedReply(const JidPtr& jid, int stype, int owner, Ice::Long source, const string& iqId){
	if(jid){
		//getManagerOneway(jid->userId)->loadOfflineFeedReply(jid, stype, owner, source, iqId);
		int id = jid->userId % 2;
		getManagerOneway(id)->loadOfflineFeedReply(jid, stype, owner, source, iqId);
	}
}

map<Ice::Long, int> WTransitAdapter::getFeedReplyCount(int userid, const vector<Ice::Long>& feeds){
	return getManager(userid)->getFeedReplyCount(feeds);
}

void WTransitAdapter::deliverFeedReplyCount(const JidPtr& jid, const vector<Ice::Long>& feeds, const string& iqid, const string& xmlns){
	int id = 2 + jid->userId % 2;
  getManagerOneway(id)->deliverFeedReplyCount(jid, feeds, iqid, xmlns);
}



void WTransitAdapter::CallFeedCache(const JidPtr& jid, long feedIndex, long replyIndex, int iqId){
	int id =4 + jid->userId % 2;
	getManagerOneway(id)->CallFeedCache(jid,feedIndex,replyIndex,iqId);
}
void WTransitAdapter::CallFeedCacheAndContent(const JidPtr& jid, long feedIndex, long replyIndex, int iqId){
	int id =15 + jid->userId % 2;
	getManagerOneway(id)->CallFeedCacheAndContent(jid,feedIndex,replyIndex,iqId);
	id =17 + jid->userId % 2;
	getManagerOneway(id)->CallFeedCacheAndContent(jid,feedIndex,replyIndex,iqId);
}
void WTransitAdapter::CallNotifyGate(const JidPtr& jid, long feedIndex, long replyIndex, int iqId){
	int id =6 + jid->userId % 2;
	getManagerOneway(id)->CallNotifyGate(jid,feedIndex,replyIndex,iqId);
}



