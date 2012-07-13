#include "TalkIqAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::adapter;


IqHandlerPrx TalkIqAdapter::getManager(int id) {
	return locate<IqHandlerPrx>(_managers, "M", id, TWO_WAY);
}

IqHandlerPrx TalkIqAdapter::getManagerOneway(int id) {
	return locate<IqHandlerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------


void TalkIqAdapter::removeBuddyNotify(int host, int guest)
{
	getManagerOneway(host)->removeBuddyNotify(host,guest);
}



void TalkIqAdapter::message(const JidPtr& host, const string& msg){
	getManagerOneway(host->userId)->message(host,msg);
}

void TalkIqAdapter::changeBuddyGroupNotify(const BuddyItemPtr& bi){
	getManagerOneway(bi->host)->changeBuddyGroupNotify(bi);
}
