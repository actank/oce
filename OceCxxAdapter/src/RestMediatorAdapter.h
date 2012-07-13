#ifndef RESTMEDIATORADAPTER_H_
#define RESTMEDIATORADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RestMediator.h"

namespace talk {
namespace rest {

using namespace MyUtil;

class RestMediatorAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, RestMediatorAdapter> {
public:
	void restRequest(Ice::Long reqId, int userid, const string& path, const map<string, string>& paras);
protected:
	virtual string name() {
		return "RestMediator";
	}
	virtual string endpoints() {
		return "@RestMediator";
	}
	virtual size_t cluster() {
		return 2;
	}
	RestMediatorManagerPrx getManagerOneway(int id);
	vector<RestMediatorManagerPrx> _managerOneway;
	

};
}
}

#endif /*RESTMEDIATORADAPTER_H_*/
