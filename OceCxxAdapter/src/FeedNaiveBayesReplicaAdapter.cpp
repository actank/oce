#include "FeedNaiveBayesReplicaAdapter.h"

using namespace xce::feed;


//FeedNaiveBayesAdapter::FeedNaiveBayesAdapter()
//{
//	_managers.resize(cluster() > 0 ? cluster() : 1);
//	_managersOneway.resize(cluster() > 0 ? cluster() : 1);
//}

FeedNaiveBayesAdapter::~FeedNaiveBayesAdapter()
{

}

FeedNaiveBayesPrx FeedNaiveBayesAdapter::getManager(int id)
{
  return getProxy(id);
//	return locate<FeedNaiveBayesPrx> (_managers, "M", id, TWO_WAY);
}

FeedNaiveBayesPrx FeedNaiveBayesAdapter::getManagerOneway(int id)
{
  return getProxyOneway(id);
//	return locate<FeedNaiveBayesPrx> (_managersOneway, "M", id, ONE_WAY);
}

