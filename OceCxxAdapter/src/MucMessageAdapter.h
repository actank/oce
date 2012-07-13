#ifndef MUCMESSAGEADAPTER_H_
#define MUCMESSAGEADAPTER_H_

#include "TalkLogic.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk{
namespace adapter {
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class MucMessageAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, MucMessageAdapter> {
public:
	MucMessageAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void message(const JidPtr& host, const string& msg);
	//void loadOfflineMessage(const JidPtr& jid);

	size_t getCluster(){
		return _cluster;
	};

protected:
	virtual string name() {
		return "MucMessage";
	}
	virtual string endpoints() {
		return "@MucMessage";
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
