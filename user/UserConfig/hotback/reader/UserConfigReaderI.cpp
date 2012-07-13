#include "UserConfigReaderI.h"
#include "UserConfigWriterAdapter.h"
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
using namespace xce::adapter;

//初始化
//--------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	string type = service.getCommunicator()->getProperties()->getPropertyWithDefault("UserConfigReader.ObjectCacheType", "Retention");
	if (type == "Retention") {
		service.registerObjectCache(USER_CONFIG, "UC", new UserConfigFactoryI, false);
	} else {
		service.registerObjectCache(USER_CONFIG, "UC", new UserConfigFactoryI, true);
	}
	service.getAdapter()->add(&UserConfigReaderI::instance(), service.createIdentity(USER_CONFIG_READER, ""));

	//用来setdata的task 	
	TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

	//HA
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserConfigReader.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfigReader.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerUserConfigReader", &UserConfigReaderI::instance(), mod, interval);
	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfigReader.Status", 0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os << "MyUtil::initialize set status : " << status;
		MCE_WARN( os.str() );
	}
	ServiceMonitorManager::instance().start();
}

//UserConfigReaderI
//--------------------------------------------------------------
MyUtil::Str2StrMap UserConfigReaderI::getUserConfig(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return _getUserConfig(userId)->getProperties();
}

void UserConfigReaderI::setUserConfig(Ice::Int userId, const MyUtil::Str2StrMap& props, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	_getUserConfig(userId)->setProperties(props);
}

void UserConfigReaderI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	ostringstream oss;
	oss << "size=" << userData->data.size();
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	TaskManager::instance().execute(new FillTask<UserConfigDataIPtr, UserConfigDataI> (userData));
}

bool UserConfigReaderI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	return ServerStateSubscriber::instance().getStatus();
}

void UserConfigReaderI::setValid(bool valid, const Ice::Current& current) {
	ostringstream oss;
	oss << "value=" << valid;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServerStateSubscriber::instance().setStatus((int) valid);
}

UserConfigDataIPtr UserConfigReaderI::_getUserConfig(int userId) {
	UserConfigDataIPtr result = NULL;

	try {
		result = ServiceI::instance().findObject<UserConfigDataIPtr> (USER_CONFIG, userId);
		if (!result) {
			result = ServiceI::instance().locateObject<UserConfigDataIPtr> (USER_CONFIG, userId);
			if (isValid()) {
				UserConfigWriterAdapter::instance().reload(userId);
			}
		}
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("UserConfigReaderI::m_getUserConfig NoSuchUserConfigException reason - NoSuchUserConfigException [" << userId << "]");
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
