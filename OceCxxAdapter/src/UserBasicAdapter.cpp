/*
 * UserBasicAdapter.cpp
 *
 */
#include "UserBasicAdapter.h"
#include "Date.h"
using namespace xce::adapter::userbasic;

UserBasicManagerPrx UserBasicAdapter::getUserBasicManager(int id)
{
  return getProxy(id);
}

UserBasicManagerPrx UserBasicAdapter::getUserBasicManagerOneway(int id)
{
  return getProxyOneway(id);
}

UserBasicDataPtr UserBasicAdapter::getUserBasic(int id, const Ice::Context& ctx)
{
        return getUserBasicManager(id)->getUserBasic(id, ctx);
}
