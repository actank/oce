
#ifndef UGC_GOSSIP_ADAPTER_H
#define UGC_GOSSIP_ADAPTER_H

#include "AdapterI.h"
#include "Singleton.h"
#include "UgcGossip.h"

namespace xce {
namespace ucgcomment {

using namespace MyUtil;
using namespace com::xiaonei::wService::gossip;


class UgcGossipAdapter: public MyUtil::AdapterI,
		public AdapterISingleton<MyUtil::WServiceUGCChannel, UgcGossipAdapter> {
public:
	UgcGossipAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }
  
  int send(int host, int owner, const string& message); 

protected:
	virtual string name() {
		return "GossipLogic";
	}
	virtual string endpoints() {
		return "@GossipLogic";
	}
	virtual size_t cluster() {
		return 1;
	}

	GossipLogicPrx  getManager(int id);
	GossipLogicPrx  getManagerOneway(int id);

	vector<GossipLogicPrx>  _managersOneway;
	vector<GossipLogicPrx>  _managers;
};

}
}
#endif
