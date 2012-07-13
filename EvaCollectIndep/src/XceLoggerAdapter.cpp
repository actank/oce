#include "XceLoggerAdapter.h"

using namespace com::xiaonei::service;
using std::cout;

IceUtil::Mutex com::xiaonei::service::XceLoggerAdapter::_mutex;
XceLoggerAdapter* com::xiaonei::service::XceLoggerAdapter::_instance;

XceLogger::XceLogger(const string& loggerEndpoints) :
		_ic(NULL),
		_loggerEndpoints(loggerEndpoints),
		_cachedEntry(std::vector<LogEntry>()) {
};


void XceLogger::addLog(const StringSeq& categories, const std::string& message) {
	LogEntry entry;
	entry.categories = categories;
	entry.message = message;
	append(entry);
}


void XceLogger::init() {

	// 生成Communicator
	Ice::PropertiesPtr properties = Ice::createProperties();
	properties->setProperty("Ice.Override.Timeout", "300");
	properties->setProperty("Ice.Override.ConnectTimeout", "70");
	properties->setProperty("IceGrid.InstanceName", "XceLog");
	properties->setProperty("Ice.ThreadPool.Client.StackSize", "65535");
	properties->setProperty("Ice.ThreadPool.Server.StackSize", "65535");
	properties->setProperty("Ice.MessageSizeMax", "20480");
	properties->setProperty("Ice.Default.Host", IpAddr::inner());
	properties->setProperty("Ice.Default.Locator",
		 "XceLog/Locator:default -h LogMaster -p 4062:default -h LogReplica1 -p 4062:default -h LogReplica2 -p 4062");
	Ice::InitializationData id;
	id.properties = properties;
	_ic = Ice::initialize(id);

	// 生成prx
	_prx = LoggerPrx::uncheckedCast( _ic->stringToProxy(_loggerEndpoints)->ice_datagram() );
	
	// 启动CheckThread
	// _checkThread = new CheckInvoke(this);
	XceLogger::CheckInvokePtr checkThread = new CheckInvoke(this);

	// _checkThread->start(65535).detach();
	checkThread->start(65535).detach();
}

void XceLogger::append(LogEntry& entry) {

	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	if(_cachedEntry.size() > 100000){
		return;
	}
	_cachedEntry.push_back(entry);
	if (_cachedEntry.size() == 1) {
		notify();
	}
};

void XceLogger::retrive(std::vector<LogEntry>& toInvokeCache) {

	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	while( _cachedEntry.empty() ) {
		timedWait(IceUtil::Time::seconds(10));
		if( toInvokeCache.size() != 0 ){
			break;
		}
	}

	toInvokeCache.insert(toInvokeCache.end(), _cachedEntry.begin(), _cachedEntry.end());
	_cachedEntry.clear();
};

XceLogger::CheckInvoke::CheckInvoke(XceLoggerPtr xceLogger) :
	_lastInvokeTime(time(NULL)),_xceLogger(xceLogger) {
};

void XceLogger::CheckInvoke::run() {

	while (true) {
		try {
			invoke();
			_xceLogger->retrive(_toInvokeCache);
		} catch(Ice::Exception& e){
			cout << "[EvaLogger::CheckInvoke] Ice::Exception " << e.what() << std::endl;
		}catch( std::exception& e){
			cout << "[EvaLogger::CheckInvoke] std::exception " << e.what() << std::endl;
		}catch (...) {
			cout << "[EvaLogger::CheckInvoke] unknown exception" << std::endl;
		}
	}
}

void XceLogger::CheckInvoke::invokeAndClear() {

	std::vector<LogEntry> toInvokeCache_swap;
	_toInvokeCache.swap(toInvokeCache_swap);
	_lastInvokeTime = time(NULL);

	try {
		_xceLogger->_prx->append(toInvokeCache_swap);
		cout << "[EvaLogger::invokeAndClear] add success " << std::endl;	// 测试
	} catch (Ice::Exception& e) {
		cout << "[EvaLogger::invokeAndClear]:  Ice::Exception: " << e.what() << std::endl;
	} catch (std::exception & e) {
		cout << "[EvaLogger::invokeAndClear]:  std::exception: " << e.what() << std::endl;
	} catch (...) {
		cout << "[EvaLogger::invokeAndClear]:  Exception " << std::endl;
	}
}

XceLoggerPtr XceLoggerAdapter::getLogger() {
	std::string loggerEndpoints = "M@EvaLogger";
	return getLogger(loggerEndpoints);
}

XceLoggerPtr XceLoggerAdapter::getLogger(std::string& loggerEndpoints){
	loggerEndpoints = loggerEndpoints.empty() ? "M@EvaLogger" : loggerEndpoints;
	XceLoggerPtr logger =  new XceLogger(loggerEndpoints);
	logger->init();
	return logger;
};

