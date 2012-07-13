#ifndef __FRIENDFINDER_INFOWORKER_REPLICA_ADAPTER_H__ 
#define __FRIENDFINDER_INFOWORKER_REPLICA_ADAPTER_H__ 

#include "FriendFinderInfoWorker.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace xce {
namespace friendfinder {
namespace adapter {
using namespace MyUtil;

class FriendFinderInfoWorkerReplicaAdapter  : public MyUtil::ReplicatedClusterAdapterI<xce::friendfinder::FriendFinderInfoWorkerPrx>,
    public MyUtil::Singleton<FriendFinderInfoWorkerReplicaAdapter>  {
public:
	FriendFinderInfoWorkerReplicaAdapter() :
		MyUtil::ReplicatedClusterAdapterI <xce::friendfinder::FriendFinderInfoWorkerPrx>("ControllerFriendFinderInfoWorker", 120, 1000) {
	}

	void createDataFromDB(int userId);
	void restartControl();
};

}
}
}

#endif

