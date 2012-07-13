#include "FeedSortAdapter.h"

using namespace xce::feed;


FeedSortAdapter::FeedSortAdapter()
{
	_managers.resize(cluster() > 0 ? cluster() : 1);
	_managersOneway.resize(cluster() > 0 ? cluster() : 1);
}

FeedSortAdapter::~FeedSortAdapter()
{

}

FeedSortPrx FeedSortAdapter::getManager(int id,int timeout)
{
	return locate<FeedSortPrx> (_managers, "M", id, TWO_WAY,timeout);
}

FeedSortPrx FeedSortAdapter::getManagerOneway(int id)
{
	return locate<FeedSortPrx> (_managersOneway, "M", id, ONE_WAY);
}

