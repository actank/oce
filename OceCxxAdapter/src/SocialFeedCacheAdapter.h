#ifndef _SOCIALFEEDCACHE_ADAPTER_H_
#define _SOCIALFEEDCACHE_ADAPTER_H_

#include "SocialFeedCache.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class SocialFeedCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialFeedCachePrx>,
	public MyUtil::Singleton<SocialFeedCacheAdapter> {
public:
	SocialFeedCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialFeedCachePrx>("ControllerSocialFeedCache", 120, 1000) {
	}

	void set(int hostId, const FeedSeq& feedList);	
	void remove(int hostId, const MyUtil::IntSeq& friendIds, const MyUtil::IntSeq& newCommonIds);	
	FeedSeq get(int hostId, int limit);
	
};

}
}

#endif
