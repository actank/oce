#include "UserConfigI.h"
#include "UserConfigReaderAdapter.h"
#include "LogWrapper.h"
#include "BatchUpdaterAdapter.h"
#include "UserLogicAdapter.h"
#include "UserConfigCommon.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "util/cpp/InvokeMonitor.h"
#include "Date.h"
#include "time.h"

using namespace mop::hi::svc::model;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace ::xce::serverstate;
using namespace xce::readeradapter;

//初始化
//--------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	string type = service.getCommunicator()->getProperties()->getPropertyWithDefault("UserConfig.ObjectCacheType", "Retention");
	if (type == "Retention") {
		service.registerObjectCache(USER_CONFIG, "UC", new UserConfigFactoryI, false);
	} else {
		service.registerObjectCache(USER_CONFIG, "UC", new UserConfigFactoryI, true);
	}
	service.getAdapter()->add(&UserConfigManagerI::instance(), service.createIdentity(USER_CONFIG_MANAGER, ""));

	//用来setdata的task 	
	TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

	//HA
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserConfig.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfig.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerUserConfigR", &UserConfigManagerI::instance(), mod, interval);
	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfig.Status", 0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os << "MyUtil::initialize set status : " << status;
		MCE_WARN( os.str() );
	}
	ServiceMonitorManager::instance().start();
}

//UserConfigManagerI
//--------------------------------------------------------------
UserConfigManagerI::UserConfigManagerI() {
	_tempMutexManager = new TempMutexManager();
}

UserConfigManagerI::~UserConfigManagerI() {
}

MyUtil::Str2StrMap UserConfigManagerI::getUserConfig(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return _getUserConfig(userId)->getProperties();
}

void UserConfigManagerI::setUserConfig(Ice::Int userId, const MyUtil::Str2StrMap& properties, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			Str2StrMap filter;
			filter[CUCUSERID] = boost::lexical_cast<string>(userId);
			QueryRunner("user_config", CDbWServer).execute(Statement::buildUpdate(TABLE_USER_CONFIG, filter, properties));
			_getUserConfig(userId)->setProperties(properties);
			UserConfigReaderAdapter::instance().setUserConfig(userId, properties);
		}
		UserLogicAdapter::instance().updateUserConfig(userId, properties, current.ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserConfigManagerI::setUserConfig] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserConfigManagerI::setUserConfig] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserConfigManagerI::setUserConfig] UNKNOWN exception");
	}
}

void UserConfigManagerI::createUserConfig(const Str2StrMap& properties, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	try {
		int userId;
		MyUtil::Str2StrMap::const_iterator itr = properties.find(CUCUSERID);
		if (itr == properties.end()) {
			MCE_WARN("UserConfigManagerI::createUserConfig, Exception[NoId]");
			return;
		} else {
			userId = boost::lexical_cast<int>(itr->second);
		}
		Lock lock(_tempMutexManager, userId);
		QueryRunner("user_config", CDbWServer).execute(Statement::buildInsert(TABLE_USER_CONFIG, properties));
		UserConfigDataIPtr rs = new UserConfigDataI;
		BatchConfigResHandlerI handler(rs);
		Str2StrMap filter;
		filter[CUCUSERID] = const_cast<Str2StrMap&> (properties)[CUCUSERID];
		QueryRunner("user_config", CDbWServer).query(Statement::buildSelect(TABLE_USER_CONFIG, filter), handler);
		ServiceI::instance().addObject(rs, USER_CONFIG, rs->userId);
		UserConfigReaderAdapter::instance().setData(userId, rs);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserConfigManagerI::createUserConfig] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserConfigManagerI::createUserConfig] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserConfigManagerI::createUserConfig] UNKNOWN exception");
	}
}

void UserConfigManagerI::reload(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	try {
		Lock lock(_tempMutexManager, userId);
		ServiceI::instance().removeObject(USER_CONFIG, userId);
		UserConfigDataIPtr result = ServiceI::instance().locateObject<UserConfigDataIPtr> (USER_CONFIG, userId);
		if (result) {
			UserConfigReaderAdapter::instance().setData(userId, result);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserConfigManagerI::reload] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserConfigManagerI::reload] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserConfigManagerI::reload] UNKNOWN exception");
	}
}

//仅供Preloader使用
void UserConfigManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	ostringstream oss;
	oss << "size=" << userData->data.size();
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	TaskManager::instance().execute(new FillTask<UserConfigDataIPtr, UserConfigDataI> (userData));
}

bool UserConfigManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	return ServerStateSubscriber::instance().getStatus();
}

void UserConfigManagerI::setValid(bool valid, const Ice::Current& current) {
	ostringstream oss;
	oss << "value=" << valid;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServerStateSubscriber::instance().setStatus((int) valid);
}

UserConfigDataIPtr UserConfigManagerI::_getUserConfig(int userId) {
	UserConfigDataIPtr result = NULL;

	try {
		result = ServiceI::instance().findObject<UserConfigDataIPtr> (USER_CONFIG, userId);
		if (!result) {
			//		//为了读性能不加锁，少数情况下会造成各个Reader的缓存不一致
			//		Lock lock(_tempMutexManager, userId);
			result = ServiceI::instance().locateObject<UserConfigDataIPtr> (USER_CONFIG, userId);
			if (isValid()) {
				UserConfigReaderAdapter::instance().setData(userId, result);
			}
		}
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("UserConfigManagerI::m_getUserConfig NoSuchUserConfigException reason - NoSuchUserConfigException [" << userId << "]");
		throw NoSuchObjectException();
	}

	return result;
}

//UserConfigFactoryI
//--------------------------------------------------------------
Ice::ObjectPtr UserConfigFactoryI::create(Ice::Int id) {
	MCE_DEBUG("Loading " << id);

	Str2StrMap filter;
	filter[CUCUSERID] = boost::lexical_cast<string>(id);

	QueryRunner qr("user_config", CDbRServer);

	UserConfigDataIPtr rs = new UserConfigDataI;
	BatchConfigResHandlerI handler(rs);
	QueryRunner("user_config", CDbRServer).query(Statement::buildSelect(TABLE_USER_CONFIG, filter), handler);

	if (rs->userId != id) {
		return 0;
	}

	MCE_DEBUG( "Loading " << id << " done.");
	return rs;
}

//ResultHandler
//--------------------------------------------------------------
bool BatchConfigResHandlerI::handle(mysqlpp::Row& row) const {
	m_rs->parse(row);
	if (m_rs == 0) {
		MCE_WARN( "Parse failed! ");
		return false;
	}
	return true;
}
