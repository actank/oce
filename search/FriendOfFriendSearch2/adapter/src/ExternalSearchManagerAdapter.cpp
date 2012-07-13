#include "../include/ExternalSearchManagerAdapter.h"

NAMESPACE_HEADER

ExternalSearchManagerAdapter::ExternalSearchManagerAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

ExternalSearchManagerAdapter::~ExternalSearchManagerAdapter()
{
}

ExternalSearchManagerAdapter::ResultPtr ExternalSearchManagerAdapter::SearchIndex(
    const ExternalSearchManagerAdapter::Condition &condition, int begin, int limit)
{
  std::vector<Prx> proxies;
  proxies = locateAll<ExternalSearchManagerAdapter::Prx>(ExternalSearchManagerAdapter::instance().managers_, "ESIM", 0, MyUtil::TWO_WAY);
  int rand_index = rand() % proxies.size();
  return proxies.at(rand_index)->searchIndex(condition, begin, limit);
}

bool ExternalSearchManagerAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return ExternalSearchManagerAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


ExternalSearchManagerAdapter::Prx ExternalSearchManagerAdapter::getManagerOneway(int id) {
  return locate<ExternalSearchManagerAdapter::Prx> (this->managers_oneway_, "ESIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
