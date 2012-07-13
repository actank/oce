#include "MucPresenceMsgHandlerAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::adapter;


PresenceHandlerPrx MucPresenceMsgHandlerAdapter::getManager(int id) {
	return locate<PresenceHandlerPrx>(_managers, "M", id, TWO_WAY);
}

PresenceHandlerPrx MucPresenceMsgHandlerAdapter::getManagerOneway(int id) {
	return locate<PresenceHandlerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------


void MucPresenceMsgHandlerAdapter::presence(const JidPtr& j, const string& show, const string& ver, const string& subver, const string& ext){
//void MucPresenceMsgHandlerAdapter::presence(const JidPtr& j,string show, string ver, string ext){
	getManagerOneway(j->userId)->presence(j,show,ver,subver,ext);
}

void MucPresenceMsgHandlerAdapter::presenceWithSecver(const JidPtr& j, const string& show, const string& ver, const string& subver, const string& ext, const string& secver){
	getManagerOneway(j->userId)->presenceWithSecver(j,show,ver,subver,ext,secver);
}

//void MucPresenceMsgHandlerAdapter::broadcastPresence(const JidPtr& j,const string& show,const string& ver, const string& ext, const string& status,
//		const string& photo,const string& nick){
//	getManagerOneway(j->userId)->broadcastPresence(j,show,ver, ext,status,photo,nick);
//}


void MucPresenceMsgHandlerAdapter::presenceSubscribe(const string& id, int from, int to, const string& status){
	getManagerOneway(from)->presenceSubscribe(id,from,to,status);
}

void MucPresenceMsgHandlerAdapter::presenceSubscribeWithPostscript(const string& id, int from, int to,	
    const string& status, const string& postscript) {
	getManagerOneway(from)->presenceSubscribeWithPostscript(id,from,to,status, postscript);
}

void MucPresenceMsgHandlerAdapter::presenceSubscribed(const string& id, int from, int to){
	getManagerOneway(from)->presenceSubscribed(id,from,to);
}
void MucPresenceMsgHandlerAdapter::presenceUnsubscribed(const string& id, int from, int to){
	getManagerOneway(from)->presenceUnsubscribed(id,from,to);
}

void MucPresenceMsgHandlerAdapter::offlineNotify(const JidSeq& seq) {
    getManagerOneway(seq.size())->offlineNotify(seq);
}

void MucPresenceMsgHandlerAdapter::notifyBuddiesOnlineStat(const AvatarSeq& activeAvatars, const AvatarSeq& selfOtherAvatars,
			const AvatarMap& buddies, NotifyType type, const MyUtil::Str2StrMap& paras){
	getManagerOneway(activeAvatars.at(0)->jid->userId)->nodifyBuddiesOnlineStat(activeAvatars, selfOtherAvatars, buddies, type, paras);
}

void MucPresenceMsgHandlerAdapter::webPresence(int userId, const Ice::Context& ctx) {
	getManagerOneway(userId)->webPresence(userId, ctx);
}

void MucPresenceMsgHandlerAdapter::message(const JidPtr& host, const string& msg){
	getManager(host->userId)->message(host,msg);
}
void MucPresenceMsgHandlerAdapter::offline(const JidPtr& j){
	if(j){
		getManagerOneway(j->userId)->offline(j);
	}
}
void MucPresenceMsgHandlerAdapter::userStatusChanged(int userId, int userStatus){
	getManagerOneway(userId)->userStatusChanged(userId, userStatus);
}
