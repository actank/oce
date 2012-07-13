#include "FeedDispatcherReplicaAdapter.h"

using namespace xce::feed;
using namespace MyUtil;

/*void
FeedDispatcherReplicaAdapter::deliverSupply(int supplier, int accepter, const Ice::Context& ctx){
	_managerOneway->deliverSupply(supplier, accepter, ctx);
}
*/

void FeedDispatcherReplicaAdapter::dispatch(const FeedSeedPtr& seed, const FeedConfigPtr& config)
{
	getManager(seed->actor)->dispatch(seed,config);
}


void FeedDispatcherReplicaAdapter::dispatchReply( const ReplyDataPtr& replyData,const FeedConfigPtr& config){
	getManager(replyData->actor)->dispatchReply(replyData,config);
}


void FeedDispatcherReplicaAdapter::addBuddyNotify(int host, int guest){
	getManagerOneway(host)->addBuddyNotify(host,guest);
}
        
void FeedDispatcherReplicaAdapter::changeBuddyNotify(int host, int guest){
	getManagerOneway(host)->Reload(Friend, host);
	getManagerOneway(guest)->Reload(Friend, guest);
}

void FeedDispatcherReplicaAdapter::buddyApplyNotify(int host, int guest){
	getManagerOneway(guest)->buddyApplyNotify(host, guest);
}

void FeedDispatcherReplicaAdapter::joinPageNotify(int page, int fans){
	getManagerOneway(page)->joinPageNotify(page,fans);
}

void FeedDispatcherReplicaAdapter::Reload(LoadType type, int id){
  for(unsigned  i = 0; i != cluster(); ++i){
    getManagerOneway(i)->Reload(type,id);
  }
}
void FeedDispatcherReplicaAdapter::ReloadBatch(LoadType type, const vector<int> & ids){
  for(unsigned  i = 0; i != cluster(); ++i){
    getManagerOneway(i)->ReloadBatch(type,ids);
  }
}

FeedDispatcherPrx FeedDispatcherReplicaAdapter::getManager(int id) {
  return getProxy(id);
	//return locate<FeedDispatcherPrx> (_managers, "FD", id, TWO_WAY);
}

FeedDispatcherPrx FeedDispatcherReplicaAdapter::getManagerOneway(int id) {
  return getProxyOneway(id);
	//return locate<FeedDispatcherPrx> (_managersOneway, "FD", id, ONE_WAY);
}
