/*
 * =====================================================================================
 *
 *       Filename:  UserDoingAdapter.cpp
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


#include "UserDoingAdapter.h"
#include "Date.h"
using namespace xce::adapter::userdoing;

UserDoingManagerPrx UserDoingAdapter::getUserDoingManager(int id)
{
	return getProxy(id);
}

UserDoingManagerPrx UserDoingAdapter::getUserDoing10sManager(int id)
{
    return getProxy(id)->ice_timeout(10000);
}


UserDoingManagerPrx UserDoingAdapter::getUserDoingManagerOneway(int id)
{
	return getProxyOneway(id);
}

UserDoingInfoPtr UserDoingAdapter::getUserDoing(int uid, const Ice::Context& ctx) {
  UserDoingInfoPtr info = new UserDoingInfo;
  info->parse(getUserDoingManager(uid)->getUserDoing(uid, ctx));
  return info;
}

void UserDoingAdapter::setUserDoing(int id, const UserDoingInfoPtr& umi) {
	if (id <= 0) {
    	return;
	}
	if (umi->id() != 0 && id != umi->id()) {
		umi->set_id(id);
	}
	getUserDoingManager(id)->setUserDoing(id, umi->demark());
}

void UserDoingAdapter::createUserDoing(int userId, const UserDoingInfoPtr& umi) {
	if(userId<=0) {
    	return;
	}
	getUserDoing10sManager(userId)->createUserDoing(umi->demark());
}
void UserDoingAdapter::reload(int userId) {
	if(userId<=0) {
    	return;
	}
	getUserDoing10sManager(userId)->reloadUserDoing(userId);
}
