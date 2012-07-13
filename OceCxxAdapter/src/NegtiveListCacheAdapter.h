#ifndef _NEGTIVELISTCACHE_ADAPTER_H_
#define _NEGTIVELISTCACHE_ADAPTER_H_

#include "NegtiveListCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class NegtiveListCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::NegtiveListCachePrx>,
	public MyUtil::Singleton<NegtiveListCacheAdapter> {
public:
	NegtiveListCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::NegtiveListCachePrx>("ControllerNegtiveListCache", 120, 1000) {
	}

    void Set(int hostId, MyUtil::IntSeq negtiveList);
    MyUtil::IntSeq Get(int hostId, int limit);
    void Remove(int hostId, MyUtil::IntSeq negtiveList);
};

}
}

#endif
