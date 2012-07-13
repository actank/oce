#include "../include/PeopleSearchManagerAdapter.h"

NAMESPACE_HEADER

PeopleSearchManagerAdapter::PeopleSearchManagerAdapter(int timeout)
{
  this->managers_.resize(1);
  this->managers_oneway_.resize(1);
  this->timeout_ = timeout;
}

PeopleSearchManagerAdapter::~PeopleSearchManagerAdapter()
{
}

PeopleSearchManagerAdapter::ResultPtr PeopleSearchManagerAdapter::SearchIndex(
    const PeopleSearchManagerAdapter::Condition &condition, int begin, int limit)
{
  std::vector<Prx> proxies;
  proxies = locateAll<PeopleSearchManagerAdapter::Prx>(PeopleSearchManagerAdapter::instance().managers_, "OPIM", 0, MyUtil::TWO_WAY);
  int rand_index = rand() % proxies.size();
  return proxies.at(rand_index)->searchIndex(condition, begin, limit);
}

bool PeopleSearchManagerAdapter::isValid(const Ice::ObjectPrx& proxy)  {
  //return PeopleSearchManagerAdapter::Prx::uncheckedCast(proxy)->isValid();
  return true;
}


PeopleSearchManagerAdapter::Prx PeopleSearchManagerAdapter::getManagerOneway(int id) {
  return locate<PeopleSearchManagerAdapter::Prx> (this->managers_oneway_, "OPIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
