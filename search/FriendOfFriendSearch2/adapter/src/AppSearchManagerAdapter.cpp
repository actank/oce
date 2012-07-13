#include "../include/AppSearchManagerAdapter.h"

NAMESPACE_HEADER

AppSearchManagerAdapter::AppSearchManagerAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

AppSearchManagerAdapter::~AppSearchManagerAdapter()
{
}

AppSearchManagerAdapter::ResultPtr AppSearchManagerAdapter::SearchIndex(
    const AppSearchManagerAdapter::Condition &condition, int begin, int limit)
{
  std::vector<Prx> proxies;
  proxies = locateAll<AppSearchManagerAdapter::Prx>(AppSearchManagerAdapter::instance().managers_, "APPIM", 0, MyUtil::TWO_WAY);
  int rand_index = rand() % proxies.size();
  return proxies.at(rand_index)->searchIndex(condition, begin, limit);
}

bool AppSearchManagerAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return AppSearchManagerAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


AppSearchManagerAdapter::Prx AppSearchManagerAdapter::getManagerOneway(int id) {
  return locate<AppSearchManagerAdapter::Prx> (this->managers_oneway_, "APPIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
