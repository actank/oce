/*
 * =====================================================================================
 *
 *       Filename:  FeedItemFacadeReplicaAdapter.h
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

#ifndef __FEEDITEMFACADE_REPLICA_ADAPTER_H__
#define __FEEDITEMFACADE_REPLICA_ADAPTER_H__

#include "UserFeeds.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedItemFacadeReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<FeedItemFacadePrx>, public MyUtil::Singleton<FeedItemFacadeReplicaAdapter> {
public:
	FeedItemFacadeReplicaAdapter() : 
		MyUtil::ReplicatedClusterAdapterI<FeedItemFacadePrx> ("ControllerFeedItemFacade", 120, 300, new XceFeedControllerChannel()){
	}

	void put(const FeedSeedPtr& seed, const ::Ice::Current& current = ::Ice::Current());
};//end FeedItemFacadeReplicaAdapter

};
};

#endif
