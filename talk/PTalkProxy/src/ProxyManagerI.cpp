#include "ProxyManagerI.h"
//#include "PresenceAdapter.h"
#include "OnlineCenterAdapter.h"
//#include "TalkLogicAdapter.h"
#include "LoginCacheAdapter.h"
#include "MessageType.h"
#include "XmppTools.h"
#include "LoginMsgHandlerAdapter.h"
#include "PresenceMsgHandlerAdapter.h"
#include "MessageMsgHandlerAdapter.h"
#include <boost/lexical_cast.hpp>
#include "MessageHolderAdapter.h"
#include "OnlineEventMediatorAdapter.h"
#include "PresenceAdapter.h"
#include "MsgFilterAdapter.h"
#include "pugixml.hpp"

using namespace talk;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace mop::hi::svc::adapter;
using namespace com::xiaonei::talk::util;
using namespace talk::online::adapter;
using namespace talk::adapter;
using namespace xce::mediator::onlineevent;
using namespace talk::online::adapter;
using namespace passport;
using namespace pugi;
using namespace com::xiaonei::antispam::wap::generation;

void MyUtil::initialize() {
	//MessageManagerI* manager = new MessageManagerI;
	ServiceI::instance().getAdapter()->add(&WTalkProxyManagerI::instance(), ServiceI::instance().createIdentity("M", ""));
	PipePool::instance().initialize(new PipeFactoryI);
	try{
		OnlineCenterAdapter::instance().closeProxyServer(ServiceI::instance().getName());
	}catch(Ice::Exception& e){
		MCE_WARN("initialize --> call close proxy err:"<<e<< "  file:"<<__FILE__<<"  line:"<<__LINE__);
	}catch(std::exception& e){
		MCE_WARN("initialize --> call close proxy err:"<<e.what()<< "  file:"<<__FILE__<<"  line:"<<__LINE__);
	}
}

//----------------------------------------------------

bool WebSession::empty() {
	//IceUtil::Mutex::Lock lock(_mutex);
	return _jidMap.empty();
}
;

