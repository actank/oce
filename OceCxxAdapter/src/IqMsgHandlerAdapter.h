#ifndef IQMSGHANDLERADAPTER_H_
#define IQMSGHANDLERADAPTER_H_

#include "TalkLogic.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk{
namespace adapter {
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class IqMsgHandlerAdapter : public MyUtil::ReplicatedClusterAdapterI<IqHandlerPrx>, public MyUtil::Singleton<IqMsgHandlerAdapter>  {
public:

	IqMsgHandlerAdapter() : MyUtil::ReplicatedClusterAdapterI<IqHandlerPrx> ("ControllerIqMsgHandler",120,300,new TalkChannel) {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void RemoveNotifyBySource(int userId, int type, Ice::Long source);
	void RemoveNotifyById(int userId, Ice::Long notifyId);
	void NotifyBuddyApplyChange(int userId);
	void removeBuddyNotify(int host, int guest);
	void message(const JidPtr& host, const string& msg);
	void changeBuddyGroupNotify(const BuddyItemPtr& ug);

	size_t getCluster(){
		return 10;
	};

protected:
	virtual string name() {
		return "IqMsgHandler";
	}
	virtual string endpoints() {
		return "@IqMsgHandler";
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
