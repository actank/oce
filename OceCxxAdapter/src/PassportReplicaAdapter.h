#ifndef PASSPORTREPLICAADAPTER_H_
#define PASSPORTREPLICAADAPTER_H_

#include "AdapterI.h"
#include "Singleton.h"
#include "Passport.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace mop::hi::svc::passport;
using namespace MyUtil;

class PassportReplicaAdapter :
	public MyUtil::Singleton<PassportReplicaAdapter> {
public:
	PassportReplicaAdapter();
	string createTicket(int userId);
	int verifyTicket(const string& ticket);
	string queryTicket(int userId);
protected:

	xce::clusterstate::ClientInterface<PassportManagerPrx> _clientCS;   /* @brief ClusterState�~Z~D�~N��~O� */

        int _cluster;


};
}
;
}
;
}
;
}
;

#endif /*PASSPORTADAPTER_H_*/
