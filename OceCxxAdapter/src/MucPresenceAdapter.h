#ifndef MUCPRESENCEADAPTER_H_
#define MUCPRESENCEADAPTER_H_

#include "TalkLogic.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace adapter {
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class MucPresenceAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, MucPresenceAdapter> {
public:
	MucPresenceAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void message(const JidPtr& host, const string& msg);
	void mucOfflineNotify(const JidSeq& jids);

	size_t getCluster() {
		return _cluster;
	}
	;

protected:
	virtual string name() {
		return "MucPresence";
	}
	virtual string endpoints() {
		return "@MucPresence";
	}
	virtual size_t cluster() {
		return 10;
	}

	PresenceHandlerPrx getManager(int id);
	PresenceHandlerPrx getManagerOneway(int id);

	vector<PresenceHandlerPrx> _managersOneway;
	vector<PresenceHandlerPrx> _managers;
};

}
;
}
;

#endif
