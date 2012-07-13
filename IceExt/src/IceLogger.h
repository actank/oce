#ifndef __ICE_LOGGER_H_
#define __ICE_LOGGER_H_

#include "UtilCxx/src/LogWrapper.h"
#include <Ice/Ice.h>
#include <IceUtil/Time.h>

using namespace std;
using namespace log4cplus;

namespace MyUtil {

//---------------------------------------------------------------------------
class IceLogger : virtual public Ice::Logger {
public:
	IceLogger(const string& path, const string& level = "WARN") {
		MCE_INIT(path, level);
	}

	virtual ~IceLogger() {
		;
	}

	virtual void print(const ::std::string& message) {
		MCE_INFO(message);
	}

	virtual void trace(const ::std::string& category,
			const ::std::string& message) {
		MCE_TRACE(" ["+category +"] "+message);
	}

	virtual void warning(const ::std::string& message) {
		MCE_WARN(message);
	}

	virtual void error(const ::std::string& message) {
		MCE_ERROR(message);
	}
};
typedef IceUtil::Handle<IceLogger> IceLoggerPtr;

class TraceLatency {
public:
	TraceLatency(const string& operation, const string& ext) :
		_operation(operation), _ext(ext) {
		_timeOfBegin = IceUtil::Time::now();
	}
	~TraceLatency() {
		IceUtil::Time timeOfEnd = IceUtil::Time::now();
		MCE_INFO("TraceLatency::trace -> " << (timeOfEnd - _timeOfBegin).toMicroSeconds()
			<< " -> " << _operation << " -> " << _ext);
	};
private:
	IceUtil::Time _timeOfBegin;
	string _operation;
	string _ext;
};
};
#endif /*ICELOGGER_H_*/
