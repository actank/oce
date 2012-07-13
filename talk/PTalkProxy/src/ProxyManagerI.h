#ifndef PROXYMANAGERI_H_
#define PROXYMANAGERI_H_

#include "WTalkProxy.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/RWRecMutex.h>
#include "ServiceI.h"
#include "Pipe.h"
#include "TalkProxy.h"

namespace talk {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace com::xiaonei::talk;

const int MAX_DURATION = 10;

class WebSession : public IceUtil::Shared {

	JidPtr _sessionJid;
	map<Ice::Long,JidPtr> _jidMap;
	map<string,time_t> _sessionKeyMap; //if the session is close, set the value as the end timestamp
	int _counter;
	time_t _timestamp;
	Ice::Long 	_msgid;
	map<Ice::Long,MessagePtr> _msgs;
	//IceUtil::Mutex _mutex;
	//int getIndex();
public:
	WebSession() : _sessionJid(0), _counter(0),
		_timestamp(::time(NULL)), _msgid(0) {
	}

	bool empty();
	bool bind(const string& sessionKey, const string& ticket,
			const JidPtr& jid, const Ice::Current& current);
	void unbind(const string& sessionKey, const JidPtr& jid);
	//virtual void handle();
	bool jidExist(const JidPtr& jid);
	JidSeq getAccessJid();
	void removeJid(const string& endpoint);
	JidPtr getSessionJid(){
		return _sessionJid;
	};
	bool timeout(){
		return (::time(NULL) - _timestamp >= MAX_DURATION);
	}
	int size(){
		return _jidMap.size();
	}
	void storeMsg(MessagePtr msg);
	void clearMsg(Ice::Long msgId);
	MessageMap getMsg(Ice::Long msgId);
};

typedef IceUtil::Handle<WebSession> WebSessionPtr;

class SessionPool : public Timer {
	IceUtil::RWRecMutex _mutex;
	map<int, WebSessionPtr> _pool;
	list<pair<WebSessionPtr, time_t> > _timeoutSessions;


	//just for stat
	int _rLockCount;
	int _wLockCount;
	float _rLockTimeMax;
	float _wLockTimeMax;
	float _rLockTimeSum;
	float _wLockTimeSum;
	bool _enter;
	//int _leave;
	//

	class Stat{
	public:
		Stat(SessionPool* pool, bool rLock) : _pool(pool), _rLock(rLock){
			_pool->_enter = true;
			gettimeofday(&_start, NULL);
		}
		~Stat() {
			timeval end;
			gettimeofday(&end, NULL);
			float timeuse=1000000*(end.tv_sec-_start.tv_sec)+end.tv_usec
					-_start.tv_usec;
			timeuse/=1000;
			if(_rLock){
				_pool->_rLockCount++;
				_pool->_rLockTimeSum += timeuse;
				if(timeuse>_pool->_rLockTimeMax){
					_pool->_rLockTimeMax = timeuse;
				}
			}else{
				_pool->_wLockCount++;
				_pool->_wLockTimeSum += timeuse;
				if(timeuse>_pool->_wLockTimeMax){
					_pool->_wLockTimeMax = timeuse;
				}
			}
			_pool->_enter = false;
		}
	private:
		timeval 		_start;
		SessionPool* 	_pool;
		bool			_rLock;
	};
public:
	friend class TimeStat;
	friend class WTalkProxyManagerI;
	SessionPool() : Timer(1000) {
		TaskManager::instance().scheduleRepeated(this);
	}
	bool bind(const string& sessionKey, const string& ticket,
			const JidPtr& jid, const Ice::Current& current);
	void unbind(const string& sessionKey, const JidPtr& jid);
	void destroySession(const JidPtr& jid);
	JidSeq getAccessJid(int userid);
	void removeJid(const string& endpoint);
	bool jidExist(const JidPtr& jid);
	void handle();
	void push(const WebSessionPtr& sess);
	pair<int, int> size();
	void storeMsg(MessagePtr msg);
	void clearMsg(int userId, Ice::Long msgId);
	MessageMap getMsg(int userId, Ice::Long msgId);
	//string getStatInfo();
	void clearStatData(){
		_rLockCount = 0;
		_wLockCount = 0;
		_rLockTimeMax = 0;
		_wLockTimeMax = 0;
		_rLockTimeSum = 0;
		_wLockTimeSum = 0;
		_enter = false;
	}
};

//-------------------------------------

class PostPipe : public Pipe {
public:
	PostPipe(const string& name, int index) :
		Pipe(name, index) {
	}
	;
	virtual void handle(const ObjectSeq& seq);
};

class PipeFactoryI : public PipeFactory {
public:
	virtual PipePtr create(const string& type, int index) {
		return new PostPipe(type, index);
	}
};

//-------------------------------------
class StatTimer : public Timer{
public:
	StatTimer() : Timer(1000*10){
		TaskManager::instance().scheduleRepeated(this);
	}
	void handle();
};

//-------------------------------------
class WTalkProxyManagerI : public WTalkProxyManager,
	public Singleton<WTalkProxyManagerI> {
	StatTimer _statTimer;
	const static int POOLSIZE = 10000;
	SessionPool _pools[POOLSIZE];
	map<string, ProxyManagerPrx> _prxs;
	IceUtil::Mutex _mutex;
public:
	virtual void express(const JidPtr& jid, const string& msg, const Ice::Current& = Ice::Current());
	virtual void deliver(const MessageSeq& msgs, const Ice::Current& = Ice::Current());
	virtual bool bind(const string& sessionKey, const string& ticket,
			const JidPtr& j, const Ice::Current& = Ice::Current());
	virtual void unbind(const string& sessionKey, const JidPtr& j, const Ice::Current& = Ice::Current());
	virtual void closeAccessServer(const string& endpoint, const Ice::Current& = Ice::Current());
	virtual JidSeq getAccessJidSeq(int userid, const Ice::Current& = Ice::Current());

	virtual bool upDeliver(const JidPtr& jid, const string& msg, const Ice::Current& = Ice::Current());
	virtual OnlineCountPtr getOnlineCount(const Ice::Current& = Ice::Current());
	string getStatInfo();

	void destroySession(const JidPtr& jid);
	void push(const WebSessionPtr& sess);

	virtual MessageMap getMsg(int userId, Ice::Long msgId, const Ice::Current& = Ice::Current());
	void clearMsg(int userId, Ice::Long msgId, const Ice::Current& = Ice::Current());

	ProxyManagerPrx getAccessServerOneway(const string & endpoint);



	virtual bool StartNotify(int schemaid, int duration, const Ice::Current& = Ice::Current()){}
	virtual void StopNotify(const Ice::Current& = Ice::Current()){}
	virtual void SetIsDeliver(bool isdeliver, const Ice::Current& = Ice::Current()){}

};

}
;

#endif /*PROXYMANAGERI_H_*/
