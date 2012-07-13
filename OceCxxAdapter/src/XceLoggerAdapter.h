#ifndef __XCELOGGER_ADAPTER_H__
#define __XCELOGGER_ADAPTER_H__

#include "XceLogger.h"
#include "IceExt/src/Channel.h"
#include "IceExt/src/AdapterI.h"
#include "UtilCxx/src/Singleton.h"
#include <boost/lexical_cast.hpp>

namespace com {
namespace xiaonei {
namespace service {
using namespace MyUtil;
using namespace ::xce::log;

class XceLogger;
typedef IceUtil::Handle<XceLogger> XceLoggerPtr;
class XceLogger : virtual public Ice::Object, virtual public IceUtil::Monitor<IceUtil::Mutex> {
public:
	XceLogger(const string& loggerEndpoints,int waitSecond = 10, int sendSize = 32768) :
		_cachedEntry(std::vector<LogEntry>()),
		_loggerEndpoints(loggerEndpoints),
		_waitSecond(waitSecond),
		_sendSize(sendSize),
		_currentSize(0) 	{
		
		_ic= _channel.getCommunicator();
		_prx = LoggerPrx::uncheckedCast( _ic->stringToProxy(_loggerEndpoints)->ice_datagram() );
	};

	XceLogger(int argc, char *argv[], const string& loggerEndpoints, int waitSecond = 10, int sendSize = 65536): 
		_cachedEntry(std::vector<LogEntry>()),
		_loggerEndpoints(loggerEndpoints),
		_waitSecond(waitSecond),
		_sendSize(sendSize),
		_currentSize(0) 	{
		
		_ic = Ice::initialize(argc, argv);
		_prx = LoggerPrx::uncheckedCast( _ic->stringToProxy(_loggerEndpoints)->ice_datagram() );
	};

	void initialize(){
		_checkThread = new CheckInvoke(this);
		_checkThread->start(65535).detach();				
	};
	
	void addLog(const StringSeq& categories, const std::string& message) {
		LogEntry entry;
		entry.categories = categories;
		entry.message = message;
		append(entry);
		// MCE_DEBUG("[append log] Categories: " << entry.categories[0] << " Message:" << entry.message);
	};

private:
	MyUtil::LogChannel _channel;
	Ice::CommunicatorPtr _ic;
	LoggerPrx _prx;
	std::vector<LogEntry> _cachedEntry;
	IceUtil::Mutex _mutex;
	string _loggerEndpoints;
	int _waitSecond;
	int _sendSize;
	int _currentSize;

	void append(LogEntry& entry) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		if(_cachedEntry.size() > 100000){
			return;
		}
		_cachedEntry.push_back(entry);
		_currentSize += entry.message.size();
		if (_cachedEntry.size() == 1) {
//			MCE_INFO("[XceLogger]: notify()");
			notify();
		}
	};

	void retrive(std::vector<LogEntry>& toInvokeCache, int& currentSize) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		while( _cachedEntry.empty() ) {
//			MCE_INFO("[XceLogger]: timedWait(IceUtil::Time::seconds(10))");
			timedWait(IceUtil::Time::seconds(_waitSecond));
			if( toInvokeCache.size() != 0 ){
				break;
			}
		}
		toInvokeCache.insert(toInvokeCache.end(), _cachedEntry.begin(), _cachedEntry.end());
		currentSize += _currentSize;
		_cachedEntry.clear();
		_currentSize = 0;
//		MCE_INFO("[XceLogger]: reading new LogEntry from cachedEntry ... _cachedEntry.size()="
//				<< _cachedEntry.size() << "  toInvokeCache.size()=" << toInvokeCache.size() );
	};

	class CheckInvoke: virtual public IceUtil::Thread {
	public:
		CheckInvoke(XceLoggerPtr xceLogger) :
			_lastInvokeTime(time(NULL)),_xceLogger(xceLogger), _currentSize(0) {
		};
		virtual void run(){
//			MCE_INFO("[XceLogger]: _checkThread started");
			while (true) {
				try {
					invoke();
					// MCE_DEBUG("[XceLogger] : retrive " << _toInvokeCache.size());
					_xceLogger->retrive(_toInvokeCache, _currentSize);
				} catch (...) {
					cerr << "[XceLogger]: CheckInvoke thread exception " << endl;
				}
			}
		};
	private:
		std::vector<LogEntry> _toInvokeCache;
		time_t _lastInvokeTime;
		XceLoggerPtr _xceLogger;
		int _currentSize;
		void invoke() {
			if (shouldInvoke()){
				invokeAndClearCache();
			}
		};
		bool shouldInvoke() {
			return (0 != _xceLogger->_prx)
					&& (_currentSize > _xceLogger->_sendSize || time(NULL) - _lastInvokeTime >_xceLogger->_waitSecond);
		};
		void invokeAndClearCache() {
			std::vector<LogEntry> toInvokeCache_swap;
			_toInvokeCache.swap(toInvokeCache_swap);
			_currentSize = 0;
//			MCE_INFO("[XceLogger]: shouldInvoke  size = " << toInvokeCache_swap.size() << "   waited = " << time(NULL) << " - " << _lastInvokeTime );
			_lastInvokeTime = time(NULL);
			try {
				_xceLogger->_prx->append(toInvokeCache_swap);
				// MCE_DEBUG("[XceLogger]:  _prx->append " << toInvokeCache_swap.size());
			} catch (Ice::Exception& e) {
				MCE_WARN("[XceLogger]:  Ice::Exception " << e);
			} catch (std::exception & e) {
				MCE_WARN("[XceLogger]:  td::exception ");
			} catch (...) {
				MCE_WARN("[XceLogger]:  Exception ");
			}
		};
	};

	friend class CheckInvoke;
	typedef IceUtil::Handle<CheckInvoke> CheckInvokePtr;
	CheckInvokePtr _checkThread;
};

class XceLoggerAdapter: public Singleton<XceLoggerAdapter> {
public:
	XceLoggerPtr getLogger(){
		std::string loggerEndpoints = "";
		return getLogger(loggerEndpoints);
	};
	XceLoggerPtr getLogger(int waitSecond, int size = 65536){
		string loggerEndpoints =  "M@XceLogger" ;
                com::xiaonei::service::XceLoggerPtr logger = new XceLogger(loggerEndpoints,waitSecond,size);
                return logger;
	};
	XceLoggerPtr getLogger(std::string& loggerEndpoints){

		loggerEndpoints = loggerEndpoints.empty() ? "M@XceLogger" : loggerEndpoints;
		com::xiaonei::service::XceLoggerPtr logger = new XceLogger(loggerEndpoints);
		return logger;
	};

	XceLoggerPtr getLogger(int argc, char *argv[], std::string& loggerEndpoints){		
		loggerEndpoints = loggerEndpoints.empty() ? "M@XceLogger" : loggerEndpoints;
		com::xiaonei::service::XceLoggerPtr logger = new XceLogger(argc, argv, loggerEndpoints);
		return logger;
	};
};

}
}
}
#endif

