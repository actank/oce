/*
 * ILikeItCacheAdapter.h
 *
 *  Created on: 2010-01-28
 *      Author: zhanghan
 */

#ifndef __MENU_CACHE_ADAPTER_H__
#define __MENU_CACHE_ADAPTER_H___

#include <IceUtil/RWRecMutex.h>
#include "MenuCache.h"
#include "IceLogger.h"
#include "IceExt/src/TopicI.h"
#include "UtilCxx/src/TaskManager.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace menu{

const int CACHE_CLUSTER = 10;

class MenuCacheReplicaAdapter : 
	public MyUtil::Singleton<MenuCacheReplicaAdapter> {
public:
	void setData(const ::MyUtil::ObjectResultPtr& objects, int mod, MenuCacheManagerPrx& localPrx, CacheType type);
	bool isValid(long id);

protected:
	friend class MyUtil::Singleton<MenuCacheReplicaAdapter>;
	MenuCacheReplicaAdapter();

private:
	xce::clusterstate::ClientInterface<MenuCacheManagerPrx> _clientCS;   /* @brief ClusterState的接口 */
	int _cluster;
};

}
}

#endif
