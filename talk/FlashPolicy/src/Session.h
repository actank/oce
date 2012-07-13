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

namespace com {
namespace xiaonei {
namespace talk {

using namespace std;

using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace pugi;
using namespace ::xce::nio;


const static size_t MAX_MSG_SIZE = 50*1024;



//class HttpParser : public IceUtil::Shared {
//public:
//	
//	void feed(const Ice::ObjectPtr& channel, const string& data);
//private:
//	string _str;
//};
//typedef IceUtil::Handle<HttpParser> HttpParserPtr;

//----------------------------------------------------------------------
class Session : public AsioChannel {
public:
	Session();
	virtual ~Session();

//	bool operator<(const Session& rhs) {
//		return this < &rhs;
//	}
/*
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
	}*/
	
	void clearCount() {
		_counter = 0;
	}
	
	void setCloseStat(){
		_isClose = true;
	}

protected:
	virtual void started();
	virtual void stopped();
	virtual bool messaged(const string& message);
	virtual void finishWrite();
	
private:
	IceUtil::Mutex _mutex;
	size_t _counter;
	//HttpParserPtr _parser;
	bool _isClose;


	
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
