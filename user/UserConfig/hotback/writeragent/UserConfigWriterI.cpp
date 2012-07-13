#include "UserConfigWriterI.h"
#include "UserConfigAdapter.h"
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

	service.getAdapter()->add(&UserConfigWriterI::instance(), service.createIdentity(USER_CONFIG_WRITER, ""));

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
void UserConfigWriterI::createUserConfig(const Str2StrMap& properties, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	MyUtil::Str2StrMap::const_iterator itr = properties.find(CUCUSERID);
	if (itr == properties.end()) {
		MCE_WARN("UserConfigWriterI::createUserConfig, Exception[NoId]");
		return;
	} else {
		userId = boost::lexical_cast<int>(itr->second);
	}
	UserConfigAdapter::instance().createUserConfig(userId, properties);
}

void UserConfigWriterI::setUserConfig(Ice::Int userId, const MyUtil::Str2StrMap& properties, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserConfigAdapter::instance().setUserConfig(userId, properties);
}

void UserConfigWriterI::reload(Ice::Int userId, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg << "id=" << userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserConfigAdapter::instance().reload(userId);
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
