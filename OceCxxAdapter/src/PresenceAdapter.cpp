#include "PresenceAdapter.h"

using namespace mop::hi::svc::adapter;

void PresenceAdapter::initialize() {
	_manager = locate<PresenceManagerPrx>("M", TWO_WAY);
	_managerOneway = locate<PresenceManagerPrx>("M", ONE_WAY);
}

void PresenceAdapter::bind(const JidPtr& jid) {
	return _managerOneway->bind(jid);
}
void PresenceAdapter::presence(const JidPtr& jid, const string& show,
		const string& ver, const string& ext) {
	_managerOneway->presence(jid, show, ver, ext);
}

void PresenceAdapter::unbind(const JidPtr& jid) {
	_managerOneway->unbind(jid);
}

JidSeqMap PresenceAdapter::getIMOnlineUserJidSeqMap(const MyUtil::IntSeq& ids) {
	return _manager->getIMOnlineUserJidSeqMap(ids);
}
JidSeq PresenceAdapter::getOnlineUserJidSeq(int id) {
	return _manager->getOnlineUserJidSeq(id);
}
map<int,int> PresenceAdapter::getOnlineStat(const MyUtil::IntSeq& ids) {
	JidSeqMap jmap = _manager->getIMOnlineUserJidSeqMap(ids);
	map<int,int> statMap;
	for (JidSeqMap::iterator it = jmap.begin(); it != jmap.end(); ++it) {
		int stat = 0;
		for (size_t i = 0; i < it->second.size(); ++i) {
			if (it->second.at(i)->endpoint != "") {
				if (it->second.at(i)->endpoint[0] == 'W') {
					stat |= WEBPAGER_ONLINE;
				}else if(it->second.at(i)->endpoint[0] == 'T'){
					stat |= TALK_ONLINE;
				}
			}
			if(stat > 2){
				break;
			}
		}
		statMap[it->first] = stat;
	}
	return statMap;
}
//void
//PresenceAdapter::removeOnlineUserByProxyServer(const string& endpoint)
//{
//	getOnewayManager()->removeOnlineUserByProxyServer(endpoint);
//}

OnlineInfoMap PresenceAdapter::filterOnlineUsers(const MyUtil::IntSeq& ids) {
	return _manager->filterOnlineUsers(ids);
}
OnlineInfoPtr PresenceAdapter::filterOnlineUser(const JidPtr& jid){
	return _manager->filterOnlineUser(jid);
}

OnlineInfoSeq PresenceAdapter::getOnlineUsersInfo(const JidSeq& jids){
	return _manager->getOnlineUsersInfo(jids);
}
bool PresenceAdapter::has(const JidPtr& jid) {
	return _manager->has(jid);
}

void PresenceAdapter::closeProxyServer(const string& endpoint) {
	_managerOneway->closeProxyServer(endpoint);
}
