#ifndef CLIENTBUFFER_H_
#define CLIENTBUFFER_H_

#include "Server.h"
#include "Session.h"
#include "TaskManager.h"
namespace talk {
namespace http {


class TimeStat{
	timeval _begin, _end;
	public:
	TimeStat(){
		reset();
	}
	void reset(){
		gettimeofday(&_begin, NULL);
	}
	float getTime(){
		gettimeofday(&_end, NULL);
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec-_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};




class ClientBuffer : public IceUtil::Shared {
public:
	ClientBuffer() :
		_activeConnection(0), _msgid(0), _timestamp(time(NULL)),
				_requestTimes(0), _flushCount(0) {
	}
	;
	void 		timestamp(time_t time);
	time_t 		timestamp();
	bool 		bind(const JidPtr j, Ice::Long activeConnection);
	JidPtr 		jid();
	Ice::Long 	activeConnection();
	void 		activeConnection(Ice::Long conn);
	void 		pushMsg(const MessagePtr& msg);
	string 		getMsg(Ice::Long lastMsgId);

	bool isFlush() {
		return (++_flushCount)%3 == 0;
	}
private:
	JidPtr 		_jid;
	Ice::Long 	_activeConnection;
	Ice::Long 	_msgid;
	time_t 		_timestamp;
	int 		_requestTimes;
	int 		_flushCount;
	map<Ice::Long,MessagePtr> _msgs;
};

typedef IceUtil::Handle<ClientBuffer> ClientBufferPtr;

class ClientPool {

public:

	void 		pushMsg(Ice::Long sessionId, const MessagePtr& msg);
	string 		getMsg(Ice::Long sessionId, Ice::Long msgid);
	bool 		bind(Ice::Long connection, const JidPtr& j, const string& ticket, bool flush);
	JidPtr 		getJid(Ice::Long sessionid);
	Ice::Long 	getActiveConnection(Ice::Long sessionid);
	void 		unbind(Ice::Long sessionId, Ice::Long connection);
	int 		size();

	bool 		offline();
	bool 		timeout();
private:
	const static int SESSION_TIMEOUT = 10;
	map<Ice::Long,ClientBufferPtr> _clients;
	IceUtil::Mutex _mutex/*, _queueMutex, _timeoutMutex*/;
	deque<pair<Ice::Long,time_t> > _offlineQueue;
	//deque<pair<Ice::Long,time_t> > _timeoutQueue;

	//void pushOfflineQueue(pair<Ice::Long,time_t>& o);
	//void pushTimeoutQueue(pair<Ice::Long,time_t>& t);

};

class ClientPoolManager : public Singleton<ClientPoolManager> {
public:
	const static int POOL_SIZE = 1000;
	ClientPool& getClientPool(Ice::Long sid) {
		if(sid < 0){
			MCE_INFO("ClientPoolManager::getClientPool --> " << sid);
		}
		return _pool[abs(sid)%POOL_SIZE];
	}
	;
	void startTasks() {
		TaskManager::instance().scheduleRepeated(new DeleteTimer);
		TaskManager::instance().scheduleRepeated(new StatTimer);
	}
	;
private:
	ClientPool _pool[POOL_SIZE];

	class DeleteTimer : public MyUtil::Timer {
public:
		DeleteTimer() :
			Timer(2000) {
		}
		;
		virtual void handle() {
			TimeStat ts;
			for (int i = 0; i < ClientPoolManager::POOL_SIZE; ++i) {
				while (ClientPoolManager::instance().getClientPool(i).offline()) {
				}
			}
			MCE_INFO("DeleteTimer::handle--> usetime:" << ts.getTime());
		}
		;
	};

	class StatTimer : public MyUtil::Timer {
public:
		StatTimer() :
			Timer(5000) {
		}
		;
		virtual void handle() {
			int count = 0;
			for (int i = 0; i < ClientPoolManager::POOL_SIZE; ++i) {
				count += ClientPoolManager::instance().getClientPool(i).size();
			}
			MCE_INFO("ONLINE SIZE:"<< count);
			//TaskManager::instance().schedule(this);
		}
		;
	};
};

}
;
}
;

#endif /*CLIENTBUFFER_H_*/
