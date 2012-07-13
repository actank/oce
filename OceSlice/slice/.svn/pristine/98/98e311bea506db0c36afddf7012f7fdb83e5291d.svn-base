#ifndef __WT_EVENT_ICE__
#define __WT_EVENT_ICE__

#include <XFeed.ice>
#include <TalkCommon.ice>
//#include <Dispatch.ice>

module talk
{
module wevent
{

class WEventManager{
	void deliverNotify(int userid, string title, string msg); 
	void deliverXFeed(xce::xfeed::XFeedLetter xftl);
	void loadOfflineXFeed(com::xiaonei::talk::common::Jid jid, long feedIndex, long replyIndex, int iqId);
//	void deliverReply(mop::hi::svc::dispatch::Note note);
	//void loadUnreadReply(com::xiaonei::talk::common::Jid jid);
	void deliverAppNotice(int toid,int appid,string appName, string appIcon, string msg);
};

};
};
#endif
