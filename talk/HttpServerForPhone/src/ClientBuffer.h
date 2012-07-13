#ifndef CLIENTBUFFER_H_
#define CLIENTBUFFER_H_

#include "Server.h"
#include "Session.h"
#include "TaskManager.h"
namespace talk {
namespace http {

class ClientBuffer : public IceUtil::Shared {
	JidPtr _jid;
	Ice::Long _activeConnection;
	//Ice::Long _msgid;
	//map<Ice::Long,MessagePtr> _msgs;
	time_t _timestamp;

	//int _requestTimes;
	int _flushCount;

public:
	ClientBuffer() :
		_activeConnection(0), /*_msgid(0),*/ _timestamp(time(NULL)),
				/*_requestTimes(0),*/ _flushCount(0) {
	}
	;
	void timestamp(time_t time);
	time_t timestamp();
	bool bind(const JidPtr j, Ice::Long activeConnection);

	bool isFlush() {
		return (++_flushCount)%3 == 0;
	}
	JidPtr jid();
	Ice::Long activeConnection();
	void activeConnection(Ice::Long conn);

	void pushMsg(const MessagePtr& msg);

	string getMsg(Ice::Long lastMsgId);
	string msgToString(MessageMap msgs);
};

typedef IceUtil::Handle<ClientBuffer> ClientBufferPtr;

class ClientPool {

public:

	void pushMsg(Ice::Long sessionId, const MessagePtr& msg);
	string getMsg(Ice::Long sessionId, Ice::Long msgid);

	bool bind(Ice::Long connection, const JidPtr& j, const string& ticket,
			bool flush, bool isWap = false);

	JidPtr getJid(Ice::Long sessionid);
	Ice::Long getActiveConnection(Ice::Long sessionid);
	void unbind(Ice::Long sessionId, Ice::Long connection, bool unbindNow=false);
	int size();

	bool offline();
	bool timeout();
private:
	map<Ice::Long,ClientBufferPtr> _clients;
	IceUtil::Mutex _mutex/*, _queueMutex, _timeoutMutex*/;
	deque<pair<Ice::Long,time_t> > _offlineQueue;
	deque<pair<Ice::Long,time_t> > _timeoutQueue;

	void pushOfflineQueue(pair<Ice::Long,time_t>& o);
	void pushTimeoutQueue(pair<Ice::Long,time_t>& t);

};

class ClientPoolManager : public Singleton<ClientPoolManager> {
public:
	ClientPool& getClientPool(Ice::Long sid) {
		return _pool[sid%1000];
	}
	;
	void startTasks() {
		TaskManager::instance().scheduleRepeated(new DeleteTimer);
		TaskManager::instance().scheduleRepeated(new TimeoutTimer);
		TaskManager::instance().scheduleRepeated(new StatTimer);
	}
	;
private:
	ClientPool _pool[1000];

	class DeleteTimer : public MyUtil::Timer {
public:
		DeleteTimer() :
			Timer(2000) {
		}
		;
		virtual void handle() {
			for (size_t i = 0; i < 1000; ++i) {
				while (ClientPoolManager::instance().getClientPool(i).offline()) {
				}
			}
			//TaskManager::instance().schedule(this);
		}
		;
	};

	class TimeoutTimer : public MyUtil::Timer {
public:
		TimeoutTimer() :
			Timer(1000) {
		}
		;
		virtual void handle() {
			for (size_t i = 0; i < 1000; ++i) {
				while (ClientPoolManager::instance().getClientPool(i).timeout()) {
				}
			}
			//TaskManager::instance().schedule(this);
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
			for (size_t i = 0; i < 1000; ++i) {
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
