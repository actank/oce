#ifndef MESSAGEMSGHANDLERADAPTER_H_
#define MESSAGEMSGHANDLERADAPTER_H_

#include "TalkLogic.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace adapter {
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class MessageMsgHandlerAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, MessageMsgHandlerAdapter> {
public:
	MessageMsgHandlerAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void message(const JidPtr& host, const string& msg, const Ice::Context& = Ice::Context());
	//void loadOfflineMessage(const JidPtr& jid);

	size_t getCluster() {
		return _cluster;
	}
	;
  void deliverAppNotice(int toid, int appid, const string& appName, const string& appIcon, const string& msg);

protected:
	virtual string name() {
		return "MessageMsgHandler";
	}
	virtual string endpoints() {
		return "@MessageMsgHandler";
	}
	virtual size_t cluster() {
		return 10;
	}

	MessageHandlerPrx getManager(int id);
	MessageHandlerPrx getManagerOneway(int id);

	vector<MessageHandlerPrx> _managersOneway;
	vector<MessageHandlerPrx> _managers;
};

}
;
}
;

#endif /*TALKLOGICADAPTER_H_*/