bool WebSession::bind(const string& sessionKey, const string& ticket,
		const JidPtr& jid, const Ice::Current& current) {
	//IceUtil::Mutex::Lock lock(_mutex);
	_timestamp =:: time(NULL);

	map<string,time_t>::iterator it = _sessionKeyMap.find(sessionKey);

	if (it != _sessionKeyMap.end()) {
		if (it->second == 0 || (::time(NULL) - it->second) <= MAX_DURATION) {
			// the sessionkey ,found, is available
			if (!_sessionJid) {
				JidPtr j = new Jid();
				j -> userId = jid -> userId;
				j -> endpoint = ServiceI::instance().getName();

				j -> index = 2;
				_sessionJid = j;

				//bool success = false;
				try {
					LoginMsgHandlerAdapter::instance().bind(j, current.ctx);

				} catch(Ice::Exception& e) {
					MCE_WARN("WebSession::sessionBind --> Login::bind " << jidToString(jid) << ", "<<e);
					//_sessionJid = 0;
				}
				/*if (!success) {
					_sessionJid = 0;
					return false;
				}*/
				try {
					MCE_INFO("WebSession::bind --> OnlineCenter::presence 1, " << jidToString(jid));
					OnlineCenterAdapter::instance().presence(_sessionJid, ONLINE, "webpager", "", Str2StrMap());
				} catch(Ice::Exception& e) {
					MCE_WARN("WebSession::bind --> OnlineCenter::presence 1 err, " << jidToString(jid) << ", "<<e);
					return false;
				}
				try{
					PresenceAdapter::instance().bind(_sessionJid);
					OnlineEventMediatorAdapter::instance().sendOnlineEvent(_sessionJid->userId,1);
				}catch(Ice::Exception& e){
					MCE_WARN("WebSession::bind --> other " << jidToString(jid) << ", " <<e);
				}
			}

			/*if(_jidMap.empty() && _msgs.empty()) {
				try {
					MCE_DEBUG("WebSession::bind --> load offline message " << jidToString(jid));
					MessageHolderAdapter::instance().loadOfflineMessage(_sessionJid);
				} catch(Ice::Exception& e) {
					MCE_WARN("WebSession::bind --> " << jidToString(jid) << " load offline message err1:"<<e);
				}
			}*/
			//jid->endpoint = ServiceI::instance().getName();
			//jid->index = getIndex();
			_jidMap[jid->index] = jid;

			it->second = 0;

			return true;

		}
	}
	// need verifyTicket
	int uid = 0;
	try{
		if(ticket.length()==33){
			uid = WapLoginCacheAdapter::instance().ticket2Id(ticket);
		}else if(jidExist(jid)){
			uid = jid->userId;
		}
	}catch(Ice::Exception& e){
		MCE_WARN("WebSession::bind -> " << jidToString(jid) << "verify err, " << e);
	}
	MCE_INFO("WebSession::bind -> verify res " << ticket << " " << uid << " " << jid->userId);
	if ((uid != -1) &&(uid == jid->userId)) {
		if (!_sessionJid) {
			JidPtr j = new Jid();
			j -> userId = jid -> userId;
			j -> endpoint = ServiceI::instance().getName();

			j -> index = 2;
			_sessionJid = j;

			//bool success = false;
			try {
				MCE_DEBUG("WebSession::bind -> " << jidToString(jid) << " do TalkLogicAdapter::bind");
				LoginMsgHandlerAdapter::instance().bind(j, current.ctx);

			} catch(Ice::Exception& e) {
				MCE_WARN("WebSession::sessionBind --> " << jidToString(jid) << " bind err :"<<e);
				//_sessionJid = 0;
			}
			/*if (!success) {
				_sessionJid = 0;
				return false;
			}*/
			try {
				MCE_INFO("WebSession::bind --> OnlineCenter::presence 2, " << jidToString(jid));
				OnlineCenterAdapter::instance().presence(_sessionJid, ONLINE, "webpager", "", Str2StrMap());
			} catch(Ice::Exception& e) {
				MCE_WARN("WebSession::bind --> OnlineCenter::presence 2 err, " << jidToString(jid) << ", "<<e);
				return false;
			}
			try{
				PresenceAdapter::instance().bind(_sessionJid);
				OnlineEventMediatorAdapter::instance().sendOnlineEvent(_sessionJid->userId,1);
			}catch(Ice::Exception& e){
				MCE_WARN("WebSession::bind --> other " << jidToString(jid) << ", "<<e);
			}
		}

		/*if(_jidMap.empty() && _msgs.empty()) {
			try {
				MCE_DEBUG("WebSession::sessionBind --> " << jidToString(jid) << " load offline message");
				MessageHolderAdapter::instance().loadOfflineMessage(_sessionJid);
			} catch(Ice::Exception& e) {
				MCE_WARN("WebSession::sessionBind --> " << jidToString(jid) << " load offline message err:"<<e);
			}
		}*/

		//jid->endpoint = ServiceI::instance().getName();
		//		jid->index = getIndex();
		_jidMap[jid->index] = jid;

		_sessionKeyMap[sessionKey] = 0;

		//		JidPtr res = new Jid();
		//		res->userId = jid->userId;
		//		res->endpoint = ServiceI::instance().getName();
		//		res->index = jid->index;

		return true;
	} else {
		return false;
	}
}

