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
  return getManager(0)->searchIndex(condition, begin, limit);
}

SearchManagerAdapter::Prx SearchManagerAdapter::getManager(int id) {
  return locate<SearchManagerAdapter::Prx> (this->managers_, "OIM",
      id, MyUtil::TWO_WAY, this->timeout_);//->ice_connectionCached(false);
}

SearchManagerAdapter::Prx SearchManagerAdapter::getManagerOneway(int id) {
  return locate<SearchManagerAdapter::Prx> (this->managers_oneway_, "OIM",
      id, MyUtil::ONE_WAY, this->timeout_);
}

NAMESPACE_FOOTER
