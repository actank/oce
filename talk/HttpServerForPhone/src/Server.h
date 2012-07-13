#ifndef SERVER_H_
#define SERVER_H_

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <boost/lexical_cast.hpp>
//

#include "Singleton.h"
//#include "Session.h"
#include "TalkCommon.h"
#include "ServiceI.h"

#include "Refactor.h"

namespace talk {
namespace http {

using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace std;
//using namespace talk::adapter;


class Session;
typedef IceUtil::Handle<Session> SessionPtr;

class Server : public Singleton<Server> {
public:

	void start(const Ice::PropertiesPtr& props);
	void stop();

//	bool bind(Ice::Long connection, const JidPtr& j, const string& ticket);

//	void unbind(Ice::Long session_id, Ice::Long connection) {
//
//		IceUtil::Mutex::Lock lock(_mutex);
//
//		map<Ice::Long,CometClientPtr>::iterator it = _clients.find(session_id);
//		if (it != _clients.end()) {
//			if (it->second->active_connection == connection) {
//				WTalkProxyAdapter::instance().unbind(boost::lexical_cast<string>(it->second->jid->index),it->second->jid);
//				_clients.erase(session_id);
//			}
//		}
//	}
	int size(){
		return _refactor->connectionPoolSize();
	}
	bool isWap(){
		return _isWap;
	}
	void setWapFlag(bool isWap){
		_isWap = isWap;
	}
//	void deliver(const Long2MsgMap & msgs);
//	void popMsg(Ice::Long session_id, MessageSeq& mseq);
	bool indexExist(Ice::Long index);
	SessionPtr getSession(Ice::Long connId);
	int getServerIndex(){
		return _serverIndex;
	}
private:

	::xce::nio::RefactorPtr _refactor;
	IceUtil::Mutex _mutex;
	bool _isWap;
	int _serverIndex;
//	//key is cookie:sessionid, 以保证当前cometclient是同一个浏览器会话,同时也是 jid的Resource ID
//	map<Ice::Long,CometClientPtr> _clients;
};
typedef IceUtil::Handle<Server> ServerPtr;

class IdleConnManager : public Timer, public Singleton<IdleConnManager> {
public:
	IdleConnManager() : Timer(1*1000){
		_cometConnSwitch = 0;
		_stoppedReadNothing = 0;
		_stoppedVerify = 0;
		_stoppedNotVerify = 0;
		_stoppedReadSomething = 0;
		_stopedNullComet = 0;
		_stopedOtherConnId = 0;
		_stopedValidComet = 0;
	}
	void handle();
	void push(Ice::Long connId){
		IceUtil::Mutex::Lock lock(_mutex);
		_sessions.push_back(make_pair(connId, ::time(NULL)));
	}
	void cometConnSwitch(){
		IceUtil::Mutex::Lock lock(_mutex);
		_cometConnSwitch++;
	}
	//void connStopped(Ice::Long connId, bool isReadSomthing, bool isVerify);
private:
	const static time_t CONN_TIMEOUT = 5;
	list<pair<Ice::Long, time_t> > _sessions;
	int	_cometConnSwitch;
	int _stoppedReadNothing;
	int _stoppedVerify;
	int _stoppedNotVerify;
	int _stoppedReadSomething; //read something, but not verified

	int _stopedNullComet;
	int _stopedOtherConnId;
	int _stopedValidComet;

	IceUtil::Mutex _mutex;
};


}
;
}
;

#endif /*SERVER_H_*/
