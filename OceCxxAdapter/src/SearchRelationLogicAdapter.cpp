#include "SearchRelationLogicAdapter.h"

SearchRelationLogicAdapter::SearchRelationLogicAdapter():MyUtil::ReplicatedClusterAdapterI<SearchRelationLogicPrx>("ControllerSearchRelationLogic", 120, 300)
{
}

SearchRelationLogicAdapter::~SearchRelationLogicAdapter()
{
}

bool SearchRelationLogicAdapter::pushFriendShip(int fromUserId, int toUserId, Operation op)
{
  SearchRelationLogicPrx proxy = getProxyOneway(0);
  if( proxy )  { 
    MCE_INFO(proxy);
    try  {
      proxy->pushFriendShip(fromUserId, toUserId, op);
    } catch (Ice::Exception& e) {
      MCE_INFO("SearchRelationLogicAdapter::update Ice::Exception " << e);
      return false;
    } catch (std::exception& e) {
      MCE_INFO("SearchRelationLogicAdapter::update std::exception " << e.what());
      return false;
    } catch (...) {
      MCE_INFO("SearchRelationLogicAdapter::update Unknown exception");
      return false;
    } 
  }  else  {
    MCE_WARN("proxy is not valid");
  }
  return true;
}



