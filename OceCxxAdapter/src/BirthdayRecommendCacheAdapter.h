#ifndef _BIRTHDAYRECOMMENDCACHE_ADAPTER_H_
#define _BIRTHDAYRECOMMENDCACHE_ADAPTER_H_

#include "BirthdayRecommendCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class BirthdayRecommendCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::BirthdayRecommendCachePrx>,
		public MyUtil::Singleton<BirthdayRecommendCacheAdapter> {
public:
	BirthdayRecommendCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::BirthdayRecommendCachePrx>("ControllerBirthdayRecommendCache", 120, 1000) {
	}

	void Set(int hostId, const MyUtil::IntSeq& birthdayRecommendList);

    int Get(int hostId);
    int GetAndRemove(int hostId);
    MyUtil::IntSeq GetList(int hostId, int limit);
    void Remove(int hostId, int friendId);
};

}
}

#endif
