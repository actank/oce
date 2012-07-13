#ifndef __XceLoggerI_h__
#define __XceLoggerI_h__

#include <XceLogger.h>
#include <Ice/Service.h>
#include <set>
#include <IceUtil/RWRecMutex.h>

namespace xce {

namespace log {

class XceLoggerService: virtual public Ice::Service {
protected:
	virtual bool start(int, char*[]);
};

class LoggerI: virtual public Logger {
public:

	LoggerI(const std::string& logroot, const std::string& config);
	virtual void append(const ::xce::log::LogEntries&, const Ice::Current&);

private:
	void initDefault(const std::string& category);
	IceUtil::RWRecMutex init_mutex_;
	std::set<std::string> inited_loggers_;
	std::string logroot_;
	const std::string config_;
};

}
}

#endif
