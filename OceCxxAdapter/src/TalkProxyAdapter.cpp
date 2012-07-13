#include "TalkProxyAdapter.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::adapter;
using namespace com::xiaonei::talk::common;


ProxyManagerPrx TalkProxyAdapter::getManager(const string & endpoint) {
	IceUtil::Mutex::Lock lock(*this);

	map<string,ProxyManagerPrx>::iterator it = _managers.find(endpoint);
	if (it != _managers.end()) {
		return it->second;
	}
	Ice::ObjectPrx prx = _channel->getCommunicator()->stringToProxy("M@"+endpoint);
	ProxyManagerPrx p = ProxyManagerPrx::uncheckedCast(prx);
	_managers[endpoint] = p;
	return p;
}
ProxyManagerPrx TalkProxyAdapter::getManagerOneway(const string & endpoint) {
	IceUtil::Mutex::Lock lock(*this);

	map<string,ProxyManagerPrx>::iterator it = _managersOneway.find(endpoint);
	if(it != _managersOneway.end()) {
		return it->second;
	}
	Ice::ObjectPrx prx = _channel->getCommunicator()->stringToProxy("M@"+endpoint);
	ProxyManagerPrx p = ProxyManagerPrx::uncheckedCast(prx->ice_oneway());
	_managersOneway[endpoint] = p;
	return p;
}
/*ProxyManagerPrx TalkProxyAdapter::getManagerBatchOneway(const string & endpoint) {
	IceUtil::Mutex::Lock lock(*this);

	map<string,ProxyManagerPrx>::iterator it = _managersBatchOneway.find(endpoint);
	if(it != _managersBatchOneway.end()) {
		return it->second;
	}
	Ice::ObjectPrx prx = _channel->getCommunicator()->stringToProxy("M@"+endpoint);
	ProxyManagerPrx p = ProxyManagerPrx::uncheckedCast(prx->ice_batchOneway());
	_managersBatchOneway[endpoint] = p;
	return p;
}*/
//--------------------------------------------------

void TalkProxyAdapter::express(const JidPtr& jid, const string& msg) {
	getManagerOneway(jid->endpoint)->express(jid,msg);
}
void TalkProxyAdapter::deliver(const MessagePtr& msg){
	getManagerOneway(msg->to->endpoint)->express(msg->to,msg->msg);
}
void TalkProxyAdapter::deliver(const MessageSeq& msgs) {
	if(msgs.empty()){
		return;
	}
	getManagerOneway(msgs.at(0)->to->endpoint)->deliver(msgs);
}
bool TalkProxyAdapter::verify(const JidPtr& jid, const string& ticket) {
	return getManager(jid->endpoint)->verify(jid,ticket);
}

string TalkProxyAdapter::GetTicket(const JidPtr& jid){
	return getManager(jid->endpoint)->GetTicket(jid);
}

void TalkProxyAdapter::leave(const JidPtr& jid) {
	getManagerOneway(jid->endpoint)->leave(jid);
}
void TalkProxyAdapter::relogin() {

}
void TalkProxyAdapter::broadcast(const string& endpoint,const string& xmlMsg) {
	getManagerOneway(endpoint)->broadcast(xmlMsg);
}
void TalkProxyAdapter::kick(const JidPtr& jid){
	if(jid){
		getManagerOneway(jid->endpoint)->kick(jid);
	}
}
