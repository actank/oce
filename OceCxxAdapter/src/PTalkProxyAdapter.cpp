#include "PTalkProxyAdapter.h"

using namespace talk::adapter;
using namespace talk;
using namespace MyUtil;

WTalkProxyManagerPrx PTalkProxyAdapter::getManager(int id) {
	return locate<WTalkProxyManagerPrx>(_managers, "M", id, TWO_WAY);
}

WTalkProxyManagerPrx PTalkProxyAdapter::getManagerOneway(int id) {
	return locate<WTalkProxyManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

/*void WTalkProxyAdapter::deliver(const MessageSeq& msgs) {
	if (msgs.empty()) {
		return;
	}
	getManagerOneway(msgs.at(0)->to->userId)->deliver(msgs);
}*/
bool PTalkProxyAdapter::bind(const string& sessionKey, const string& ticket,
		const JidPtr& j) {
	if(!j){
	return false;
	}
	return getManager(j->userId)->bind(sessionKey, ticket, j);

}

void PTalkProxyAdapter::unbind(const string& sessionKey, const JidPtr& j) {
	getManagerOneway(j->userId)->unbind(sessionKey, j);
}

void PTalkProxyAdapter::closeAccessServer(const string& endpoint) {
	for (size_t i = 0; i < _cluster; ++i) {
		try {
			getManagerOneway(i)->closeAccessServer(endpoint);
		} catch(...) {
		}
	}
}

bool PTalkProxyAdapter::upDeliver(const JidPtr& jid, const string& msg){
	if(jid){
		return getManager(jid->userId)->upDeliver(jid, msg);
	}
	return false;
}

OnlineCountPtr PTalkProxyAdapter::getOnlineCount(){
	OnlineCountPtr count = new OnlineCount;
	count->user = 0;
	count->avatar = 0;
	for(int i=0; i<cluster(); i++){
		OnlineCountPtr tmp = getManager(i)->getOnlineCount();
		count->user += tmp->user;
		count->avatar += tmp->avatar;
	}
	return count;
}

MessageMap PTalkProxyAdapter::getMsg(int userId, Ice::Long msgId){
	return getManager(userId)->getMsg(userId, msgId);
}
