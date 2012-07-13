#include "XceLoggerI.h"
#include "ServiceI.h"  /* 所有引用过log4cplus的头文件，必须放在一起，都放在最前面 */
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"


using namespace std;
using namespace MyUtil;
using namespace xce::log;
using xce::serverstate::ServerStateSubscriber;

//***************************************************************************

void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();
	std::string logroot = service.getCommunicator()->getProperties()->getPropertyWithDefault("XceLogger.LogRoot", "../log/XceLog");
	IceUtil::Handle<LoggerI> logger = new LoggerI(logroot);
	service.getAdapter()->add(logger, service.createIdentity("M", ""));

	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("XceLogger.Mod", 0);
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("XceLogger.Interval", 5);
	string ctrlEndpoints = service.getCommunicator()->getProperties()
		->getPropertyWithDefault("XceLogger.ControllerEndpoint", "ControllerXceLogger");
	string protocal = service.getCommunicator()->getProperties()->getProperty("XceLogger.Endpoints");	// 设置tcp/udp协议
	int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("XceLogger.Status", 0); // 设置正选/备份

	ServerStateSubscriber::instance().initialize(ctrlEndpoints, logger, mod, interval, new OceChannel(),"10", "3000", "131072", protocal);
	ServerStateSubscriber::instance().setStatus(status);
	
	MCE_INFO("[MyUtil::initialize] Controller : " << ctrlEndpoints << " Protocal : " << protocal << " Status : " << status);
}

//***************************************************************************

LoggerI::LoggerI(const std::string& logroot) :
	logroot_(logroot) {
}

void xce::log::LoggerI::append(const ::xce::log::LogEntries& messages,
		const Ice::Current& current) {
	for (LogEntries::const_iterator it = messages.begin(); it != messages.end(); ++it) {
		if (it->message.empty())
			continue;
		ostringstream category_stream;
		for (StringSeq::const_iterator categories_iterator =
				it->categories.begin(); categories_iterator
				!= it->categories.end(); ++categories_iterator) {
			category_stream << "/" << *categories_iterator;
		}
		string category = category_stream.str();
		HourlyRollingFileAppenderPtr appender = locateAppender(category);
		if (!appender) {
			MCE_WARN("Cannot locate an appender for " << category);
			continue;
		}
		if(it->message.at(it->message.length()-1)=='\n'){
			appender->log(it->message.substr(0, it->message.length() - 1));
		} else {
			appender->log(it->message);
		}
	}
}

HourlyRollingFileAppenderPtr xce::log::LoggerI::locateAppender(const std::string& category) {
	{
		IceUtil::RWRecMutex::RLock lock(mutex_);
		std::map<std::string, HourlyRollingFileAppenderPtr>::iterator it = inited_loggers_.find(category);
		if (it != inited_loggers_.end()) {
			return it->second;
		}
	}
	IceUtil::RWRecMutex::WLock lock(mutex_);
	std::map<std::string, HourlyRollingFileAppenderPtr>::iterator it = inited_loggers_.find(category);
	if (it != inited_loggers_.end()) {
		return it->second;
	}
	//MCE_DEBUG("logroot is "<<logroot_);
	HourlyRollingFileAppenderPtr appender = new HourlyRollingFileAppender(logroot_ + "/logger" + category);
	appender->initialize();
	if (inited_loggers_.insert(make_pair(category, appender)).second) {
		return appender;
	};
	return 0;
}
