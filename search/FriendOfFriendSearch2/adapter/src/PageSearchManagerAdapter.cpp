#include "../include/PageSearchManagerAdapter.h"

NAMESPACE_HEADER

PageSearchManagerAdapter::PageSearchManagerAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

PageSearchManagerAdapter::~PageSearchManagerAdapter()
{
}

PageSearchManagerAdapter::ResultPtr PageSearchManagerAdapter::SearchIndex(
    const PageSearchManagerAdapter::Condition &condition, int begin, int limit)
{
  std::vector<Prx> proxies;
  proxies = locateAll<PageSearchManagerAdapter::Prx>(PageSearchManagerAdapter::instance().managers_, "PAGEIM", 0, MyUtil::TWO_WAY);
  int rand_index = rand() % proxies.size();
  return proxies.at(rand_index)->searchIndex(condition, begin, limit);
}

bool PageSearchManagerAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return PageSearchManagerAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


PageSearchManagerAdapter::Prx PageSearchManagerAdapter::getManagerOneway(int id) {
  return locate<PageSearchManagerAdapter::Prx> (this->managers_oneway_, "PAGEIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
