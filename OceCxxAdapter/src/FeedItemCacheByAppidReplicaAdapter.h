/*
 * =====================================================================================
 *
 *       Filename:  FeedItemCacheByAppidReplicaAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月21日 14时22分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FEEDITEMCACHE_BYAPPID_REPLICA_ADAPTER_H__
#define __FEEDITEMCACHE_BYAPPID_REPLICA_ADAPTER_H__

#include "UserFeeds.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedItemCacheByAppidReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<AppFeedsPrx>, public MyUtil::Singleton<FeedItemCacheByAppidReplicaAdapter> {
public:
	FeedItemCacheByAppidReplicaAdapter() : 
		MyUtil::ReplicatedClusterAdapterI<AppFeedsPrx> ("ControllerFeedItemCacheByAppid", 120, 300, new XceFeedControllerChannel()){
	}

	void put(const FeedMeta& d, const ::Ice::Context& context);

	FeedMetaSeq getByTypes(const IntSeq& appids, ::Ice::Int begin, ::Ice::Int limit, const IntSeq& types);
	
	FeedMetaSeq getGreaterThanFeedid(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit);
	FeedMetaSeq getGreaterThanFeedidWithUseridByType(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit, ::Ice::Int userid, const MyUtil::IntSeq& types);
};//end FeedItemCacheByAppidReplicaAdapter

};
};

#endif
