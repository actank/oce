#ifndef _FRIENDCLUSTERCACHE_ADAPTER_H_
#define _FRIENDCLUSTERCACHE_ADAPTER_H_

#include "FriendClusterCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class FriendClusterCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::FriendClusterCachePrx>,
	public MyUtil::Singleton<FriendClusterCacheAdapter> {
public:
	FriendClusterCacheAdapter():
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::FriendClusterCachePrx>("ControllerFriendClusterCache", 120, 1000) {
	}
  
  FriendClusterData Get(MyUtil::IntList userIdList);
};

}
}

#endif
