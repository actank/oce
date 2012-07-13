#ifndef __USERNETWORK_ADAPTER_H__
#define __USERNETWORK_ADAPTER_H__

#include "UserNetwork.h"
#include "Channel.h"
#include "Singleton.h"
#include "DataWrapperI.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;

class UserNetworkAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::SocialGraphChannel, UserNetworkAdapter> {
public:
	UserNetworkAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	
	NetworkBigObjectNPtr getBigObjectN(int userId);
	NetworkManagerPrx getNetworkManager(int userId);
        NetworkManagerPrx getNetworkManagerOneway(int userId);

private:
	virtual string name() {
		return "UserNetworkSocial";
	}
	virtual string endpoints() {
		return "@UserNetworkSocial";
	}
	virtual size_t cluster() {
		return 10;
	}


	typedef vector<NetworkManagerPrx> NetworkManagerSeq;

	NetworkManagerSeq _managers;
	NetworkManagerSeq _managersOneway;
};

}
;
}
;
}
;
}
;

#endif

