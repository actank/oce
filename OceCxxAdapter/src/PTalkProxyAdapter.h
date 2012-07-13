#ifndef PTALKPROXYADAPTER_H_
#define PTALKPROXYADAPTER_H_

#include "WTalkProxy.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace adapter {

using namespace ::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class PTalkProxyAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, PTalkProxyAdapter> {
public:
	PTalkProxyAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	//void deliver(const MessageSeq& msgs);
	bool bind(const string& sessionKey, const string& ticket, const JidPtr& j);
	void unbind(const string& sessionKey, const JidPtr& j);
	void closeAccessServer(const string& endpoint);
	bool upDeliver(const JidPtr& jid, const string& msg);
	MessageMap getMsg(int userId, Ice::Long msgId);
	OnlineCountPtr getOnlineCount();
protected:
	virtual string name() {
		return "PTalkProxy";
	}
	virtual string endpoints() {
		return "@PTalkProxy";
	}
	virtual size_t cluster() {
		return 2;
	}

	WTalkProxyManagerPrx getManager(int id);
	WTalkProxyManagerPrx getManagerOneway(int id);

	vector<WTalkProxyManagerPrx> _managersOneway;
	vector<WTalkProxyManagerPrx> _managers;
};

}
}

#endif /*WTALKPROXYADAPTER_H_*/
