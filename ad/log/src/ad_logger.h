#ifndef _XCE_AD_AD_LOGGER_S_H_
#define _XCE_AD_AD_LOGGER_S_H_

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>

#include "TaskManager.h"

namespace xce {
namespace ad {

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

inline void InitAdChargeLogger(
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

inline void InitAdPvChargeLogger(
    const std::string& name,
    const std::string& path,
    const std::string& loglevel = "WARN",
    const std::string& pattern = "%m%n")
    //const std::string& pattern = "%m,%D{%Y-%m-%d %H:%M}:00,%D{%Y-%m-%d %H:%M}:59%n")
{
  log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::MINUTELY, 1, true));
  append_1->setName(name);
  append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
  log4cplus::Logger::getInstance(name).addAppender(append_1);
  log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

/*
 * For AdEngine ststem monitor
 */
/* inline void InitAdEngineLogLogger(
  const std::string& name,
  const std::string& path,
  const std::string& loglevel = "WARN",
  //const std::string& pattern = "%L, %-10D{%H:%M:%S.%q} : [%p] : %m%n")
  const std::string& pattern = "%m%n")
{
  log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::HOURLY, 1, true));
  append_1->setName(name);
  append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
  log4cplus::Logger::getInstance(name).addAppender(append_1);
  log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
} */

inline void InitAdStatLogger(
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


#define ADPV_INFO(msg) LOG_INFO("ad_pv", msg);
#define ADCK_INFO(msg) LOG_INFO("ad_click", msg);
#define ADCH_INFO(msg) LOG_INFO("ad_charge", msg);
#define ADPC_INFO(msg) LOG_INFO("ad_pv_charge", msg);

// #define AD_ENGINE_LOG_INFO(msg) LOG_INFO("ad_engine_log", msg);
#define AD_STAT_INFO(msg) LOG_INFO("ad_stat", msg);


class AdLogTimer : public MyUtil::Timer {
public:
  AdLogTimer() : Timer(43 * 1000) {}
  virtual void handle() {
    ADPV_INFO("empty");
    ADCK_INFO("empty");
    ADCH_INFO("empty");
    ADPC_INFO("empty");
  }
};

}
}

#endif // _XCE_AD_AD_LOGGER_S_H_

