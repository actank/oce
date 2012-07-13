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


#include "UserStateAdapter.h"
#include "Date.h"
using namespace xce::adapter::userstate;

void UserStateInfo::parse(const UserStateDataPtr& data) {
  id_ = data->id;
  state_ = data->state;
  star_ = data->star;
  level_ = data->level;
}

UserStateManagerPrx UserStateAdapter::getUserState10sManager(int id)
{
	return getProxy(id)->ice_timeout(10000);
}

UserStateManagerPrx UserStateAdapter::getUserStateManager(int id)
{
	return getProxy(id);
}

UserStateManagerPrx UserStateAdapter::getUserStateManagerOneway(int id)
{
	return getProxyOneway(id);
}

void UserStateAdapter::reload(int id, const Ice::Context& ctx)
{
        return getUserStateManager(id)->reload(id, ctx);
}

void UserStateAdapter::setKeepUseOn(int userid) {
	getUserStateManager(userid)->setStateOn(userid, CStateKEEPUSE);
}

void UserStateAdapter::setKeepUseOff(int userid) {
	getUserStateManager(userid)->setStateOff(userid, CStateKEEPUSE);
}

void UserStateAdapter::setActiveOn(int userid) {
	getUserStateManager(userid)->setStateOn(userid, CStateACTIVE);
}

void UserStateAdapter::setActiveOff(int userid) {
	getUserStateManager(userid)->setStateOff(userid, CStateACTIVE);
}

void UserStateAdapter::setZombieOn(int userid) {
	getUserStateManager(userid)->setStateOn(userid, CStateZOMBIE);
}

void UserStateAdapter::setZombieOff(int userid) {
	getUserStateManager(userid)->setStateOff(userid, CStateZOMBIE);
}

UserStateInfoPtr UserStateAdapter::getUserState(int userid, const Ice::Context& ctx) {
  UserStateInfoPtr info = new UserStateInfo;
	UserStateDataPtr data = getUserStateManager(userid)->getUserState(userid, ctx);
  if (data) {
    info->parse(data);
  }
  return info;
}

void UserStateAdapter::setUserState(int id, const UserStateInfoPtr& info) {
  if (id<=0) {
    return;
  }
  if (info->id()!=0&&id != info->id()) {
    info->set_id(id);
  }
  getUserStateManager(id)->setUserState(id, info->demark());
}

void UserStateAdapter::createUserState(int userId, const UserStateInfoPtr& props) {
  if (userId <= 0) {
    return;
  }
  getUserState10sManager(userId)->createUserState(props->demark());
}
