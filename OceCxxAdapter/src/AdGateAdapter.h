#ifndef _AD_GATE_ADAPTER_H_
#define _AD_GATE_ADAPTER_H_

#include "AdapterI.h"
#include "AdGate.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdGateAdapter :
		public AdapterI,
		public AdapterISingleton<MyUtil::AdChannel, AdGateAdapter>
{
protected:
	virtual string name() {
		return "AdGate";
	}
	virtual string endpoints() {
		return "@AdGate";
	}
	virtual size_t cluster() {
		return 1;
	}

public:
	AdGateAdapter(){
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

	bool ReloadCreativeById(Ice::Long id) {
    for(size_t i = 0; i < cluster(); ++i)
    {
		  getManager(i)->ReloadCreativeById(id);
    }
    return true;
	}

private:
	AdGatePrx getManager(int id);
  AdGatePrx getManagerOneway(int id);

	vector<AdGatePrx> _managersOneway;
	vector<AdGatePrx> _managers;
};

}
}

#endif // _AD_GATE_ADAPTER_H_
