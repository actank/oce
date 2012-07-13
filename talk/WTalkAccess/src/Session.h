#ifndef __SESSION_H__
#define __SESSION_H__

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Tag.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "TalkCommon.h"
#include "pugixml.hpp"
#include "Connection.h"
#include "WTalkProxyAdapter.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace std;

using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace pugi;
using namespace ::xce::nio;
using namespace ::talk::adapter;

const static size_t MAX_MSG_SIZE = 4*1024;

//----------------------------------------------------------------------
class Session : public AsioChannel {
public:
	Session();
	virtual ~Session();

	JidPtr& jid() {
		return _jid;
	}
	bool bind(const string& sessionKey, const string& ticket);
	void jid(const JidPtr& jid) {
		_jid = jid;
		_jid->index = connectionId();
	}
	void verify(bool v) {
		_verify = v;
	}
	bool verify() {
		return _verify;
	}

	bool operator<(const Session& rhs) {
		return this < &rhs;
	}

	friend ostream& operator<<(ostream& os, Session& rhs) {
		os << "Session[" << &rhs << "]";
		if (rhs._jid) {
			os << ",[" << rhs._jid->userId << "@" << rhs._jid->endpoint << "_"
					<< rhs._jid->index << "]";
		} else {
			os << "[null]";
		}
		os << "::";
		return os;
	}

	void token(const string& token){
		_token = token;
	}
	string& token() {
		return _token;
	}
protected:
	virtual void started();
	virtual void stopped();
	virtual bool messaged(const string& message);

private:
	IceUtil::Mutex _mutex;
	string _buffer;

	JidPtr _jid;
	bool _verify;
	string _token;
	string _sessionKey;
	IceUtil::Mutex _bindMutex;
	int _binding;
};

typedef IceUtil::Handle<Session> SessionPtr;

class SessionFactoryI : public AsioChannelFactory {
public:
	virtual AsioChannelPtr create() {
		return new Session;
	}
};

}
;
}
;
}
;

#endif
