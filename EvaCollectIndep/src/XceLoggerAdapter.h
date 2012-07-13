#ifndef __XCELOGGER_ADAPTER_H__
#define __XCELOGGER_ADAPTER_H__

#include "XceLogger.h"
#include "IpAddr.hpp"
#include "Ice/Ice.h"
#include <boost/lexical_cast.hpp>

namespace com {
namespace xiaonei {
namespace service {
using namespace MyUtil;
using std::string;
using std::cout;
using ::xce::log::StringSeq;
using ::xce::log::LoggerPrx;
using ::xce::log::LogEntry;

class XceLogger;
typedef IceUtil::Handle<XceLogger> XceLoggerPtr;
class XceLogger : virtual public Ice::Object, virtual public IceUtil::Monitor<IceUtil::Mutex> {
public:
	XceLogger(const string& loggerEndpoints);
	void addLog(const StringSeq& categories, const std::string& message);
	void init();
private:
	Ice::CommunicatorPtr _ic;
	LoggerPrx _prx;
	string _loggerEndpoints;
	std::vector<LogEntry> _cachedEntry;
	IceUtil::Mutex _mutex;

	void append(LogEntry& entry);
	void retrive(std::vector<LogEntry>& toInvokeCache);

	class CheckInvoke: virtual public IceUtil::Thread {
	public:
		CheckInvoke(XceLoggerPtr xceLogger);
		virtual void run();
	private:
		std::vector<LogEntry> _toInvokeCache;
		time_t _lastInvokeTime;
		XceLoggerPtr _xceLogger;

		void invokeAndClear();

		bool shouldInvoke() {
			bool should = (0 != _xceLogger->_prx)
					&& (_toInvokeCache.size() > 128 || time(NULL) - _lastInvokeTime > 10);
			return should;
		};

		void invoke() {
			if (shouldInvoke()){
				invokeAndClear();
			}
		};
	};

	friend class CheckInvoke;
	typedef IceUtil::Handle<CheckInvoke> CheckInvokePtr;
	CheckInvokePtr _checkThread;
};

class XceLoggerAdapter {
public:
	XceLoggerPtr getLogger();

	XceLoggerPtr getLogger(std::string& loggerEndpoints);

	static XceLoggerAdapter& instance() {
		if (!_instance) {
			IceUtil::Mutex::Lock lock(_mutex);
			if ( !_instance) {
				_instance = new XceLoggerAdapter;
			}
		}
		return *_instance;
	}

private:
	static XceLoggerAdapter* _instance;
	static IceUtil::Mutex _mutex;

	XceLoggerAdapter() {
	};

	virtual ~XceLoggerAdapter() {
		cout << "XceLoggerAdapter::~XceLoggerAdapter" << std::endl;
		if (_instance) {
			XceLoggerAdapter* p = _instance;
			_instance = NULL;
			delete p;
		}
	};
};

}
}
}
#endif

