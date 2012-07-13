/*
 * =====================================================================================
 *
 *       Filename:  UserContactAdapter.cpp
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


#include "UserContactAdapter.h"
#include "UserContactReaderAdapter.cpp"
#include "UserContactWriterAdapter.cpp"

using namespace xce::adapter::usercontact;

UserContactDataPtr UserContactAdapter::getUserContact(int userId, const Ice::Context& ctx) {
	return UserContactReaderAdapter::instance().getUserContact(userId, ctx);
}

UserContactDataNPtr UserContactAdapter::getUserContactN(int userId, const Ice::Context& ctx) {
	return UserContactReaderAdapter::instance().getUserContactN(userId, ctx);
}

void UserContactAdapter::reload(int userId, const Ice::Context& ctx) {
	UserContactWriterAdapter::instance().reload(userId, ctx);
}

void UserContactAdapter::createUserContact(int userId, const UserContactInfoPtr& info) {
	UserContactWriterAdapter::instance().createUserContact(userId, info);
}

void UserContactAdapter::createUserContact(int userId, const MyUtil::Str2StrMap& props) {
	UserContactWriterAdapter::instance().createUserContact(userId, props);
}

void UserContactAdapter::setUserContact(int userid, const UserContactInfoPtr& info) {
	UserContactWriterAdapter::instance().setUserContact(userid, info);
}

void UserContactAdapter::setUserContact(int userId, const MyUtil::Str2StrMap& props) {
	UserContactWriterAdapter::instance().setUserContact(userId, props);
}
