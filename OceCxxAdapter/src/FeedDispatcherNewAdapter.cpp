#include "FeedDispatcherNewAdapter.h"

using namespace xce::feed;
using namespace MyUtil;

/*void
FeedDispatcherNewAdapter::deliverSupply(int supplier, int accepter, const Ice::Context& ctx){
	_managerOneway->deliverSupply(supplier, accepter, ctx);
}
*/

void FeedDispatcherNewAdapter::dispatch(const FeedSeedPtr& seed, const FeedConfigPtr& config)
{
	getManager(seed->actor)->dispatch(seed,config);
}


void FeedDispatcherNewAdapter::dispatchReply( const ReplyDataPtr& replyData,const FeedConfigPtr& config){
	getManager(replyData->actor)->dispatchReply(replyData,config);
}

void FeedDispatcherNewAdapter::InvalidateFactor(const string & factor){
  getManager(0)->InvalidateFactor(factor);
}
void FeedDispatcherNewAdapter::SetSendConfig(int uid, int stype ,
    int appid , bool newsconfig , bool miniconfig){
  getManager(0)->SetSendConfig(uid,stype,appid,newsconfig,miniconfig);
}
void FeedDispatcherNewAdapter::SetRecvConfig(int uid, int stype , int appid , bool newsconfig){
  getManager(0)->SetRecvConfig(uid,stype,appid,newsconfig);
}


void FeedDispatcherNewAdapter::addBuddyNotify(int host, int guest){

	getManagerOneway(host)->addBuddyNotify(host,guest);
}
        
void FeedDispatcherNewAdapter::joinPageNotify(int page, int fans){
	getManagerOneway(page)->joinPageNotify(page,fans);
}

FeedDispatcherNewPrx FeedDispatcherNewAdapter::getManager(int id) {
	return locate<FeedDispatcherNewPrx> (_managers, "FD", id, TWO_WAY);
}

FeedDispatcherNewPrx FeedDispatcherNewAdapter::getManagerOneway(int id) {
	return locate<FeedDispatcherNewPrx> (_managersOneway, "FD", id, ONE_WAY);
}
