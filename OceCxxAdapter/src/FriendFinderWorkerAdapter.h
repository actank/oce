#ifndef _FRIENDFINDER_WORKER_ADAPTER_H_
#define _FRIENDFINDER_WORKER_ADAPTER_H_

#include "SocialGraphWorker.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace socialgraph {

class FriendFinderWorkerAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphWorkerPrx>, 
		public MyUtil::Singleton<FriendFinderWorkerAdapter> {
public:
	FriendFinderWorkerAdapter():
		MyUtil::ReplicatedClusterAdapterI<xce::socialgraph::SocialGraphWorkerPrx>("ControllerFriendFinderWorker", 120, 300) {
	}

	void Report(int host_id, const Info& information);
	void RebuildCommonFriend(int host_id);
	void RemoveCommonFriend(int host_id, int friend_id);
};

}
}

#endif 
