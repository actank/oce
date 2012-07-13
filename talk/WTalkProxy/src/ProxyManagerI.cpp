#include "ProxyManagerI.h"
//#include "PresenceAdapter.h"
#include "OnlineCenterAdapter.h"
//#include "TalkLogicAdapter.h"
#include "TicketAdapter.h"
#include "MessageType.h"
#include "XmppTools.h"
#include "LoginMsgHandlerAdapter.h"
#include "PresenceMsgHandlerAdapter.h"
#include "MessageMsgHandlerAdapter.h"
#include <boost/lexical_cast.hpp>
#include "MessageHolderAdapter.h"
#include "OnlineEventMediatorAdapter.h"
#include "PresenceAdapter.h"
#include "QueryRunner.h"
#include "MucGateAdapter.h"
#include "NotifyDispatcherAdapter.h"
using namespace xce::notify;
using namespace com::xiaonei::talk::muc;
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
using namespace com::xiaonei::xce;

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
		const JidPtr& jid) {
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

				j -> index = 0;
				_sessionJid = j;

				//bool success = false;
				try {
					LoginMsgHandlerAdapter::instance().bind(j);

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
				}
				try{
					MucGateAdapter::instance().UserOnline(_sessionJid);
				}catch(Ice::Exception& e){
					MCE_WARN("WebSession::bind --> invoke MucGate.UserOnline" << jidToString(jid) << " error, " << e);
				}


				try{
					PresenceAdapter::instance().bind(_sessionJid);
					OnlineEventMediatorAdapter::instance().sendOnlineEvent(_sessionJid->userId,1);
				}catch(Ice::Exception& e){
					MCE_WARN("WebSession::bind --> other " << jidToString(jid) << ", " <<e);
				}
			}

      
			if(_jidMap.empty()) {
				try {
					MCE_DEBUG("WebSession::bind --> load offline UnreadNotify" << jidToString(jid));
					MessageHolderAdapter::instance().LoadUnreadNotifyInfoCount(_sessionJid);
					//loadOfflineNotify(jid);
				} catch(Ice::Exception& e) {
					MCE_WARN("WebSession::bind --> " << jidToString(jid) << " load Unread Notify err1:"<<e);
				}
			}
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
		uid = TicketAdapter::instance().verifyTicket(ticket, "");
	}catch(Ice::Exception& e){
		MCE_WARN("WebSession::bind -> " << jidToString(jid) << "verify err, " << e);
	}
	MCE_INFO("WebSession::bind -> verify res " << uid << " " << jid->userId);
	if ((uid != -1) &&(uid == jid->userId)) {
		if (!_sessionJid) {
			JidPtr j = new Jid();
			j -> userId = jid -> userId;
			j -> endpoint = ServiceI::instance().getName();

			j -> index = 0;
			_sessionJid = j;

			//bool success = false;
			try {
				MCE_DEBUG("WebSession::bind -> " << jidToString(jid) << " do TalkLogicAdapter::bind");
				LoginMsgHandlerAdapter::instance().bind(j);

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
			}
			try{
				MucGateAdapter::instance().UserOnline(_sessionJid);
			}catch(Ice::Exception& e){
				MCE_WARN("WebSession::bind --> invoke MucGate.UserOnline" << jidToString(jid) << " error, " << e);
			}

			try{
				PresenceAdapter::instance().bind(_sessionJid);
				OnlineEventMediatorAdapter::instance().sendOnlineEvent(_sessionJid->userId,1);
			}catch(Ice::Exception& e){
				MCE_WARN("WebSession::bind --> other " << jidToString(jid) << ", "<<e);
			}
		}

		if(_jidMap.empty()) {
			try {
				MCE_DEBUG("WebSession::sessionBind --> " << jidToString(jid) << " load Unread Notify");
				MessageHolderAdapter::instance().LoadUnreadNotifyInfoCount(_sessionJid);
				//loadOfflineNotify(jid);
			} catch(Ice::Exception& e) {
				MCE_WARN("WebSession::sessionBind --> " << jidToString(jid) << " load Unread Notify err:"<<e);
			}
		}

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

void WebSession::loadOfflineNotify(const JidPtr& jid){
	MCE_INFO("WebSession::loadOfflineNotify --> " << jid->userId);
	Statement sql;
	sql << "select * from offline_notify where userid=" << _sessionJid->userId << " limit 1";
	mysqlpp::StoreQueryResult res = QueryRunner("im", CDbRServer).store(sql);
	if (!res) {
		return;
	}
	//MessageSeq mSeq;
	for (size_t i = 0; i<res.num_rows(); ++i) {
		mysqlpp::Row row = res.at(i);
		MessagePtr m = new Message;
		m->from = new Jid();
		m->from->userId = -1;
		m->from->index = -1;
		m->msg = row["xml"].c_str();
		m->type = NOTIFY_MESSAGE;
		map<Ice::Long,JidPtr>::iterator it = _jidMap.begin();
		m->to = jid;
		PipePool::instance().push(jid->endpoint, 0, m);
	}
	//MCE_INFO("WebSession::loadOfflineNotify --> deliver " << mSeq.size() << " jid:" << jidToString(jid));
	sql.clear();
	sql << "delete from offline_notify where userid=" << _sessionJid->userId;
	QueryRunner("im", CDbWServer).store(sql);
	
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
	for (map<Ice::Long,JidPtr>::iterator it = _jidMap.begin(); it
				!= _jidMap.end(); ++it) {
		if((it->first/100) == (jid->index/100)){
			return true;
		}
	}
	MCE_WARN("WebSession::jidExist --> not exist, " << jidToString(jid));
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
//------------------------------------------------------

bool SessionPool::bind(const string& sessionKey, const string& ticket,
		const JidPtr& jid) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	Stat st(this, false);
	if (!jid) {
		return false;
	}

	//---------------人人爱听活动----------------
	if(WTalkProxyManagerI::instance()._notifyactive){
		int userid = jid->userId;
		set<int>::iterator sit = _sendedids.find(userid);
		if(sit == _sendedids.end()){
			_sendedids.insert(userid);
			TaskManager::instance().execute(new SendNotifyToUserTask(userid));
		}else{
			MCE_INFO("SessionPool::bind --> user:" << userid << " has sended, do not send to him _sendedids.size:" << _sendedids.size());
		}
	}
	//-------------------------------------------

	MCE_INFO("SessionPool::bind --> " << jidToString(jid));
	map<int, WebSessionPtr>::iterator it = _pool.find(jid->userId);

	if (it == _pool.end()) {
		WebSessionPtr s = new WebSession();
		bool success = s->bind(sessionKey, ticket, jid);
		if (!success) {
			return false;
		}
		_pool[jid->userId] = s;
		return true;
	}
	return it->second->bind(sessionKey, ticket, jid);
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
				MCE_DEBUG("SessionPool::destroySession --> do TalkLoginAdapter::unbind  jid:"<<jidToString(jid));
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




void SessionPool::StopNotify(){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	_sendedids.clear();
}

void SessionPool::NotifyOnlineUsers(){
	IntSeq ids;
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		for(map<int, WebSessionPtr>::iterator it = _pool.begin(); it != _pool.end(); ++it) {
			int userid = it->first;
			set<int>::iterator sit = _sendedids.find(userid);
			if(sit == _sendedids.end()){
				_sendedids.insert(userid);
				ids.push_back(userid);
			}
		}
	}
	
	int count = 1;
	IntSeq tmp;
	for(IntSeq::iterator it = ids.begin(); ; ++it){
		if(it != ids.end()){
			tmp.push_back(*it);
		}
		if((count == 30 || it == ids.end()) && (!tmp.empty())){
			count = 0;
			TaskManager::instance().execute(new SendNotifyToUsersTask(tmp));
			tmp.clear();
		}
		if(it == ids.end()){
			break;
		}
		++count;
	}
	if(ids.size())
		MCE_INFO("SessionPool::NotifyOnlineUsers --> ids.size:" << ids.size());
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
		if ((msgs.at(i)->type == MESSAGE) || (msgs.at(i)->type == OFFLINE_MESSAGE) ||(msgs.at(i)->type == NOTIFY2_MESSAGE) || (msgs.at(i)->type == NOTIFY_MESSAGE) || (msgs.at(i)->type == FEED_STATUS)|| (msgs.at(i)->type == MUC_MESSAGE) || (msgs.at(i)->type == MUC_PRESENCE)) {

			MCE_DEBUG("msg:"<<msgs.at(i)->msg<<"  toid:"<<msgs.at(i)->to->userId);
			JidSeq jseq = _pools[msgs.at(i)->to->userId % POOLSIZE].getAccessJid(msgs.at(i)->to->userId);
			MCE_DEBUG("jid size:"<<jseq.size()<<"  toid:"<<msgs.at(i)->to->userId);

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

bool WTalkProxyManagerI::upDeliver(const JidPtr& jid, const string& msg, const Ice::Current&){
	if(!jid || jid->userId<=0){
		return false;
	}
	if(_pools[jid->userId % POOLSIZE].jidExist(jid)){
		jid->index = 0;
    jid->endpoint = ServiceI::instance().getName();
		MessageMsgHandlerAdapter::instance().message(jid, msg);
		return true;
	}
	MCE_INFO(jid->userId << "WTalkProxyManagerI updliver error. msg : " << msg);
	return false;
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
		const JidPtr& j, const Ice::Current&) {

	MCE_DEBUG("WTalkProxyManagerI::bind --> key:"<<sessionKey<<"  ticket:"<<ticket<<"  jid:"<<j->userId);
	if (j && j->userId > 0) {
		return _pools[j->userId % POOLSIZE].bind(sessionKey, ticket, j);
	}
	return false;
}
void WTalkProxyManagerI::unbind(const string& sessionKey, const JidPtr& j,
		const Ice::Current&) {
	if(j && j->userId > 0){
		_pools[j->userId % POOLSIZE].unbind(sessionKey, j);
	}
}

JidSeq WTalkProxyManagerI::getAccessJidSeq(int userid, const Ice::Current&){
	if(userid > 0){
		return _pools[userid % POOLSIZE].getAccessJid(userid);
	}
	return JidSeq();
}

void WTalkProxyManagerI::destroySession(const JidPtr& jid) {
	if(jid && jid->userId > 0){
		_pools[jid->userId % POOLSIZE].destroySession(jid);
	}
}

void WTalkProxyManagerI::push(const WebSessionPtr& sess){
	_pools[sess->getSessionJid()->userId % POOLSIZE].push(sess);
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

MessageMap WTalkProxyManagerI::getMsg(int userId, Ice::Long msgId, const Ice::Current&){
        MCE_INFO("WTalkProxyManagerI::getMsg --> " << userId << " " << msgId);
        return MessageMap();
	//return _pools[userId % POOLSIZE].getMsg(userId, msgId);
}

void WTalkProxyManagerI::closeAccessServer(const string& endpoint,
		const Ice::Current&) {
	for (size_t i = 0; i < POOLSIZE; ++i) {
		_pools[i].removeJid(endpoint);
	}
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



//-----------------------人人爱听活动推送NOTIFY--------------
bool WTalkProxyManagerI::StartNotify(int schema_id, int duration, const Ice::Current&){
	if(_notifyactive){
		MCE_WARN("WTalkProxyManagerI::StartNotify --> _schema_id:" << _schema_id << " is active now.");
		return false;
	}
	_notifyactive = true;
	_schema_id = schema_id;
	TaskManager::instance().schedule(new StopTimer(duration * 60 * 1000));
	TaskManager::instance().execute(new NotifyOnlineUsersTask());
	MCE_INFO("WTalkProxyManagerI::StartNotify --> start schema_id:" << _schema_id << " duration:" << duration);
	return true;
}
void WTalkProxyManagerI::StopNotify(const Ice::Current&){
	MCE_INFO("WTalkProxyManagerI::StopNotify --> stop schema_id:" << _schema_id);
	_notifyactive = false;
	_schema_id = 0;
	for(int i = 0; i < POOLSIZE; ++i){
		_pools[i].StopNotify();
	}
	//clear pool's sended cache;
}
void WTalkProxyManagerI::NotifyOnlineUsers(){
	MCE_INFO("WTalkProxyManagerI::NotifyOnlineUsers --> notify online users");
	for(int i = 0; i < POOLSIZE; ++i){
		_pools[i].NotifyOnlineUsers();
	}
}
//----------------------------------------------------------------------------------------------
void SendNotifyToUserTask::handle(){
	int schema_id = WTalkProxyManagerI::instance()._schema_id;
	if(!schema_id){
		return;
	}
	map<string, string> para;
	para["schema_id"] = boost::lexical_cast<string>(schema_id);//
	para["source"] = "0";
	para["owner"] = "0";
	para["from"] = "0";
	para["to"] = boost::lexical_cast<string>(_userid);
	para["time"] = boost::lexical_cast<string>(time(NULL));
	para["type"] = boost::lexical_cast<string>(schema_id);//

	bool isdeliver = WTalkProxyManagerI::instance()._isdeliver;
	try{
		if(isdeliver){
			NotifyDispatcherAdapter::instance().dispatch(para);
		}
	}catch(Ice::Exception& e){
		MCE_WARN("SendNotifyToUserTask::handle -->NotifyDispatcherAdapter::dispatch--> error << e:" << e);
	}
	MCE_INFO("SendNotifyToUserTask::handle --> send notify to userid:" << _userid << " schema_id:" << schema_id << " isdeliver:" << isdeliver);
}

void SendNotifyToUsersTask::handle(){
	int schema_id = WTalkProxyManagerI::instance()._schema_id;
	if(!schema_id){
		return;
	}
	
	ostringstream os;
	for(IntSeq::iterator idit = _ids.begin(); idit != _ids.end(); ++idit){
		if(idit != _ids.begin()){
			os << ",";
		}
		os << (*idit);
	}

	map<string, string> para;
	para["schema_id"] = boost::lexical_cast<string>(schema_id);//
	para["source"] = "0";
	para["owner"] = "0";
	para["from"] = "0";
	para["to"] = boost::lexical_cast<string>(os.str());
	para["time"] = boost::lexical_cast<string>(time(NULL));
	para["type"] = boost::lexical_cast<string>(schema_id);//


	bool isdeliver = WTalkProxyManagerI::instance()._isdeliver;
	try{
		if(isdeliver){
			NotifyDispatcherAdapter::instance().dispatch(para);
		}
	}catch(Ice::Exception& e){
		MCE_WARN("SendNotifyToUsersTask::handle -->NotifyDispatcherAdapter::dispatch--> error << e:" << e);
	}
	MCE_INFO("SendNotifyToUsersTask::handle --> send notify to userids:" << os.str() << " schema_id:" << schema_id << " isdeliver: " << isdeliver);
}

void StopTimer::handle(){
	WTalkProxyManagerI::instance().StopNotify();
}

void NotifyOnlineUsersTask::handle(){
	WTalkProxyManagerI::instance().NotifyOnlineUsers();
}

//-----------------------------------------------------------
