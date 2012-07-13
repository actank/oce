#include "UserConfigM2SAdapter.h"

using com::xiaonei::xce::Statement;
using com::xiaonei::xce::CDbWServer;
using com::xiaonei::xce::QueryRunner;
using mop::hi::svc::model::CreateHandle;
using mop::hi::svc::model::SetHandle;
using mop::hi::svc::model::UserConfigResultHandlerI;
using mop::hi::svc::model::UserConfigM2SAdapter;
using callguarantee::GuaranteeHandlePtr;

CreateHandle::CreateHandle(int userId, const std::vector<UserConfigSlavePrx>& prxs) : userId_(userId), prxs_(prxs), data_(0) {
	Statement sql;
	sql << "SELECT * FROM user_config WHERE id = '" << userId_ << "'";
	UserConfigDataPtr data = new UserConfigData;
	UserConfigResultHandlerI handler(data);
	size_t resultSize = 0;
	try {
		resultSize = QueryRunner(DB_USER_CONFIG , CDbWServer).query(sql, handler);
	} catch (...) {
		MCE_WARN("CreateHandle::CreateHandle for "<<userId_);
	}
	if( resultSize > 0) {
		data_ = data;
	}
}

bool CreateHandle::doHandle() {
	if (data_ == 0) {
		MCE_WARN("CreateHandle::doHandle data is NULL for "<<userId_);
		return true;
	}
	try {
		for(std::vector<UserConfigSlavePrx>::const_iterator it = prxs_.begin(); it != prxs_.end(); ++it) {
			(*it)->createUserConfig(data_);
		}
	} catch (const Ice::Exception& e) {
		MCE_WARN("CreateHandle::doHandle Ice::Exception: "<<e);
		return false;
	} catch (...) {
		MCE_WARN("CreateHandle::doHandle unknown exception");
		return false;
	}
	return true;
}

bool CreateHandle::reDoHandle() {
	return doHandle();
}

bool SetHandle::doHandle() {

	try {
		for(std::vector<UserConfigSlavePrx>::const_iterator it = _prxs.begin();it != _prxs.end(); ++it) {
			(*it)->setUserConfig(_userId, _props);
		}
	} catch (const Ice::Exception& e) {
		MCE_WARN("SetHandle::doHandle Ice::Exception: "<<e());
		return false;
	} catch (...) {
		MCE_WARN("SetHandle::doHandle unknown exception");
		return false;
	}
	return true;
}

bool SetHandle::reDoHandle() {
	return doHandle();
}

void SetHandle::merge(const GuaranteeHandlePtr& newData) {
        SetHandlePtr sourceData = SetHandlePtr::dynamicCast(newData);
	for(MyUtil::Str2StrMap::const_iterator it = sourceData->_props.begin(); it != sourceData->_props.end(); ++ it) {
		_props[it->first] = it->second;
	}
}

bool UserConfigResultHandlerI::handle(mysqlpp::Row& row) const {
	_props->userId = (int)(row[CUCUSERID.c_str()]);
	_props->basicConfig = (int)(row[CUCBASICCONFIG.c_str()]);
	_props->pokeConfig = (int)(row[CUCPOKECONFIG.c_str()]);
	_props->requestFriendConfig = (int)(row[CUCREQUESTFRIENDCONFIG.c_str()]);
	_props->photoConfig = (int)(row[CUCPHOTOCONFIG.c_str()]);
	_props->messageConfig = (int)(row[CUCMESSAGECONFIG.c_str()]);
	_props->emailConfig = (long)(row[CUCEMAILCONFIG.c_str()]);
	_props->browseConfig = (int)(row[CUCBROWSECONFIG.c_str()]);
	_props->wantSeeCss = (short)(row[CUCWANTSEECSS.c_str()]);
	_props->statusConfig = (int)(row[CUCSTATUSCONFIG.c_str()]);
#ifndef NEWARCH
	_props->displayMenuList = row[CUCDISPLAYMENULIST.c_str()].get_string();
	_props->moreMenuList = row[CUCMOREMENULIST.c_str()].get_string();
	_props->sendFeedConfig = row[CUCSENDFEEDCONFIG.c_str()].get_string();
	_props->recvFeedConfig = row[CUCRECVFEEDCONFIG.c_str()].get_string();
	_props->profilePrivacy = row[CUCPROFILEPRIVACY.c_str()].get_string();
#else
	_props->displayMenuList = row[CUCDISPLAYMENULIST.c_str()].data();
	_props->moreMenuList = row[CUCMOREMENULIST.c_str()].data();
	_props->sendFeedConfig = row[CUCSENDFEEDCONFIG.c_str()].data();
	_props->recvFeedConfig = row[CUCRECVFEEDCONFIG.c_str()].data();
	_props->profilePrivacy = row[CUCPROFILEPRIVACY.c_str()].data();
#endif
	return true;
}

void UserConfigM2SAdapter::createUserConfig(int userId)
{
	DoInTime(userId, new CreateHandle(userId, getAllProxySeq(userId)));
}

void UserConfigM2SAdapter::setUserConfig(int userId, const MyUtil::Str2StrMap& props)
{
	DoInTime(userId, new SetHandle(userId, props, getAllProxySeq(userId)));
}
