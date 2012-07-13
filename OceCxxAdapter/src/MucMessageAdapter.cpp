#include "MucMessageAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::adapter;


MessageHandlerPrx MucMessageAdapter::getManager(int id) {
	return locate<MessageHandlerPrx>(_managers, "M", id, TWO_WAY);
}

MessageHandlerPrx MucMessageAdapter::getManagerOneway(int id) {
	return locate<MessageHandlerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------



void MucMessageAdapter::message(const JidPtr& host, const string& msg){
	getManagerOneway(host->userId)->message(host,msg);
}
//void TalkMessageAdapter::chat(const MessagePtr& msg, const Ice::Context& ctx) {
//	getManagerOneway(msg->to->userId)->chat(msg, ctx);
//}
//
/*
void MucMessageAdapter::loadOfflineMessage(const JidPtr& jid){
	getManagerOneway(jid->userId)->loadOfflineMessage(jid);
}
*/