void WebSession::unbind(const string& sessionKey, const JidPtr& jid) {
	//IceUtil::Mutex::Lock lock(_mutex);


	_timestamp =:: time(NULL);

	map<string,time_t>::iterator it = _sessionKeyMap.find(sessionKey);
	if (it != _sessionKeyMap.end()) {
		it->second = ::time(NULL);
	}
	MCE_DEBUG("WebSession::unbind --> find index ?"<< (_jidMap.find(jid->index) != _jidMap.end()) << "  index="<<jid->index);
	_jidMap.erase(jid->index);

	for (map<string,time_t>::iterator sit = _sessionKeyMap.begin(); sit
			!= _sessionKeyMap.end();) {
		if ((sit->second != 0) && ((_timestamp - sit->second)> MAX_DURATION)) {
			_sessionKeyMap.erase(sit++);
		} else {
			++sit;
		}
	}
	MCE_DEBUG("WebSession::unbind --> " << jidToString(jid) << " jid map size:"<<_jidMap.size());
	if (_jidMap.empty()) {
		//TaskManager::instance().schedule(this);
		WTalkProxyManagerI::instance().push(this);
	}

}
bool WebSession::jidExist(const JidPtr& jid){
	for (map<Ice::Long,JidPtr>::iterator it = _jidMap.begin(); it != _jidMap.end(); ++it) {
		if((it->first/100) == (jid->index/100)){
			return true;
		}
	}
	MCE_WARN("WebSession::jidExist --> not exist,  " << jidToString(jid));
	return false;
}
/*
void WebSession::handle() {
	MCE_DEBUG("WebSession::handle --> " << jidToString(_sessionJid));
	if (::time(NULL) - _timestamp >= MAX_DURATION) {
		WTalkProxyManagerI::instance().destroySession(this->_sessionJid);
	} else {
		if (_jidMap.empty()) {
			TaskManager::instance().schedule(this);
		}
	}
}
*/
JidSeq WebSession::getAccessJid() {
	JidSeq seq;
	for (map<Ice::Long,JidPtr>::iterator it = _jidMap.begin(); it
			!= _jidMap.end(); ++it) {
		seq.push_back(it->second);
	}
	return seq;
}

void WebSession::removeJid(const string& endpoint) {

	_timestamp =:: time(NULL);

	for (map<Ice::Long,JidPtr>::iterator it = _jidMap.begin(); it != _jidMap.end();) {
		if (it->second->endpoint == endpoint) {
			_jidMap.erase(it++);
		} else {
			++it;
		}
	}

	if (_jidMap.empty()) {
		//TaskManager::instance().schedule(this);
		WTalkProxyManagerI::instance().push(this);
	}

}

void WebSession::storeMsg(MessagePtr msg){
	++_msgid;
	_msgs[_msgid] = msg;
}
//MessageMap getMsg(Ice::Long msgId);
MessageMap WebSession::getMsg(Ice::Long msgId){
	//MCE_INFO("WebSession::getMsg --> " << msgId);
	MessageMap msgs;
	for (map<Ice::Long,MessagePtr>::iterator it = _msgs.begin(); it != _msgs.end(); ) {
		if (it->first > msgId) {
			msgs[it->first] = it->second;
			++it;
		}else{
			if(it->second->msgKey > 0) {
				try {
					MessageHolderAdapter::instance().send(it->second->to->userId,it->second->msgKey);
				} catch(Ice::Exception& e) {
					MCE_WARN("ClientBuffer::getMsg --> call MessageHolderAdapter clean offline msg");
				}
			}
			_msgs.erase(it++);
		}
	}
	return msgs;
}

void WebSession::clearMsg(Ice::Long msgId){
	/*for (map<Ice::Long,MessagePtr>::iterator it = _msgs.begin(); it != _msgs.end();) {
		if (it->first <= msgId) {
			try {
				if(it->second->msgKey > 0) {
					MessageHolderAdapter::instance().send(it->second->to->userId,it->second->msgKey);
				}
			} catch(Ice::Exception& e) {
				MCE_WARN("ClientBuffer::getMsg --> call MessageHolderAdapter clean offline msg");
			}
			_msgs.erase(it++);
			continue;
		}else{
			break;
		}
	}*/
}
//------------------------------------------------------

