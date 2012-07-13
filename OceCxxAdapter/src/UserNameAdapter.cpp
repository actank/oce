/*
 * =====================================================================================
 *
 *       Filename:  UserNameAdapter.cpp
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


/*
 * UserNameAdapter.cpp
 *
 */
#include "UserNameAdapter.h"
#include "Date.h"
using namespace xce::adapter::username;

UserNameManagerPrx UserNameAdapter::getUserName10sManager(int id)
{
	return getProxy(id)->ice_timeout(10000);
}

UserNameManagerPrx UserNameAdapter::getUserNameManager(int id)
{
	return getProxy(id);
}

UserNameManagerPrx UserNameAdapter::getUserNameManagerOneway(int id)
{
	return getProxyOneway(id);
}

void UserNameAdapter::reload(int id, const Ice::Context& ctx)
{
    getUserNameManager(id)->reload(id, ctx);
}

UserNameInfoPtr UserNameAdapter::getUserName(int uid, const Ice::Context& ctx) {
  UserNameInfoPtr info = new UserNameInfo;
  info->parse(getUserNameManager(uid)->getUserName(uid,ctx));
  return info;
}

void UserNameAdapter::setUserName(int id, const UserNameInfoPtr& umi) {
  if (id <= 0) {
    return;
  }
  if (umi->id() != 0 && id != umi->id()) {
    umi->set_id(id);
  }
  getUserNameManager(id)->setUserName(id, umi->demark());
}

void UserNameAdapter::createUserName(int userId, const UserNameInfoPtr& umi) {
  if (userId<=0) {
    return;
  }
  getUserName10sManager(userId)->createUserName(umi->demark());
}
