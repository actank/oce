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
#include "util/cpp/String.h"
#include "XmppTools.h"
#include "IceLogger.h"
#include "Connection.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace std;
using asio::ip::tcp;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace MyUtil;
using namespace xce::nio;

const static size_t MAX_MSG_SIZE = 4*1024;

enum SessionState {
	SessionStateNone,
	SessionStateFirstStream,
	SessionStateSecondStream,
	SessionStateLogin,
	SessionStateChat,
};

//----------------------------------------------------------------------
class Session :  public AsioChannel {
public:
	Session();
	virtual ~Session();

	string& ticket();

	JidPtr& jid() {
		return _jid;
	}

	void jid(const JidPtr& j){
		_jid = j;
	}
	void presence(const string& msg);

	void state(SessionState state) {
		_state = state;
	}
	SessionState state() {
		return _state;
	}

	bool operator<(const Session& rhs) {
		return this < &rhs;
	}

	friend ostream& operator<<(ostream& os, Session& rhs) {
		os << "Session [" << &rhs << "]";
		if (rhs._jid) {
			os << ",[" << rhs._jid->userId << "@" << rhs._jid->endpoint << "_"
					<< rhs._jid->index << "]";
		} else {
			os << "[null]";
		}
		os << "::";
		return os;
	}

	//bool saslHandshake(SaslHandshakeStep step,const string& instr);
	void clearCounter(){
		_counter = 0;
	}
	void setShutdownAt(int at){
		_shutdownAt = at;
	}
protected:
	virtual void started();
	virtual void stopped();
	virtual bool messaged(const string& message);

private:
	size_t _counter;
	TagParserPtr _parser;

	string _ticket;
	JidPtr _jid;
	SessionState _state;
	IceUtil::Mutex _bindMutex;
	int _binding;// 0 , 1 is bind, 2 is unbind
	time_t _timestamp;

	int _shutdownAt;
};
typedef IceUtil::Handle<Session> SessionPtr;

class SessionFactoryI : public AsioChannelFactory {
public:
	virtual AsioChannelPtr create() {
		return new Session;
	}
};

//-----------------------------------------------
//SASL call back


}
;
}
;
}
;

#endif
