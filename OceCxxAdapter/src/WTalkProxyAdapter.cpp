#include "WTalkProxyAdapter.h"

using namespace talk::adapter;
using namespace talk;
using namespace MyUtil;

WTalkProxyManagerPrx WTalkProxyAdapter::getManager(int id) {
	return locate<WTalkProxyManagerPrx>(_managers, "M", id, TWO_WAY);
}

WTalkProxyManagerPrx WTalkProxyAdapter::getManagerOneway(int id) {
	return locate<WTalkProxyManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

/*void WTalkProxyAdapter::deliver(const MessageSeq& msgs) {
	if (msgs.empty()) {
		return;
	}
	getManagerOneway(msgs.at(0)->to->userId)->deliver(msgs);
}*/
bool WTalkProxyAdapter::bind(const string& sessionKey, const string& ticket,
		const JidPtr& j) {
	if(!j){
	return false;
	}
	return getManager(j->userId)->bind(sessionKey, ticket, j);

}
void WTalkProxyAdapter::unbind(const string& sessionKey, const JidPtr& j) {
	getManagerOneway(j->userId)->unbind(sessionKey, j);
}

bool WTalkProxyAdapter::upDeliver(const JidPtr& jid, const string& msg){
	if(jid){
		return getManager(jid->userId)->upDeliver(jid, msg);
	}
	return false;
}

OnlineCountPtr WTalkProxyAdapter::getOnlineCount(){
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

void WTalkProxyAdapter::closeAccessServer(const string& endpoint) {
	for (size_t i = 0; i < _cluster; ++i) {
		try {
			getManagerOneway(i)->closeAccessServer(endpoint);
		} catch(...) {
		}
	}
}
