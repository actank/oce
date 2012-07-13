#ifndef __SERVER_H__
#define __SERVER_H__

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Thread.h>
#include <Ice/Ice.h>
#include "Session.h"
#include "Singleton.h"
#include "TalkCommon.h"
#include "ServiceI.h"
#include "Refactor.h"
#include "Connection.h"


void changeDomain(string& data, const string& src, const string& dst);

namespace com {
namespace xiaonei {
namespace talk {

using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace std;


struct less_jid : public binary_function<JidPtr, JidPtr, bool> {
	bool operator()(const JidPtr& x, const JidPtr& y) {
		if (x->userId < y->userId) {
			return true;
		} else if (x->userId == y->userId) {
			return x->index < y->index;
		} else {
			return false;
		}
	}
};
typedef map<JidPtr, SessionPtr, less_jid> SessionMap;
typedef set<SessionPtr, less_jid> SessionSet;
//----------------------------------------------------------------------
class Server : public Singleton<Server> {
public:
	Server() :
		_joinCount(0), _onlineUserCount(0) {
	}
	;
	void start(const Ice::PropertiesPtr& props);
	void stop();

	//	void apply(const SessionPtr& session);
	//	void join(const SessionPtr& session);
	//	void leave(const SessionPtr& session);
	//	void leave(const JidPtr& jid);

	bool deliver(const JidPtr& jid, const string& message);
	bool verify(const JidPtr& jid, const string& ticket);
	string GetTicket(const JidPtr& jid);

	void join() {
		IceUtil::Mutex::Lock lock(_mutex);
		++_joinCount;
	}
	void leave() {
		IceUtil::Mutex::Lock lock(_mutex);
		--_joinCount;
	}
	int getJoinCount(){
		return _joinCount;
	}
	void bind() {
		IceUtil::Mutex::Lock lock(_mutex);
		++_onlineUserCount;
	}
	void unbind() {
		IceUtil::Mutex::Lock lock(_mutex);
		--_onlineUserCount;
	}
	int getOnlineUserCount(){
		return _onlineUserCount;
	}
	int size(bool apply = false);
	bool indexExist(Ice::Long index);
	//void sendLog(const JidPtr& from, const JidPtr& to, const string& msg);
	bool isRenren(){
		return _renren;
	}
	void kick(const JidPtr& jid);
private:

	RefactorPtr _refactor;
	IceUtil::Mutex _mutex;
	//	set<SessionPtr> _applies;
	//	SessionMap _sessions; // userid-resourceid -> sessionPtr;
	//	SessionSet _sessions;
	int _joinCount, _onlineUserCount;
	bool _renren;

};

typedef IceUtil::Handle<Server> ServerPtr;

}
;
}
;
}
;
#endif

