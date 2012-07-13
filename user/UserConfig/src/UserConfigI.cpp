#include "UserConfigI.h"
#include "LogWrapper.h"
#include "BatchUpdaterAdapter.h"
#include "UserLogicAdapter.h"
#include "Date.h"
#include "UserConfigCommon.h"

#include "QueryRunner.h"

#include "time.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
#include "util/cpp/InvokeMonitor.h"
#include "Monitor/client/service/ServiceMonitor.h"
using namespace mop::hi::svc::model;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;

using namespace ::xce::serverstate;
//--------------------------------------------------------------------------------
/* the new instialize */
void MyUtil::initialize() {
	ServiceI& service=ServiceI::instance();
	string type = service.getCommunicator()->getProperties()->getPropertyWithDefault("UserConfig.ObjectCacheType", "Retention");
	if (type == "Retention") {
		service.registerObjectCache(USER_CONFIG, "UC", new UserConfigFactoryI, false);
	} else {
		service.registerObjectCache(USER_CONFIG, "UC", new UserConfigFactoryI, true);
	}
	service.getAdapter()->add(&UserConfigManagerI::instance(), service.createIdentity(
			USER_CONFIG_MANAGER, ""));

	//用来setdata的task 	
    TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));
	
	//	--------------------------------HA--------------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserConfig.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfig.Interval",5);
	ServerStateSubscriber::instance().initialize("ControllerUserConfigR",&UserConfigManagerI::instance(),mod,interval);
	{ 
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfig.Status",0);
		ServerStateSubscriber::instance().setStatus(status);
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;
		MCE_WARN( os.str() );
	}
  ServiceMonitorManager::instance().start();
}

//--------------------------------------------------------------------------------

void UserConfigManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	ostringstream oss;
	oss << "size: "<<userData->data.size();
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	TaskManager::instance().execute(new FillTask<UserConfigDataIPtr, UserConfigDataI>(userData));
}

void UserConfigManagerI::setValid(bool valid, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	ServerStateSubscriber::instance().setStatus((int)valid);
}

void UserConfigManagerI::createUserConfig(const Str2StrMap& properties,
		const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	QueryRunner( "user_config" , CDbWServer ).execute( Statement::buildInsert( TABLE_USER_CONFIG , properties ) );
	UserConfigDataIPtr rs = new UserConfigDataI ;
	BatchConfigResHandlerI handler(rs);
	Str2StrMap filter;
	filter[CUCUSERID]=const_cast<Str2StrMap&> (properties)[CUCUSERID];
	QueryRunner( "user_config" , CDbWServer ).query(Statement::buildSelect(TABLE_USER_CONFIG,filter),handler);
	ServiceI::instance().addObject(rs , USER_CONFIG, rs->userId);
}

MyUtil::Str2StrMap UserConfigManagerI::getUserConfig(Ice::Int uid,
		const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id="<<uid;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return _getUserConfig(uid)->getProperties();
}

void UserConfigManagerI::setUserConfig(Ice::Int uid,
		const MyUtil::Str2StrMap& props, const Ice::Current& current) {

	ostringstream inMsg;
	inMsg<<"id="<<uid;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	Str2StrMap filter;
	filter[CUCUSERID] = boost::lexical_cast<string>(uid);
	QueryRunner("user_config", CDbWServer).execute( Statement::buildUpdate( TABLE_USER_CONFIG , filter ,  props ) );
	_getUserConfig(uid)->setProperties(props);
	UserLogicAdapter::instance().updateUserConfig(uid, props, current.ctx);
}

//reload
void UserConfigManagerI::reload(Ice::Int userId, const Ice::Current& current) {
	
	ostringstream inMsg;
	inMsg<<"id="<<userId;
  InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	ServiceI& service = ServiceI::instance();
	service.reloadObject(USER_CONFIG, userId);
}

UserConfigDataIPtr UserConfigManagerI::_getUserConfig(int userId) {
	try {
		return ServiceI::instance().locateObject<UserConfigDataIPtr>(USER_CONFIG, userId);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("UserPassportManagerI::m_getUserConfig NoSuchUserConfigException reason - NoSuchUserConfigException [" << userId << "]");
		throw NoSuchObjectException();
	}
}

//--------------------------------------------------------------------------------------

Ice::ObjectPtr UserConfigFactoryI::create(Ice::Int id) {
	MCE_DEBUG("Loading " << id);
	
	Str2StrMap filter;
	filter[CUCUSERID]=boost::lexical_cast<string>(id);

	QueryRunner qr( "user_config" , CDbRServer );
	
	UserConfigDataIPtr rs = new UserConfigDataI ;
	BatchConfigResHandlerI handler(rs);
	QueryRunner( "user_config" , CDbRServer ).query(Statement::buildSelect(TABLE_USER_CONFIG,filter),handler);
	
	if (rs->userId != id) {
		return 0;
	}

	MCE_DEBUG( "Loading " << id << " done.");
	return rs;
}
//-------------------------------BatchConfigResHandlerI-----------------------
bool BatchConfigResHandlerI::handle(mysqlpp::Row& row) const
{
	m_rs->parse(row);
	if(m_rs == 0)
	{
		MCE_WARN( "Parse failed! ");
		return false;
	}
	return true;
}

