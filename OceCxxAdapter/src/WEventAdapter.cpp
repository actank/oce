#include "WEventAdapter.h"

using namespace talk::adapter;
using namespace MyUtil;

WEventManagerPrx WEventAdapter::getManagerOneway(int id) {
	return locate<WEventManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

void WEventAdapter::deliverNotify(Ice::Int userid,const string& title, const string& msg){
	getManagerOneway(userid)->deliverNotify(userid, title, msg);
}

void WEventAdapter::deliverXFeed(const XFeedLetterPtr& xftl) {
	getManagerOneway(xftl->actor)->deliverXFeed(xftl);
}

void WEventAdapter::loadOfflineXFeed(const JidPtr& jid, Ice::Long feedIndex,Ice::Long replyIndex, int iqId)
{
//	if(jid->userId == 128487631 || jid->userId == 227366242 || jid->userId == 200600528 || jid->userId == 228132541 ||jid->userId == 227805680){
//		_channel->uncheckedCast<WEventManagerPrx>(_channel->locate(
//								"M", "@WEvent11", ONE_WAY))->loadOfflineXFeed(jid,beginIndex,iqId);
//		return;
//	}
	getManagerOneway(jid->userId)->loadOfflineXFeed(jid,feedIndex,replyIndex,iqId);
}

/*void WEventAdapter::deliverReply(const mop::hi::svc::dispatch::NotePtr& note){
	getManagerOneway(note->toId)->deliverReply(note);
}*/
