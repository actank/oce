#include "FeedQueueAdapter.h"

using namespace xce::feed;


FeedQueueAdapter::FeedQueueAdapter()
{
	_managers.resize(cluster() > 0 ? cluster() : 1);
	_managersOneway.resize(cluster() > 0 ? cluster() : 1);
}

FeedQueueAdapter::~FeedQueueAdapter()
{

}

FeedQueuePrx FeedQueueAdapter::getManager(int id)
{
	return locate<FeedQueuePrx> (_managers, "M", id, TWO_WAY);
}

FeedQueuePrx FeedQueueAdapter::getManagerOneway(int id)
{
	return locate<FeedQueuePrx> (_managersOneway, "M", id, ONE_WAY);
}
int FeedQueueAdapter::Push(int index,const SqlInfoPtr & sqlinfo){
  FeedQueuePrx::uncheckedCast(getManagerTwoway(index)->ice_datagram())->Push(sqlinfo);
  return 1;
//  return getManager(index)->Push(sqlinfo);
}
