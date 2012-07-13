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
  return getManager(0)->searchIndex(condition, begin, limit);
}

PeopleSearchManagerAdapter::Prx PeopleSearchManagerAdapter::getManager(int id) {
  return locate<PeopleSearchManagerAdapter::Prx> (this->managers_, "OPIM",
      id, MyUtil::TWO_WAY, this->timeout_);//->ice_connectionCached(false);
}

PeopleSearchManagerAdapter::Prx PeopleSearchManagerAdapter::getManagerOneway(int id) {
  return locate<PeopleSearchManagerAdapter::Prx> (this->managers_oneway_, "OPIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