bool SessionPool::bind(const string& sessionKey, const string& ticket,
		const JidPtr& jid, const Ice::Current& current) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	Stat st(this, false);
	if (!jid) {
		return false;
	}
	MCE_INFO("SessionPool::bind --> " << jidToString(jid));
	map<int, WebSessionPtr>::iterator it = _pool.find(jid->userId);

	if (it == _pool.end()) {
		WebSessionPtr s = new WebSession();
		bool success = s->bind(sessionKey, ticket, jid, current);
		if (!success) {
			return false;
		}
		_pool[jid->userId] = s;
		return true;
	}
	return it->second->bind(sessionKey, ticket, jid, current);
}

void SessionPool::unbind(const string& sessionKey, const JidPtr& jid) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	Stat st(this, false);
	if(jid){
		MCE_INFO("SessionPool::unbind --> " << jidToString(jid));
	}
	map<int, WebSessionPtr>::iterator it = _pool.find(jid->userId);
	if (it != _pool.end()) {
		it->second->unbind(sessionKey, jid);
	} else {
		MCE_DEBUG("SessionPool::unbind --> not in pool jid:"<< jidToString(jid));
		JidSeq jseq = OnlineCenterAdapter::instance().getUserJids(jid->userId);
		for (size_t i = 0; i < jseq.size(); ++i) {
			if (jseq.at(i)->endpoint == ServiceI::instance().getName()) {
				try {
					MCE_DEBUG("SessionPool::unbind --> not in pool jid:"<< jidToString(jseq.at(i)));
					LoginMsgHandlerAdapter::instance().unbind(jseq.at(i));
					PresenceMsgHandlerAdapter::instance().offline(jseq.at(i));
				} catch(Ice::Exception & e) {
					MCE_WARN("do unbind err:"<<e);
				}
			}
		}
	}
}

void SessionPool::destroySession(const JidPtr& jid) {
	//no lock
	//IceUtil::Mutex::Lock lock(_mutex);
	MCE_DEBUG("SessionPool::destroySession --> jid:"<< jidToString(jid));
	map<int, WebSessionPtr>::iterator it = _pool.find(jid->userId);

	if (it != _pool.end()) {
		MCE_DEBUG("SessionPool::destroySession --> jid:"<< jidToString(jid)
				<< "  at 1");
		if (!it->second->empty()) {
			MCE_DEBUG("SessionPool::destroySession --> jid:"<< jidToString(jid)
					<< "  at 2");
			return;
		}
		if (jid -> index == it->second->getSessionJid()->index) {
			_pool.erase(it);

			try {
				MCE_INFO("SessionPool::destroySession --> do TalkLoginAdapter::unbind  jid:"<<jidToString(jid));
				LoginMsgHandlerAdapter::instance().unbind(jid);
				PresenceMsgHandlerAdapter::instance().offline(jid);
			} catch(Ice::Exception& e) {
				MCE_WARN("SessionPool::destroySession --> do TalkLogin::unbind jid:"<<jidToString(jid)<<" err:"<<e);
			}
		}
	} else {
		MCE_DEBUG("SessionPool::destroySession --> jid:"<< jidToString(jid)
				<< "  at 3");
	}
}

JidSeq SessionPool::getAccessJid(int userid) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	Stat st(this, true);
	map<int, WebSessionPtr>::iterator it = _pool.find(userid);
	if (it != _pool.end()) {
		return it->second->getAccessJid();
	}
	return JidSeq();
}

bool SessionPool::jidExist(const JidPtr& jid){
	IceUtil::RWRecMutex::RLock lock(_mutex);
	map<int, WebSessionPtr>::iterator it = _pool.find(jid->userId);
	if (it != _pool.end()) {
		return it->second->jidExist(jid);
	}
	return false;
}

