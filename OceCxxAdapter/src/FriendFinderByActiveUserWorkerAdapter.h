#ifndef _FRIENDFINDER_BYACTIVEUSERWORKER_ADAPTER_H__
#define _FRIENDFINDER_BYACTIVEUSERWORKER_ADAPTER_H__

#include "FriendFinderByActiveUserWorker.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace friendfinder {
namespace adapter {

class FriendFinderByActiveUserWorkerAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::friendfinder::FriendFinderByActiveUserWorkerPrx>,
	public MyUtil::Singleton<FriendFinderByActiveUserWorkerAdapter> {
public:
	FriendFinderByActiveUserWorkerAdapter() :
		MyUtil::ReplicatedClusterAdapterI<xce::friendfinder::FriendFinderByActiveUserWorkerPrx>("ControllerFriendFinderByActiveUserWorker", 120, 1000) {
	}

	void load();
};

}
}
}

#endif
