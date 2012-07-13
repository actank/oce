/*
 * UserBornAdapter.cpp
 *
 */
#include "UserBornAdapter.h"
#include "Date.h"
using namespace xce::adapter::userborn;

UserBornManagerPrx UserBornAdapter::getUserBorn10sManager(int id)
{
	return getProxy(id)->ice_timeout(10000);
}
UserBornManagerPrx UserBornAdapter::getUserBornManager(int id)
{
	return getProxy(id);
}

UserBornInfoPtr UserBornAdapter::getUserBorn(int id, const Ice::Context& ctx)
{
  UserBornInfoPtr info = new UserBornInfo;
  info->parse(getUserBornManager(id)->getUserBorn(id, ctx));
	return info;
}

void UserBornAdapter::setUserBorn(int id, const UserBornInfoPtr& info) {
  if (id <= 0) {
    return;
  }
  if (info->id() != 0 && id != info->id()) {
    info->set_id(id);
  }
  getUserBornManager(id)->setUserBorn(id, info->demark());
}

void UserBornAdapter::createUserBorn(int id, const UserBornInfoPtr& info) {
  if (id <= 0) {
    return;
  }
  getUserBorn10sManager(id)->createUserBorn(info->demark());
}
void UserBornAdapter::reload(int id) {
  if (id <= 0) {
    return;
  }
  getUserBorn10sManager(id)->reloadUserBorn(id);
}
