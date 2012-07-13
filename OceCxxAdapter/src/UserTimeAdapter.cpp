/*
 * =====================================================================================
 *
 *       Filename:  UserTimeAdapter.cpp
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


#include "UserTimeAdapter.h"
#include "UserTimeReaderAdapter.cpp"
#include "UserTimeWriterAdapter.cpp"

using namespace xce::adapter::usertime;

UserTimeInfoPtr UserTimeAdapter::getUserTime(int userId, const Ice::Context& ctx) {
	return UserTimeReaderAdapter::instance().getUserTime(userId, ctx);
}

void UserTimeAdapter::reload(int userId, const Ice::Context& ctx) {
	UserTimeWriterAdapter::instance().reload(userId);
}

void UserTimeAdapter::setUserTime(int userId, const UserTimeInfoPtr& info) {
	UserTimeWriterAdapter::instance().setUserTime(userId, info->demark());
}

void UserTimeAdapter::setUserTime(int userId, const MyUtil::Str2StrMap& props, const Ice::Context& ctx) {
	UserTimeWriterAdapter::instance().setUserTime(userId, props);
}

void UserTimeAdapter::createUserTime(int userId, const UserTimeInfoPtr& info) {
	UserTimeWriterAdapter::instance().createUserTime(userId, info->demark());
}

void UserTimeAdapter::createUserTime(int userId, const MyUtil::Str2StrMap& props) {
	UserTimeWriterAdapter::instance().createUserTime(userId, props);
}
