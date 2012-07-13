#ifndef ONLINESTATDISPATCHADAPTER_H_
#define ONLINESTATDISPATCHADAPTER_H_

#include "TalkCommon.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "OnlineStatSync.h"

namespace talk {
namespace online {
namespace adapter {

using namespace com::xiaonei::talk::common;
using namespace ::talk::online;
using namespace MyUtil;

class OnlineStatSyncAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, OnlineStatSyncAdapter> {
public:

	OnlineStatSyncAdapter() {
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void presence(const JidPtr& jid, OnlineStat stat, const string& ver, const string& ext);
	//void online(const JidPtr& j, const string& show="");
	void offline(const JidPtr& jid);
	void offlineBatch(const JidSeq& seq);

	void addBuddy(int host, int guest);
	void removeBuddy(int host, int guest);

protected:
	virtual string name() {
		return "OnlineStatSync";
	}
	virtual string endpoints() {
		return "@OnlineStatSync";
	}
	virtual size_t cluster() {
		return 10;
	}

	//OnlineManagerPrx getManager(int id, int timeout = 0);
	DispatchManagerPrx getManagerOneway(int id);

	vector<DispatchManagerPrx> _managersOneway;
	//vector<OnlineManagerPrx> _managers;


};

}
;
}
;
}
;

#endif /*ONLINESTATDISPATCHADAPTER_H_*/