void SessionPool::removeJid(const string& endpoint) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	Stat st(this, false);
	for (map<int, WebSessionPtr>::iterator it = _pool.begin(); it != _pool.end(); ++it) {
		it->second->removeJid(endpoint);
	}
}

pair<int, int> SessionPool::size(){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	pair<int, int> p = make_pair<int, int>(0, 0);
	for (map<int, WebSessionPtr>::iterator it = _pool.begin(); it != _pool.end(); ++it) {
		int size = it->second->size();
		if(size > 0){
			p.first++;
			p.second += size;
		}
	}
	return p;
}

void SessionPool::push(const WebSessionPtr& sess){
	//no lock
	_timeoutSessions.push_back(make_pair<WebSessionPtr, time_t>(sess, ::time(NULL)));
}

void SessionPool::handle(){
	while(true){
		IceUtil::RWRecMutex::WLock lock(_mutex);
		Stat st(this, false);
		if(_timeoutSessions.empty()){
			return;
		}
		pair<WebSessionPtr, time_t> p = _timeoutSessions.front();
		if(::time(NULL) - p.second < MAX_DURATION){
			return;
		}
		_timeoutSessions.pop_front();
		if(p.first->timeout()){
			WTalkProxyManagerI::instance().destroySession(p.first->getSessionJid());
		}
	}
}

void SessionPool::storeMsg(MessagePtr msg){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	map<int, WebSessionPtr>::iterator it = _pool.find(msg->to->userId);
	if (it != _pool.end()) {
		return it->second->storeMsg(msg);
	}
	MCE_WARN("SessionPool::storeMsg --> session not exist " << jidToString(msg->to));
}

MessageMap SessionPool::getMsg(int userId, Ice::Long msgId){
  IceUtil::RWRecMutex::WLock lock(_mutex);
	MCE_INFO("#SessionPool::getMsg --> " << userId << " " << msgId);
	map<int, WebSessionPtr>::iterator it = _pool.find(userId);
	//MCE_INFO("#SessionPool::getMsg 11 --> " << userId << " " << msgId);
	if (it != _pool.end()) {
		MCE_INFO("#SessionPool::getMsg 222 --> " << userId << " " << msgId);
		return it->second->getMsg(msgId);
	}
	//MCE_INFO("#SessionPool::getMsg 33 --> " << userId << " " << msgId);
	return MessageMap();
}

void SessionPool::clearMsg(int userId, Ice::Long msgId){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	map<int, WebSessionPtr>::iterator it = _pool.find(userId);
	if (it != _pool.end()) {
		return it->second->clearMsg(msgId);
	}
}
//------------------------------------------------------

void PostPipe::handle(const ObjectSeq& seq) {

	if (seq.empty()) {
		return;
	} else {
		MCE_DEBUG("PostPipe::handle --> objectseq size = " <<seq.size());
	}

	MessageSeq mseq;
	for (size_t i = 0; i < seq.size(); ++i) {
		mseq.push_back(MessagePtr::dynamicCast(seq.at(i)));
	}
	try {
		MCE_DEBUG("start to deliver --> toid:"<<mseq.at(0)->to->userId);
		WTalkProxyManagerI::instance().getAccessServerOneway(getName())->deliver(mseq);
		MCE_DEBUG("end  deliver --> toid:"<<mseq.at(0)->to->userId);
	} catch(Ice::Exception& e) {
		MCE_WARN("PostPipe::handle --> deliver err:"<<e<<"  to endpoint:"<< mseq.at(0)->to);
	}
}

//------------------------------------------------------

void WTalkProxyManagerI::express(const JidPtr& jid, const string& msg,
		const Ice::Current&) {

}

