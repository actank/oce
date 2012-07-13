#include "../include/ExternalPeopleSearchManagerAdapter.h"

NAMESPACE_HEADER

ExternalPeopleSearchManagerAdapter::ExternalPeopleSearchManagerAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

ExternalPeopleSearchManagerAdapter::~ExternalPeopleSearchManagerAdapter()
{
}

ExternalPeopleSearchManagerAdapter::ResultPtr ExternalPeopleSearchManagerAdapter::SearchIndex(
    const ExternalPeopleSearchManagerAdapter::Condition &condition, int begin, int limit)
{
  std::vector<Prx> proxies;
  proxies = locateAll<ExternalPeopleSearchManagerAdapter::Prx>(ExternalPeopleSearchManagerAdapter::instance().managers_, "EPSIM", 0, MyUtil::TWO_WAY);
  int rand_index = rand() % proxies.size();
  return proxies.at(rand_index)->searchIndex(condition, begin, limit);
}

bool ExternalPeopleSearchManagerAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return ExternalPeopleSearchManagerAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


ExternalPeopleSearchManagerAdapter::Prx ExternalPeopleSearchManagerAdapter::getManagerOneway(int id) {
  return locate<ExternalPeopleSearchManagerAdapter::Prx> (this->managers_oneway_, "EPSIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
