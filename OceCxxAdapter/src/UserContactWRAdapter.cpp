#include "UserContactWRAdapter.h"

using namespace xce::adapter::usercontact;

UserContactDataPtr UserContactWRAdapter::getUserContact(int userId, const Ice::Context& ctx) {
	return UserContactReaderAdapter::instance().getUserContact(userId, ctx);
}

void UserContactWRAdapter::reload(int userId, const Ice::Context& ctx) {
	UserContactWriterAdapter::instance().reload(userId, ctx);
}

void UserContactWRAdapter::createUserContact(int userId, const UserContactInfoPtr& info) {
	UserContactWriterAdapter::instance().createUserContact(userId, info);
}

void UserContactWRAdapter::createUserContact(int userId, const MyUtil::Str2StrMap& props) {
	UserContactWriterAdapter::instance().createUserContact(userId, props);
}

void UserContactWRAdapter::setUserContact(int userid, const UserContactInfoPtr& info) {
	UserContactWriterAdapter::instance().setUserContact(userid, info);
}

void UserContactWRAdapter::setUserContact(int userId, const MyUtil::Str2StrMap& props) {
	UserContactWriterAdapter::instance().setUserContact(userId, props);
}

