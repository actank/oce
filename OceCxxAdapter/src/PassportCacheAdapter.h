#ifndef PASSPORTCACHEADAPTER_H_
#define PASSPORTCACHEADAPTER_H_

#include <PassportCache.h>
#include <AdapterI.h>
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace passportcache {

class PassportCacheAdapter : public MyUtil::ReplicatedAdapterI,
        public MyUtil::AdapterISingleton<MyUtil::CoreChannel, PassportCacheAdapter> {
public:
	PassportCacheAdapter();
	void initialize();
	bool checkTicket(int userid, std::string ticket);
	MyUtil::ObjectResultPtr getPassportCache(std::vector<int> userIds);
protected:
	virtual bool isValid(const Ice::ObjectPrx& proxy);
private:
	friend class MyUtil::AdapterISingleton<MyUtil::CoreChannel, PassportCacheAdapter>;

	xce::clusterstate::ClientInterface<PassportCacheManagerPrx> _clientCS;
	virtual std::string name();
        virtual std::string endpoints();
        virtual size_t cluster();
};
}
}

#endif