void WTalkProxyManagerI::deliver(const MessageSeq& msgs, const Ice::Current&) {
	MCE_DEBUG("WTalkProxyManagerI::deliver --> msgs.size:"<<msgs.size());
	for (size_t i = 0; i < msgs.size(); ++i) {
		if ((msgs.at(i)->type == MESSAGE) || (msgs.at(i)->type == OFFLINE_MESSAGE) || (msgs.at(i)->type == PHONE_NOTIFY)) {

			//MCE_DEBUG("msg:"<<msgs.at(i)->msg<<"  toid:"<<msgs.at(i)->to->userId);
			JidSeq jseq = _pools[msgs.at(i)->to->userId % POOLSIZE].getAccessJid(msgs.at(i)->to->userId);
			//MCE_DEBUG("jid size:"<<jseq.size()<<"  toid:"<<msgs.at(i)->to->userId);
			string body;
			xml_document doc;
			try{
				msgs.at(i)->msg = MsgFilterAdapter::instance().wapFilter(msgs.at(i)->msg);
			}catch(Ice::Exception& e){
				MCE_WARN("WTalkProxyManagerI::deliver --> invoke antispam err, " << e);
			}

			if(doc.load(msgs.at(i)->msg.c_str())){
				body = doc.child("message").child("body").first_child().value();
			}
			MCE_INFO("WTalkProxyManagerI::deliver --> from:" << jidToString(msgs.at(i)->from) << " to:" << jidToString(msgs.at(i)->to) << " msg:" << body);
			_pools[msgs.at(i)->to->userId % POOLSIZE].storeMsg(msgs.at(i));

			if (jseq.empty()) {
				//TalkLogicAdapter::instance().unbind(msgs.at(i)->to);
				MCE_WARN("WTalkProxyManagerI::deliver --> access jid size = 0");
				return;
			}

			for (size_t j = 0; j < jseq.size(); ++j) {
				MessagePtr msg = new Message();
				msg->from = msgs.at(i)->from;
				msg->to = jseq.at(j);
				msg->type = msgs.at(i)-> type;
				msg->msg = msgs.at(i)->msg;
				msg->msgKey = msgs.at(i)->msgKey;
				MCE_DEBUG("call pipepool   toid:"<<msg->to->userId);
				PipePool::instance().push(jseq.at(j)->endpoint, 0, msg);
			}
		}
	}
}

OnlineCountPtr WTalkProxyManagerI::getOnlineCount(const Ice::Current&){
	OnlineCountPtr count = new OnlineCount;
	count->user = 0;
	count->avatar = 0;
	for(int i=0; i<POOLSIZE; i++){
		pair<int, int> p = _pools[i].size();
		count->user += p.first;
		count->avatar += p.second;
	}
	return count;
}

bool WTalkProxyManagerI::bind(const string& sessionKey, const string& ticket,
		const JidPtr& j, const Ice::Current& current) {

	MCE_DEBUG("WTalkProxyManagerI::bind --> key:"<<sessionKey<<"  ticket:"<<ticket<<"  jid:"<<j->userId);
	if ((!j) || j->userId < 0) {
		return false;
	}
	return _pools[j->userId % POOLSIZE].bind(sessionKey, ticket, j, current);
}
void WTalkProxyManagerI::unbind(const string& sessionKey, const JidPtr& j,
		const Ice::Current&) {
	_pools[j->userId % POOLSIZE].unbind(sessionKey, j);
}

JidSeq WTalkProxyManagerI::getAccessJidSeq(int userid, const Ice::Current&){
	return _pools[userid % POOLSIZE].getAccessJid(userid);
}

void WTalkProxyManagerI::destroySession(const JidPtr& jid) {
	_pools[jid->userId % POOLSIZE].destroySession(jid);
}

void WTalkProxyManagerI::push(const WebSessionPtr& sess){
	_pools[sess->getSessionJid()->userId % POOLSIZE].push(sess);
}

MessageMap WTalkProxyManagerI::getMsg(int userId, Ice::Long msgId, const Ice::Current&){
	MCE_INFO("WTalkProxyManagerI::getMsg --> " << userId << " " << msgId);
	return _pools[userId % POOLSIZE].getMsg(userId, msgId);
}

