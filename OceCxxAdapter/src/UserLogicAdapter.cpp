#include "UserLogicAdapter.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "Date.h"

using namespace mop::hi::svc::adapter;
using namespace mop::hi::svc::model;
using namespace std;

//---------------------------------------------------------------------------
/*void UserLogicAdapter::initialize() {
	_userLogicManager = locate<UserLogicManagerPrx>("M", TWO_WAY);
	_userLogicManagerOneway = locate<UserLogicManagerPrx>("M", ONE_WAY);
}*/

//---------------------------------------------------------------------------

void UserLogicAdapter::updateUser(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUser(hostId, props, ctx);
}

void UserLogicAdapter::updateUserDesc(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserDesc(hostId, props, ctx);
}

void UserLogicAdapter::updateUserNetwork(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserNetwork(hostId, props, ctx);
}

void UserLogicAdapter::insertUserDoing(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserDoing(hostId, props, ctx);
}

void UserLogicAdapter::updateUserDoing(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserDoing(hostId, props, ctx);
}

void UserLogicAdapter::insertUserBorn(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserBorn(hostId, props, ctx);
}

void UserLogicAdapter::updateUserBorn(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserBorn(hostId, props, ctx);
}

void UserLogicAdapter::insertUserStage(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserStage(hostId, props, ctx);
}

void UserLogicAdapter::updateUserStage(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserStage(hostId, props, ctx);
}

void UserLogicAdapter::insertUserState(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserState(hostId,props,ctx);
}

void UserLogicAdapter::updateUserState(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserState(hostId,props,ctx);
}

void UserLogicAdapter::updateUserNames(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserNames(hostId,props,ctx);
}

void UserLogicAdapter::insertUserNames(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserNames(hostId,props,ctx);
}

void UserLogicAdapter::updateUserPassport(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserPassport(hostId,props,ctx);
}

void UserLogicAdapter::insertUserPassport(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserPassport(hostId,props,ctx);
}

void UserLogicAdapter::updateUserConfig(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserConfig(hostId,props,ctx);
}

void UserLogicAdapter::insertUserConfig(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserConfig(hostId,props,ctx);
}

void UserLogicAdapter::updateUserUrl(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserUrl(hostId,props,ctx);
}

void UserLogicAdapter::insertUserUrl(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserUrl(hostId,props,ctx);
}

void UserLogicAdapter::updateUserTime(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserTime(hostId,props,ctx);
}

void UserLogicAdapter::insertUserTime(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserTime(hostId,props,ctx);
}

void UserLogicAdapter::updateUserContact(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserContact(hostId,props,ctx);
}

void UserLogicAdapter::insertUserContact(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserContact(hostId,props,ctx);
}

void UserLogicAdapter::updateUserFav(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserFav(hostId,props,ctx);
}

void UserLogicAdapter::insertUserFav(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserFav(hostId,props,ctx);
}

void UserLogicAdapter::updateUserRight(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserRight(hostId,props,ctx);
}

void UserLogicAdapter::insertUserRight(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->insertUserRight(hostId,props,ctx);
}

void UserLogicAdapter::updateUserRegion(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserRegion(hostId,props,ctx);
}

void UserLogicAdapter::updateUserWorkplace(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserWorkplace(hostId,props,ctx);
}

void UserLogicAdapter::updateUserElementarySchool(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserElementarySchool(hostId,props,ctx);
}

void UserLogicAdapter::updateUserJuniorHighSchool(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserJuniorHighSchool(hostId,props,ctx);
}

void UserLogicAdapter::updateUserHighSchool(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserHighSchool(hostId,props,ctx);
}

void UserLogicAdapter::updateUserCollege(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserCollege(hostId,props,ctx);
}

void UserLogicAdapter::updateUserUniversity(int hostId, const Str2StrMap& props,
		const Str2StrMap& ctx) {
	getProxyOneway(0)->updateUserUniversity(hostId,props,ctx);
}
