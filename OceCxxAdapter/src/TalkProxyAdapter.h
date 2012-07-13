#ifndef TALKPROXYADAPTER_H_
#define TALKPROXYADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "TalkProxy.h"
#include "TalkCommon.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace adapter {

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;

class TalkProxyAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::TalkChannel, TalkProxyAdapter> {
public:

	void express(const JidPtr& jid, const string& msg);
	void deliver(const MessagePtr& msg);
	void deliver(const MessageSeq& msgs);
	bool verify(const JidPtr& jid, const string& ticket);
	string GetTicket(const JidPtr& jid);
	void leave(const JidPtr& jid);
	void relogin();
	void broadcast(const string& endpoint,const string& xmlMsg);
	void kick(const JidPtr& jid);
protected:
	virtual string name() {
		return "TalkProxy";
	}
	virtual string endpoints() {
		return "@TalkProxy";
	}
	virtual size_t cluster() {
		return 0;
	}

	ProxyManagerPrx getManager(const string & endpoint);
	ProxyManagerPrx getManagerOneway(const string & endpoint);
	ProxyManagerPrx getManagerBatchOneway(const string & endpoint);

	map<string,ProxyManagerPrx> _managersOneway;
	map<string,ProxyManagerPrx> _managers;
	//map<string,ProxyManagerPrx> _managersBatchOneway;
};

}
;
}
;
}
;
}
;

#endif /*TALKPROXYADAPTER_H_*/
