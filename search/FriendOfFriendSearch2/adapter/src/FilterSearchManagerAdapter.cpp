#include "../include/FilterSearchManagerAdapter.h"

NAMESPACE_HEADER

FilterSearchManagerAdapter::FilterSearchManagerAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

FilterSearchManagerAdapter::~FilterSearchManagerAdapter()
{
}

FilterSearchManagerAdapter::ResultPtr FilterSearchManagerAdapter::SearchIndex(
    const FilterSearchManagerAdapter::Condition &condition, int begin, int limit)
{
  std::vector<Prx> proxies;
  proxies = locateAll<FilterSearchManagerAdapter::Prx>(FilterSearchManagerAdapter::instance().managers_, "FSIM", 0, MyUtil::TWO_WAY);
  int rand_index = rand() % proxies.size();
  return proxies.at(rand_index)->searchIndex(condition, begin, limit);
}

bool FilterSearchManagerAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return FilterSearchManagerAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


FilterSearchManagerAdapter::Prx FilterSearchManagerAdapter::getManagerOneway(int id) {
  return locate<FilterSearchManagerAdapter::Prx> (this->managers_oneway_, "FSIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
