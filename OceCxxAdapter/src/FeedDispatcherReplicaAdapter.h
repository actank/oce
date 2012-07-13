#ifndef FEEDDISPATCHER_REPLICA_ADAPTER_H_
#define FEEDDISPATCHER_REPLICA_ADAPTER_H_

#include "RFeed.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"


namespace xce {
namespace feed {

using namespace MyUtil;

class FeedDispatcherReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<FeedDispatcherPrx>,
    public MyUtil::Singleton<FeedDispatcherReplicaAdapter>{

public:
  FeedDispatcherReplicaAdapter() : MyUtil::ReplicatedClusterAdapterI<FeedDispatcherPrx> ("ControllerFeedDispatcherR",120,300,new XceFeedControllerChannel){
          }

	void dispatch(const FeedSeedPtr& seed, const FeedConfigPtr&);
	void dispatchReply( const ReplyDataPtr&,const FeedConfigPtr&);
	void addBuddyNotify(int host, int guest);
	void joinPageNotify(int page, int fans);
	void buddyApplyNotify(int host, int guest);

  void changeBuddyNotify(int host, int guest);

  void Reload(LoadType type, int id);
  void ReloadBatch(LoadType type, const vector<int> & ids);
protected:
  virtual size_t cluster() {
    return 10; 
//    return 1; 
  }

	FeedDispatcherPrx getManager(int id);
	FeedDispatcherPrx getManagerOneway(int id);
};

};
};

#endif /*BUDDYCOREADAPTER_H_*/
