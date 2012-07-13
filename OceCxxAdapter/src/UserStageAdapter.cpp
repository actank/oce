/*
 * UserStageAdapter.cpp
 *
 */
#include "UserStageAdapter.h"
#include "Date.h"
using namespace xce::adapter::userstage;

UserStageManagerPrx UserStageAdapter::getUserStageManager(int id)
{
	return getProxy(id);
}

UserStageManagerPrx UserStageAdapter::getUserStage10sManager(int id) {
	return getProxy(id)->ice_timeout(10000);
}

void UserStageAdapter::setUserStage(int id, const UserStageInfoPtr& ptr) {
	if(id <= 0) {
		return;
	}
	if(ptr->id() != 0 && id != ptr->id()) {
		ptr->set_id(id);
	}
	getUserStageManager(id)->setUserStage(id, ptr->demark());
}

void UserStageAdapter::createUserStage(int id, const UserStageInfoPtr& ptr) {
	if(id <= 0) {
		return;
	}
	getUserStage10sManager(id)->createUserStage(ptr->demark());		
}

UserStageInfoPtr UserStageAdapter::getUserStage(int id, const Ice::Context& ctx){
	UserStageInfoPtr info = new UserStageInfo;
	info->parse(getUserStageManager(id)->getUserStage(id, ctx));
	return info;
}
void UserStageAdapter::reload(int id, const Ice::Context& ctx) {
	return getUserStageManager(id)->reloadUserStage(id, ctx);
}
