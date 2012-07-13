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


#include "UserLoginCountAdapter.h"
#include "Date.h"
#include <boost/lexical_cast.hpp>
using namespace xce::adapter::userlogincount;

UserLoginCountManagerPrx UserLoginCountAdapter::getUserLoginCount10sManager(int id)
{
	return getProxy(id)->ice_timeout(10000);
}

UserLoginCountManagerPrx UserLoginCountAdapter::getUserLoginCountManager(int id)
{
	return getProxy(id);
}

UserLoginCountManagerPrx UserLoginCountAdapter::getUserLoginCountManagerOneway(int id)
{
	return getProxyOneway(id);
}

int UserLoginCountAdapter::getUserLoginCount(int userId, const Ice::Context& ctx) {
	return getProxy(userId)->getUserLoginCount(userId);
}

void UserLoginCountAdapter::createUserLoginCount(int userid) {
  if (userid<=0) {
    return;
  }
  std::map<std::string, std::string> m;
  m.insert(make_pair<std::string, std::string>("ID", boost::lexical_cast<std::string>(userid)));
  getUserLoginCount10sManager(userid)->createUserLoginCount(m);
}

void UserLoginCountAdapter::incUserLoginCount(int userid) {
  if (userid <= 0) {
    return;
  }
  getUserLoginCountManager(userid)->incUserLoginCount(userid);
}

void UserLoginCountAdapter::reload(int id, const Ice::Context& ctx)
{
	return getUserLoginCountManager(id)->reload(id, ctx);
}