void WTalkProxyManagerI::clearMsg(int userId, Ice::Long msgId, const Ice::Current&){
	_pools[userId % POOLSIZE].clearMsg(userId, msgId);
}

ProxyManagerPrx WTalkProxyManagerI::getAccessServerOneway(const string& endpoint) {

	map<string, ProxyManagerPrx>::iterator it;

	{
		IceUtil::Mutex::Lock lock(_mutex);
		it = _prxs.find(endpoint);
	}
	if (it != _prxs.end()) {
		return it->second;
	} else {
		Ice::ObjectPrx oprx = ServiceI::instance().getAdapter()->getCommunicator()->stringToProxy("M@"+endpoint);
		ProxyManagerPrx prx =
				ProxyManagerPrx::uncheckedCast(oprx->ice_oneway());
		{
			IceUtil::Mutex::Lock lock(_mutex);
			_prxs[endpoint] = prx;
		}
		return prx;
	}
}

void WTalkProxyManagerI::closeAccessServer(const string& endpoint,
		const Ice::Current&) {
	for (int i = 0; i < POOLSIZE; ++i) {
		_pools[i].removeJid(endpoint);
	}
}

bool WTalkProxyManagerI::upDeliver(const JidPtr& jid, const string& msg, const Ice::Current&){
	if(!jid){
		return false;
	}
	if(_pools[jid->userId % POOLSIZE].jidExist(jid)){
		jid->index = 0;
		jid->endpoint = ServiceI::instance().getName();
		MCE_INFO("WTalkProxyManagerI::upDeliver --> " << jid->userId << " " << jid->endpoint << " " << jid->index);
    MessageMsgHandlerAdapter::instance().message(jid, msg);
		return true;
	}
	return false;
}

string WTalkProxyManagerI::getStatInfo(){

	int rLock = 0;
	int wLock = 0;
	int rLockCountMax = 0;
	int wLockCountMax = 0;

	float rLockTimeMax = 0;
	float wLockTimeMax = 0;
	float rLockTimeAvg = 0;
	float wLockTimeAvg = 0;
	int locked = 0;
	for(int i=0; i<POOLSIZE; i++){
		rLock += _pools[i]._rLockCount;
		wLock += _pools[i]._wLockCount;

		if(_pools[i]._rLockCount > rLockCountMax){
			rLockCountMax = _pools[i]._rLockCount;
		}
		if(_pools[i]._wLockCount > wLockCountMax){
			wLockCountMax = _pools[i]._wLockCount;
		}
		if(_pools[i]._rLockTimeMax > rLockTimeMax){
			rLockTimeMax = _pools[i]._rLockTimeMax;
		}
		if(_pools[i]._wLockTimeMax > wLockTimeMax){
			wLockTimeMax = _pools[i]._wLockTimeMax;
		}
		rLockTimeAvg += _pools[i]._rLockTimeSum/_pools[i]._rLockCount;
		wLockTimeAvg += _pools[i]._wLockTimeSum/_pools[i]._wLockCount;
		if(_pools[i]._enter){
			locked++;
		}
		_pools[i].clearStatData();
	}
	rLockTimeAvg /= POOLSIZE;
	wLockTimeAvg /= POOLSIZE;

	ostringstream os;
	os << "~~ lock stat r=" << rLock << ", w=" << wLock
		<< ", rmax=" << rLockCountMax << ", wmax=" << wLockCountMax
		<< ", rtimemax=" << rLockTimeMax << ", wtimemax=" << wLockTimeMax
		<< ", rtimeavg=" << rLockTimeAvg << ", wtimeavg=" << wLockTimeAvg << ", locked=" << locked;
	return os.str();
}
//------------------------------------------------------

void StatTimer::handle(){
	string info = WTalkProxyManagerI::instance().getStatInfo();
	MCE_INFO("StatTimer::handle --> " << info);
}

