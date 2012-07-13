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


#include "UserRightAdapter.h"
#include "Date.h"
using namespace xce::adapter::userright;

UserRightInfoPtr UserRightAdapter::getUserRight(int userId, const Ice::Context& ctx) {
	UserRightInfoPtr info = new UserRightInfo;	
	info->parse(getUserRight10sManager(userId)->getUserRight(userId, ctx));
	return info;
}

UserRightManagerPrx UserRightAdapter::getUserRightManager(int id)
{
	return getProxy(id);
}

UserRightManagerPrx UserRightAdapter::getUserRightManagerOneway(int id)
{
	return getProxyOneway(id);
}

UserRightManagerPrx UserRightAdapter::getUserRight10sManager(int id) {
	return getProxy(id)->ice_timeout(10000);
}
void UserRightAdapter::setUserRight(int id, const UserRightInfoPtr& ptr) {
	if(id <= 0) {
		return;
	}
	if(ptr->id() != 0 && id != ptr->id()) {
		ptr->set_id(id);
	}
	getUserRightManager(id)->setUserRight(id, ptr->demark());
}

void UserRightAdapter::createUserRight(int id, const UserRightInfoPtr& ptr) {
	if(id <= 0) {
		return;
	}
	getUserRight10sManager(id)->createUserRight(ptr->demark());
}	
	
void UserRightAdapter::reload(int id, const Ice::Context& ctx) {
	return getUserRightManager(id)->reload(id, ctx);
}
