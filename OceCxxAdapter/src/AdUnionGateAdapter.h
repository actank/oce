#ifndef _AD_UNION_GATE_ADAPTER_H_
#define _AD_UNION_GATE_ADAPTER_H_

#include "AdapterI.h"
#include "AdUnion.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdUnionGateAdapter :
		public AdapterI,
		public AdapterISingleton<MyUtil::AdChannel, AdUnionGateAdapter>
{
protected:
	virtual string name() {
		return "AdUnionGate";
	}
	virtual string endpoints() {
		return "@AdUnionGate";
	}
	virtual size_t cluster() {
		return 1;
	}

public:
	AdUnionGateAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

	bool Reload() {
    bool ret = true;
    for(size_t i = 0; i < cluster(); ++i) {
		  if (!getManager(i)->Reload()) {
        ret = false;
      }
    }
    return ret;
	}

private:
	AdUnionGatePrx getManager(int id);
  AdUnionGatePrx getManagerOneway(int id);

	vector<AdUnionGatePrx> _managersOneway;
	vector<AdUnionGatePrx> _managers;
};

}
}


#endif // _AD_UNION_GATE_ADAPTER_H_
