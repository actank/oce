#include "FeedMemcContentAdapter.h"

using namespace xce::feed;

void FeedMemcContentAdapter::Test(const LongSeq& ids){
	FeedMemcContentPrx::uncheckedCast(getManagerOneway(0)->ice_datagram())->Test(ids);
}

void FeedMemcContentAdapter::Test2(const IntSeq& ids){
	FeedMemcContentPrx::uncheckedCast(getManagerOneway(0)->ice_datagram())->Test2(ids);
}


