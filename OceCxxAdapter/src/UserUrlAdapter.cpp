/*
 * =====================================================================================
 *
 *       Filename:  UserBaseAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年09月16日 12时09分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#include "UserUrlAdapter.h"
#include "Date.h"
using namespace xce::adapter::userurl;

UserUrlManagerPrx UserUrlAdapter::getUserUrl10sManager(int id)
{
	return getProxy(id)->ice_timeout(10000);
}

UserUrlManagerPrx UserUrlAdapter::getUserUrlManager(int id)
{
	return getProxy(id);
}

UserUrlManagerPrx UserUrlAdapter::getUserUrlManagerOneway(int id)
{
	return getProxyOneway(id);
}


UserUrlInfoPtr UserUrlAdapter::getUserUrl(int userId, const Ice::Context& ctx) {
  UserUrlInfoPtr info = new UserUrlInfo;
  info->parse(getProxy(userId)->getUserUrl(userId, ctx));
	return info;
}

void UserUrlAdapter::setUserUrl(int id, const UserUrlInfoPtr& info) {
  if (id <= 0) {
    return;
  }
  if (info->id() != 0 && id != info->id()) {
    info->set_id(id);
  }
  getUserUrlManager(id)->setUserUrl(id, info->demark());
}

void UserUrlAdapter::createUserUrl(int id, const UserUrlInfoPtr& info) {
  if (id <= 0) {
    return;
  }
  getUserUrl10sManager(id)->createUserUrl(info->demark());
}

void UserUrlAdapter::reload(int id, const Ice::Context& ctx)
{
        return getUserUrlManager(id)->reload(id, ctx);
}
