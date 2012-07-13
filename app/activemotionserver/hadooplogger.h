#ifndef __HADOOP_LOGGER_H__
#define __HADOOP_LOGGER_H__

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>


namespace cwf {
namespace socialgraph {

inline void init_hadoop_logger(
	const std::string& name, 
	const std::string& path, 
	const std::string& loglevel = "WARN",
	const std::string& pattern = "%m%n")
{
	log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::HOURLY , 1, true));
	append_1->setName(name);
	append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
	log4cplus::Logger::getInstance(name).addAppender(append_1);
	log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

#define HADOOP_LOG_INIT(logger, path, level)  \
    cwf::socialgraph::init_hadoop_logger(logger, path, level);

#define HADOOP_LOG_LEVEL(logger, loglevel)  \
    do { \
    	Logger::getInstance(logger).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel)); \
    } while(0);

#define HADOOP_LOG_TRACE(logger, msg)  \
    do { \
        LOG4CPLUS_TRACE(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HADOOP_LOG_DEBUG(logger, msg)  \
    do { \
        LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HADOOP_LOG_INFO(logger, msg) \
    do { \
        LOG4CPLUS_INFO(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HADOOP_LOG_WARN(logger, msg) \
    do { \
       LOG4CPLUS_WARN(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HADOOP_LOG_ERROR(logger, msg) \
    do { \
        LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(logger), msg) ; \
    } while(0);

#define HADOOP_LOG_FATAL(logger, msg) \
    do { \
        LOG4CPLUS_FATAL(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HADOOP_INIT(path, level) HADOOP_LOG_INIT("Hadoop", path, level);
#define HADOOP_LEVEL(level) HADOOP_LOG_LEVEL("Hadoop", level);
#define HADOOP_TRACE(msg) HADOOP_LOG_TRACE("Hadoop", msg);
#define HADOOP_DEBUG(msg) HADOOP_LOG_DEBUG("Hadoop", msg);
#define HADOOP_INFO(msg) HADOOP_LOG_INFO("Hadoop", msg);
#define HADOOP_WARN(msg) HADOOP_LOG_WARN("Hadoop", msg);
#define HADOOP_ERROR(msg) HADOOP_LOG_WARN("Hadoop", msg);
#define HADOOP_FATAL(msg) HADOOP_LOG_FATAL("Hadoop", msg);    

    
}
}
#endif

