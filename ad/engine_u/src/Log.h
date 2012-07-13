/*
 * Log.h
 *
 *  Created on: 2010-11-2
 *      Author: ark
 */

#ifndef LOG_H_
#define LOG_H_
#include "ad/thriftappender/src/thriftappender.h"

namespace xce {
namespace ad {

#define FORMULA_INFO(msg) LOG_INFO("formula", msg);
#define STATISTICS(msg) LOG_INFO("statistics", msg);

inline void InitAdEngineLogger(
  const std::string& name,
  const std::string& path,
  const std::string& loglevel = "WARN",
  const std::string& pattern = "%m,%D{%Y-%m-%d %H:%M}:00,%D{%Y-%m-%d %H:%M}:59%n")
{
  log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::HOURLY, 1, true));
  append_1->setName(name);
  append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
  log4cplus::Logger::getInstance(name).addAppender(append_1);
  log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

inline void InitAdWhiteListLogger(
  const std::string& name,
  const std::string& host,
  int port,
  const std::string& loglevel = "WARN",
  const std::string& pattern = "%m,%D{%Y-%m-%d %H:%M}:00,%D{%Y-%m-%d %H:%M}:59%n")
{
  log4cplus::SharedAppenderPtr append_1(new log4cplus::ThriftAppender(host, port, name));
  log4cplus::Logger::getInstance(name).addAppender(append_1);
  log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}


}
}

#endif /* LOG_H_ */
