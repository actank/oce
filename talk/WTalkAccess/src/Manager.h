#ifndef __PROXY_H__
#define __PROXY_H__

#include "TalkProxy.h"
#include "TalkLogic.h"
#include "TalkCommon.h"
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "asio.hpp"
#include "asio/ssl.hpp"
#include <IceUtil/IceUtil.h>
#include <IceUtil/Thread.h>
#include <Ice/Ice.h>
#include "Session.h"
#include "Singleton.h"
#include "TalkCommon.h"

namespace com {
namespace xiaonei {
namespace talk {
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace std;
using asio::ip::tcp;

const string PROXY_MANAGER = "M";

class Manager : public ProxyManager, public Singleton<Manager> {
public:
	virtual void deliver(const common::MessageSeq& mseq,
			const Ice::Current& = Ice::Current());
	virtual bool checkTicket(const JidPtr& jid, const string& ticket,
			const Ice::Current& = Ice::Current()) {
		return false;
	}
	;
	virtual bool verify(const JidPtr& jid, const string& ticket,
			const Ice::Current& = Ice::Current()) {
		return false;
	}
	;

	virtual void express(const JidPtr& jid, const string& msg,
			const Ice::Current& = Ice::Current());

	virtual void leave(const JidPtr& jid, const Ice::Current& = Ice::Current());
	virtual void relogin(const Ice::Current& = Ice::Current());

	virtual void broadcast(const string& xmlMsg,
			const Ice::Current& = Ice::Current());
	virtual bool indexExist(Ice::Long index, const Ice::Current& = Ice::Current());
	//virtual void event(const EventSeq& seq, const Ice::Current& = Ice::Current());
};

}
;
}
;
}
;
#endif

