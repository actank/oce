#include "UserConfigWriterI.h"
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
using namespace xce::tempmutex;
using namespace xce::readeradapter;

//初始化
//--------------------------------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&UserConfigWriterI::instance(), service.createIdentity(USER_CONFIG_WRITER, ""));

	//用来setdata的task 	
	TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

	//HA
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserConfigWriter.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfigWriter.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerUserConfigWriter", &UserConfigWriterI::instance(), mod, interval);
	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfigWriter.Status", 0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os << "MyUtil::initialize set status : " << status;
		MCE_WARN( os.str() );
	}
	ServiceMonitorManager::instance().start();
}

//UserConfigWriterI
//--------------------------------------------------------------
UserConfigWriterI::UserConfigWriterI() {
	_tempMutexManager = new TempMutexManager();
}

UserConfigWriterI::~UserConfigWriterI() {
}

void UserConfigWriterI::createUserConfig(const Str2StrMap& properties, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	try {
		int userId;
		MyUtil::Str2StrMap::const_iterator itr = properties.find(CUCUSERID);
		if (itr == properties.end()) {
			MCE_WARN("UserConfigWriterI::createUserConfig, Exception[NoId]");
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
		UserConfigReaderAdapter::instance().setData(userId, rs);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserConfigWriterI::createUserConfig] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserConfigWriterI::createUserConfig] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserConfigWriterI::createUserConfig] UNKNOWN exception");
	}
}

void UserConfigWriterI::setUserConfig(Ice::Int userId, const MyUtil::Str2StrMap& properties, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	try {
		{
			Lock lock(_tempMutexManager, userId);
			Str2StrMap filter;
			filter[CUCUSERID] = boost::lexical_cast<string>(userId);
			QueryRunner("user_config", CDbWServer).execute(Statement::buildUpdate(TABLE_USER_CONFIG, filter, properties));
			UserConfigReaderAdapter::instance().setUserConfig(userId, properties);
		}
		UserLogicAdapter::instance().updateUserConfig(userId, properties, current.ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserConfigWriterI::setUserConfig] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserConfigWriterI::setUserConfig] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserConfigWriterI::setUserConfig] UNKNOWN exception");
	}
}

void UserConfigWriterI::reload(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	try {
		Lock lock(_tempMutexManager, userId);
		UserConfigDataIPtr rs = new UserConfigDataI;
		BatchConfigResHandlerI handler(rs);
		Str2StrMap filter;
		filter[CUCUSERID] = boost::lexical_cast<string>(userId);
		QueryRunner("user_config", CDbWServer).query(Statement::buildSelect(TABLE_USER_CONFIG, filter), handler);
		UserConfigReaderAdapter::instance().setData(userId, rs);
	} catch (Ice::Exception& e) {
		MCE_WARN("[UserConfigWriterI::reload] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[UserConfigWriterI::reload] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[UserConfigWriterI::reload] UNKNOWN exception");
	}
}

bool UserConfigWriterI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	return ServerStateSubscriber::instance().getStatus();
}

void UserConfigWriterI::setValid(bool valid, const Ice::Current& current) {
	ostringstream oss;
	oss << "value=" << valid;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServerStateSubscriber::instance().setStatus((int) valid);
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
