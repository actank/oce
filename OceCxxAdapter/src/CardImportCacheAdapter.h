#ifndef __CARD_IMPORT_CACHE_H__
#define __CARD_IMPORT_CACHE_H__

#include "CardCache.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace cardcache{


class CardImportCacheAdapter :  public MyUtil::Singleton<CardImportCacheAdapter>{
public:
	CardImportCacheAdapter();
  ImportInfoSeq get(int userId, int limit);
  ImportInfoNewSeq getNew(int userId, int limit);
private:
	virtual string name() {
		return "M";
	}

	virtual string endpoints() {
		return "@CardImportCache";
	}

	virtual size_t cluster() {
		return 1;
	}
	xce::clusterstate::ClientInterface<CardImportCachePrx> clientCS_;

};

};
};

#endif
