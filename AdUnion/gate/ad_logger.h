#ifndef _XCE_AD_AD_LOGGER_H_
#define _XCE_AD_AD_LOGGER_H_

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>

#include "TaskManager.h"

namespace xce {
namespace ad {

inline void InitAdLogger(
	const std::string& name, 
	const std::string& path, 
	const std::string& loglevel = "WARN",
	//const std::string& pattern = "%L, %-10D{%H:%M:%S.%q} : [%p] : %m%n")
	const std::string& pattern = "%m,%D{%Y-%m-%d %H:%M}:00,%D{%Y-%m-%d %H:%M}:59%n")
{
	log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::MINUTELY, 1, true));
	append_1->setName(name);
	append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
	log4cplus::Logger::getInstance(name).addAppender(append_1);
	log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

inline void InitAdPVLogger(
  const std::string& name,
  const std::string& path,
  const std::string& loglevel = "WARN",
  //const std::string& pattern = "%L, %-10D{%H:%M:%S.%q} : [%p] : %m%n")
  const std::string& pattern = "%m%n")
{
  log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::MINUTELY, 1, true));
  append_1->setName(name);
  append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
  log4cplus::Logger::getInstance(name).addAppender(append_1);
  log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

inline void InitAdClickLogger(
  const std::string& name,
  const std::string& path,
  const std::string& loglevel = "WARN",
  //const std::string& pattern = "%L, %-10D{%H:%M:%S.%q} : [%p] : %m%n")
  const std::string& pattern = "%m%n")
{
  log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::MINUTELY, 1, true));
  append_1->setName(name);
  append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
  log4cplus::Logger::getInstance(name).addAppender(append_1);
  log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

#define AD_DEBUG(msg) LOG_DEBUG("ad", msg);
#define AD_INFO(msg) LOG_INFO("ad", msg);
#define AD_WARN(msg) LOG_WARN("ad", msg);

#define ADPV_INFO(msg) LOG_INFO("ad_pv", msg);
#define ADCK_INFO(msg) LOG_INFO("ad_click", msg);

inline void InitAdUnionPvLogger(
	const std::string& name, 
	const std::string& path, 
	const std::string& loglevel = "WARN",
	//const std::string& pattern = "%L, %-10D{%H:%M:%S.%q} : [%p] : %m%n")
	const std::string& pattern = "%m%n")
{
	log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::MINUTELY, 1, true));
	append_1->setName(name);
	append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
	log4cplus::Logger::getInstance(name).addAppender(append_1);
	log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

#define AD_UNION_PV_DEBUG(msg) LOG_DEBUG("ad_union_pv", msg);
#define AD_UNION_PV_INFO(msg) LOG_INFO("ad_union_pv", msg);
#define AD_UNION_PV_WARN(msg) LOG_WARN("ad_union_pv", msg);

class AdLogTimer : public MyUtil::Timer {
public:
  AdLogTimer() : Timer(43 * 1000) {}
  virtual void handle() {
    AD_INFO("empty");
    AD_UNION_PV_INFO("empty");
    ADPV_INFO("empty");
    ADCK_INFO("empty");
  }
};

}
}

#endif // _XCE_AD_AD_LOGGER_H_

