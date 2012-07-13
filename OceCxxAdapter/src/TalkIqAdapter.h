#ifndef TALKIQADAPTER_H_
#define TALKIQADAPTER_H_

#include "TalkLogic.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk{
namespace adapter {
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class TalkIqAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, TalkIqAdapter> {
public:

	TalkIqAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	
	void removeBuddyNotify(int host, int guest);
	void message(const JidPtr& host, const string& msg);
	void changeBuddyGroupNotify(const BuddyItemPtr& ug);
	
	size_t getCluster(){
		return _cluster;
	};

protected:
	virtual string name() {
		return "TalkIq";
	}
	virtual string endpoints() {
		return "@TalkIq";
	}
	virtual size_t cluster() {
		return 10;
	}

	IqHandlerPrx getManager(int id);
	IqHandlerPrx getManagerOneway(int id);

	vector<IqHandlerPrx> _managersOneway;
	vector<IqHandlerPrx> _managers;
};

}
;
}
;


#endif /*TALKLOGICADAPTER_H_*/
