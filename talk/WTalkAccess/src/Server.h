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

//----------------------------------------------------------------------
class Server : public Singleton<Server> {
public:
	Server() :
		_joinCount(0), _onlineUserCount(0) {
		}

	void start(const Ice::PropertiesPtr& props);
	void stop();
/*
	void apply(SessionPtr session);
	void join(SessionPtr session,JidPtr jid);
	void leave(SessionPtr session);
	int applySize();
	int onlineSize();
*/
	bool deliver(const JidPtr& jid, const string& message);

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
private:

	::xce::nio::RefactorPtr _refactor;
	IceUtil::Mutex _mutex;

	int _joinCount, _onlineUserCount;


};



typedef IceUtil::Handle<Server> ServerPtr;
//---------------------------------------------

class ActiveUser : public Ice::Object{
public:
	string name;
};

typedef IceUtil::Handle<ActiveUser> ActiveUserPtr;

class ActiveUserFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

}
;
}
;
}
;
#endif

