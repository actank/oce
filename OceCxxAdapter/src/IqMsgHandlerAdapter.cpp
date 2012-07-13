#include "IqMsgHandlerAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::adapter;


IqHandlerPrx IqMsgHandlerAdapter::getManager(int id) {
	return getProxy(id);
	//return locate<IqHandlerPrx>(_managers, "M", id, TWO_WAY);
}

IqHandlerPrx IqMsgHandlerAdapter::getManagerOneway(int id) {
	return getProxyOneway(id);
	//return locate<IqHandlerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

void IqMsgHandlerAdapter::RemoveNotifyBySource(int userId, int type, Ice::Long source){
	getManagerOneway(userId)->RemoveNotifyBySource(userId, type, source);
}
void IqMsgHandlerAdapter::RemoveNotifyById(int userId, Ice::Long notifyId){
  getManagerOneway(userId)->RemoveNotifyById(userId, notifyId);
}

void IqMsgHandlerAdapter::NotifyBuddyApplyChange(int userId){
	getManagerOneway(userId)->NotifyBuddyApplyChange(userId);
}

void IqMsgHandlerAdapter::removeBuddyNotify(int host, int guest)
{
	getManagerOneway(host)->removeBuddyNotify(host,guest);
}



void IqMsgHandlerAdapter::message(const JidPtr& host, const string& msg){
	getManagerOneway(host->userId)->message(host,msg);
}

void IqMsgHandlerAdapter::changeBuddyGroupNotify(const BuddyItemPtr& bi){
	getManagerOneway(bi->host)->changeBuddyGroupNotify(bi);
}
