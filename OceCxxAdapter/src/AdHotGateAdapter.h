#ifndef _REPLYNOTIFYNGINXADAPTER_H_
#define _REPLYNOTIFYNGINXADAPTER_H_

#include "AdapterI.h"
#include "AdHot.h"
#include "Singleton.h"
#include "AdapterI.h"
// #include "TaskManager.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdHotGateAdapter :
		public AdapterI,
		public AdapterISingleton<MyUtil::NotifyChannel, AdHotGateAdapter>
{

protected:
	virtual string name() {
		return "M";
	}
	virtual string endpoints() {
		return "@AdHotGate";
	}
	virtual size_t cluster() {
		return 1;
	}

public:
	AdHotGateAdapter() {
		_managers_oneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void Reload() {
    // for(size_t i = 0; i < cluster(); ++i) {
		getManagerOneway(0)->Reload();
    // }
	}
private:
	AdHotGatePrx getManagerOneway(int id);
	vector<AdHotGatePrx> _managers_oneway;
};

}
}


#endif /* _REPLYNOTIFYNGINX_H_ */
