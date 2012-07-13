#ifndef __CLUSTER_STATE_LISTENER_H__
#define __CLUSTER_STATE_LISTENER_H__

#include "ReplicaCluster.h"
#include "ReplicaCluster/SubjectObserver/src/server/SubjectObserverI.h"
#include "UtilCxx/src/Singleton.h"

namespace xce {
namespace clusterstate {


class ClusterStateListener : public MyUtil::Singleton<ClusterStateListener> {

public:
	ClusterStateListener():
		_name("ClusterState")
	{
	}

        void NotifyClient(const ServerStateMap& seqInfo);
private:
	std::string _name;
	std::string _endpoints;
};
};
};

#endif
