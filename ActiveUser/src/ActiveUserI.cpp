#include "ActiveUserI.h"
#include "ConnectionQuery.h"

using namespace mop::hi::svc::model;
using namespace MyUtil;
using namespace com::xiaonei::xce;

//=============================================================================
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(new ActiveUserManagerI, service.createIdentity(ACTIVE_USERER, ""));
}
;

//=============================================================================
ActiveUserManagerI::ActiveUserManagerI() {
	ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	//	_database = props->getPropertyWithDefault(
	//		"Service."+getName()+".Database", "fb_active_user");
	_table = props->getPropertyWithDefault("Service."+service.getName()+".Table",
			"user_desc_ab");
	int duration = props->getPropertyAsIntWithDefault("Service."+service.getName()
			+".Duration", 60*60*24*7);	
	_activeUserIds.setDuration(duration);
	
	_minSize = props->getPropertyAsIntWithDefault("Service."+service.getName()
				+".MinSize", 3000000);
	
	TaskManager::instance().execute(new LoadActiveUsersTask(_activeUserIds, _table));
}

ActiveUserManagerI::~ActiveUserManagerI() {

}

Int2IntMap ActiveUserManagerI::checkActiveUsers(const IntSeq& userIds,
		const Ice::Current&) {
	Int2IntMap result;
	for (uint i = 0; i < userIds.size(); ++i) {
		if (_activeUserIds.size() < _minSize
				|| _activeUserIds.has(userIds.at(i))) {
			result[userIds.at(i)] = 1;
		} else {
			result[userIds.at(i)] = 0;
		}
	}
	return result;
}

IntSeq ActiveUserManagerI::filterActiveUsers(const IntSeq& userIds,
		const Ice::Current&) {
	IntSeq result;
	for (size_t i = 0; i < userIds.size(); ++i) {
		int userId = userIds.at(i);
		if ((_activeUserIds.size() < _minSize) || (_activeUserIds.has(userId))) {
			result.push_back(userId);
		}
	}
	MCE_DEBUG("Input: " << userIds.size() << ", Output: " << result.size());
	return result;
}

void ActiveUserManagerI::addActiveUsers(const IntSeq& userIds,
		const Ice::Current&) {
	MCE_DEBUG("addActive: " << userIds.size());
	for (uint i = 0; i < userIds.size(); ++i) {
		int userId = userIds.at(i);
		if (!_activeUserIds.has(userId)) {
			_activeUserIds.add(1, userId);
		}
	}
	MCE_DEBUG("addActive: " << userIds.size() << " done ");
}

void ActiveUserManagerI::removeActiveUsers(const IntSeq& userIds,
		const Ice::Current&) {
	MCE_DEBUG("removeActive: " << userIds.size());
	for (uint i = 0; i < userIds.size(); ++i) {
		_activeUserIds.remove(userIds.at(i));
	}
}

Ice::Int ActiveUserManagerI::getActiveUserSize(const Ice::Current&) {
	MCE_DEBUG("getActive: " << _activeUserIds.size());
	return _activeUserIds.size();
}

void ActiveUserManagerI::LoadActiveUsersTask::handle() {
	level(time(NULL)+60*60*24);
	TaskManager::instance().execute(this);

	IntSeq userIds;
	try {
		ConnectionHolder conn("active_user", CDbRServer);
		mysqlpp::Query query = conn.query();
		query << "SELECT id FROM " << _table;
		MCE_WARN("LoadActiveUserIdSet -> Sql: " << query.preview());
		mysqlpp::ResUse res = query.use();
		mysqlpp::Row row;
		while (row = res.fetch_row()) {
			userIds.push_back(row["id"]);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("ActiveUserIdSet: " << e);
	} catch (...) {
		MCE_WARN("ActiveUserIdSet unkown exception " );
	}
	MCE_DEBUG("LoadActiveUserIdSet -> Size: " << userIds.size());
	MCE_WARN("LoadActiveUserIdSet -> Final Size: " << userIds.size());
	for (uint i = 0; i < userIds.size(); ++i) {
		_activeUserIds.add(1, userIds.at(i));
	}
}

