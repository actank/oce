#include "../include/SearchManagerAdapter.h"

NAMESPACE_HEADER

SearchManagerAdapter::SearchManagerAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

SearchManagerAdapter::~SearchManagerAdapter()
{
}

SearchManagerAdapter::ResultPtr SearchManagerAdapter::SearchIndex(
    const SearchManagerAdapter::Condition &condition, int begin, int limit)
{
  std::vector<Prx> proxies;
  proxies = locateAll<SearchManagerAdapter::Prx>(SearchManagerAdapter::instance().managers_, "OIM", 0, MyUtil::TWO_WAY);
  int rand_index = rand() % proxies.size();
  return proxies.at(rand_index)->searchIndex(condition, begin, limit);
}

bool SearchManagerAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return SearchManagerAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


SearchManagerAdapter::Prx SearchManagerAdapter::getManagerOneway(int id) {
  return locate<SearchManagerAdapter::Prx> (this->managers_oneway_, "OIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
