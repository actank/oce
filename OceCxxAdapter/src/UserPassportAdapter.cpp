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


#include "UserPassportAdapter.h"
#include "Date.h"
using namespace xce::adapter::userpassport;

UserPassportManagerPrx UserPassportAdapter::getUserPassportManager(int id)
{
	return getProxy(id);
}

UserPassportManagerPrx UserPassportAdapter::getUserPassport10sManager(int id)
{
	return getProxy(id)->ice_timeout(10000);
}
UserPassportManagerPrx UserPassportAdapter::getUserPassportManagerOneway(int id)
{
	return getProxyOneway(id);
}

void UserPassportAdapter::reload(int id, const Ice::Context& ctx)
{
     getUserPassportManager(id)->reload(id, ctx);
}

void UserPassportAdapter::createUserPassport(int userId, const UserPassportInfoPtr& ptr) {
	if(userId <= 0) {
		return;
	}
	getUserPassport10sManager(userId)->createUserPassport(ptr->demark());
}

void UserPassportAdapter::setUserPassport(int id, const UserPassportInfoPtr& ptr) {
	if(id <= 0) {
		return;
	}
	if(ptr->id() != 0 && ptr->id() != id) {
		ptr->set_id(id);
	}
	getUserPassportManager(id)->setUserPassport(id, ptr->demark());
}
UserPassportInfoPtr UserPassportAdapter::getUserPassport(int uid, const Ice::Context& ctx){
	UserPassportInfoPtr ptr = new UserPassportInfo;
	ptr->parse(getUserPassportManager(uid)->getUserPassport(uid, ctx));
	return ptr;
}
