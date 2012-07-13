#ifndef __WT_TRANSIT_ICE__
#define __WT_TRANSIT_ICE__

#include <RFeed.ice>
#include <TalkCommon.ice>
//#include <Dispatch.ice>
#include <Notify.ice>

module talk
{
module wtransit
{

class WTransitManager{
	//void deliverNotify(int userid, string title, string msg); 
	void deliverFeed(xce::feed::FeedSeed seed, MyUtil::IntSeq target);
	void deliverFeedReply( xce::feed::ReplyData data, MyUtil::IntSeq target);
	void loadOfflineFeed(com::xiaonei::talk::common::Jid jid, long feedIndex, long replyIndex, int iqId);
	//void deliverReply(xce::notify::NotifyContent content);
	void deliverNotifyWebpager(MyUtil::IntSeq toids, string content);
	void deliverNotifyClient(MyUtil::IntSeq toids, string content);
	void deliverNotifyPhone(MyUtil::IntSeq toids, string content);

	void deliverGroupNotifyPager(MyUtil::IntSeq groupids, MyUtil::IntSeq touids, MyUtil::IntSeq blockuids, string content);
	void deliverGroupNotifyClient(MyUtil::IntSeq groupids, MyUtil::IntSeq touids, MyUtil::IntSeq blockuids, string content);
	void deliverGroupNotifyPhone(MyUtil::IntSeq groupids, MyUtil::IntSeq touids, MyUtil::IntSeq blockuids, string content);

	void deliverAppNotice(int toid,int appid,string appName, string appIcon, string msg);
	MyUtil::Long2IntMap getFeedReplyCount(MyUtil::LongSeq feeds);
  void deliverFeedReplyCount(com::xiaonei::talk::common::Jid jid, MyUtil::LongSeq feeds, string iqid, string xmlns);
	void loadOfflineFeedReply(com::xiaonei::talk::common::Jid jid, int stype, int actor, long source, string iqId);

	//--------------------------BUG ERROR TEST-------------------
	void CallFeedCache(com::xiaonei::talk::common::Jid jid, long feedIndex, long replyIndex, int iqId);
	void CallFeedCacheAndContent(com::xiaonei::talk::common::Jid jid, long feedIndex, long replyIndex, int iqId);
	void CallNotifyGate(com::xiaonei::talk::common::Jid jid, long feedIndex, long replyIndex, int iqId);
	//--------------------------BUG ERROR TEST-------------------
};

};
};
#endif
