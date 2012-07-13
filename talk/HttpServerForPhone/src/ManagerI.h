#ifndef MANAGERI_H_
#define MANAGERI_H_

#include "TalkProxy.h"
#include "Singleton.h"

namespace talk
{
namespace http
{
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk;
using namespace MyUtil;

const string PROXY_MANAGER = "M";

class ManagerI : public ProxyManager, public Singleton<ManagerI> {
public:

//	virtual void cometDeliver(const Long2MsgMap& msg, const Ice::Current& = Ice::Current());

	virtual void deliver(const MessageSeq& mseq,
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
			const Ice::Current& = Ice::Current()){};

	virtual void leave(const JidPtr& jid, const Ice::Current& = Ice::Current()){};
	virtual string GetTicket(const JidPtr& jid, const Ice::Current& = Ice::Current()){};
	virtual void relogin(const Ice::Current& = Ice::Current()){};

	virtual void broadcast(const string& xmlMsg,
			const Ice::Current& = Ice::Current()){};

	virtual bool indexExist(Ice::Long index, const Ice::Current& = Ice::Current());
	virtual void kick(const JidPtr& jid, const Ice::Current& = Ice::Current());
};

}
}
#endif /*MANAGERI_H_*/

