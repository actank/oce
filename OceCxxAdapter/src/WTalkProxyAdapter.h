#ifndef WTALKPROXYADAPTER_H_
#define WTALKPROXYADAPTER_H_

#include "WTalkProxy.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace adapter {

using namespace ::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class WTalkProxyAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, WTalkProxyAdapter> {
public:
	WTalkProxyAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	//void deliver(const MessageSeq& msgs);
	bool bind(const string& sessionKey, const string& ticket, const JidPtr& j);
	void unbind(const string& sessionKey, const JidPtr& j);
	bool upDeliver(const JidPtr& jid, const string& msg);
	void closeAccessServer(const string& endpoint);
	OnlineCountPtr getOnlineCount();
protected:
	virtual string name() {
		return "WTalkProxy";
	}
	virtual string endpoints() {
		return "@WTalkProxy";
	}
	virtual size_t cluster() {
		return 10;
	}

	WTalkProxyManagerPrx getManager(int id);
	WTalkProxyManagerPrx getManagerOneway(int id);

	vector<WTalkProxyManagerPrx> _managersOneway;
	vector<WTalkProxyManagerPrx> _managers;
};

}
}

#endif /*WTALKPROXYADAPTER_H_*/
