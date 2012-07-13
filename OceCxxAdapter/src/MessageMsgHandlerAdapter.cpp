#include "MessageMsgHandlerAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::adapter;


MessageHandlerPrx MessageMsgHandlerAdapter::getManager(int id) {
	return locate<MessageHandlerPrx>(_managers, "M", id, TWO_WAY);
}

MessageHandlerPrx MessageMsgHandlerAdapter::getManagerOneway(int id) {
	return locate<MessageHandlerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------



void MessageMsgHandlerAdapter::message(const JidPtr& host, const string& msg, const Ice::Context& ctx){
	getManagerOneway(host->userId)->message(host,msg, ctx);
}


void MessageMsgHandlerAdapter::deliverAppNotice(int toid, int appid, const string& appName, const string& appIcon, const string& msg){
	  getManagerOneway(toid)->deliverAppNotice(toid, appid, appName, appIcon, msg);
}

//void MessageMsgHandlerAdapter::chat(const MessagePtr& msg, const Ice::Context& ctx) {
//	getManagerOneway(msg->to->userId)->chat(msg, ctx);
//}
//
/*
void MessageMsgHandlerAdapter::loadOfflineMessage(const JidPtr& jid){
	getManagerOneway(jid->userId)->loadOfflineMessage(jid);
}*/

