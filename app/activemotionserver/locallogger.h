#ifndef __LOCAL_LOGGER_H__
#define __LOCAL_LOGGER_H__

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>


namespace cwf {
namespace socialgraph {

inline void init_local_logger(
	const std::string& name, 
	const std::string& path, 
	const std::string& loglevel = "WARN",
	const std::string& pattern = "%-10D{%H:%M:%S.%q} : [%p] : %m%n")
{
	log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::DAILY, 1, true));
	append_1->setName(name);
	append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
	log4cplus::Logger::getInstance(name).addAppender(append_1);
	log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

#define FCGI_LOG_INIT(logger, path, level)  \
    cwf::socialgraph::init_local_logger(logger, path, level);

#define FCGI_LOG_LEVEL(logger, loglevel)  \
    do { \
    	Logger::getInstance(logger).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel)); \
    } while(0);

#define FCGI_LOG_TRACE(logger, msg)  \
    do { \
        LOG4CPLUS_TRACE(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define FCGI_LOG_DEBUG(logger, msg)  \
    do { \
        LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define FCGI_LOG_INFO(logger, msg) \
    do { \
        LOG4CPLUS_INFO(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define FCGI_LOG_WARN(logger, msg) \
    do { \
       LOG4CPLUS_WARN(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define FCGI_LOG_ERROR(logger, msg) \
    do { \
        LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(logger), msg) ; \
    } while(0);

#define FCGI_LOG_FATAL(logger, msg) \
    do { \
        LOG4CPLUS_FATAL(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define FCGI_INIT(path, level) FCGI_LOG_INIT("Fcgi", path, level);
#define FCGI_LEVEL(level) FCGI_LOG_LEVEL("Fcgi", level);
#define FCGI_TRACE(msg) FCGI_LOG_TRACE("Fcgi", msg);
#define FCGI_DEBUG(msg) FCGI_LOG_DEBUG("Fcgi", msg);
#define FCGI_INFO(msg) FCGI_LOG_INFO("Fcgi", msg);
#define FCGI_WARN(msg) FCGI_LOG_WARN("Fcgi", msg);
#define FCGI_ERROR(msg) FCGI_LOG_WARN("Fcgi", msg);
#define FCGI_FATAL(msg) FCGI_LOG_FATAL("Fcgi", msg);    


#define HUODONG_LOG_INIT(logger, path, level)  \
    cwf::socialgraph::init_local_logger(logger, path, level);

#define HUODONG_LOG_LEVEL(logger, loglevel)  \
    do { \
    	Logger::getInstance(logger).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel)); \
    } while(0);

#define HUODONG_LOG_TRACE(logger, msg)  \
    do { \
        LOG4CPLUS_TRACE(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HUODONG_LOG_DEBUG(logger, msg)  \
    do { \
        LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HUODONG_LOG_INFO(logger, msg) \
    do { \
        LOG4CPLUS_INFO(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HUODONG_LOG_WARN(logger, msg) \
    do { \
       LOG4CPLUS_WARN(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HUODONG_LOG_ERROR(logger, msg) \
    do { \
        LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(logger), msg) ; \
    } while(0);

#define HUODONG_LOG_FATAL(logger, msg) \
    do { \
        LOG4CPLUS_FATAL(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define HUODONG_INIT(path, level) HUODONG_LOG_INIT("HuoDong", path, level);
#define HUODONG_LEVEL(level) HUODONG_LOG_LEVEL("HuoDong", level);
#define HUODONG_TRACE(msg) HUODONG_LOG_TRACE("HuoDong", msg);
#define HUODONG_DEBUG(msg) HUODONG_LOG_DEBUG("HuoDong", msg);
#define HUODONG_INFO(msg) HUODONG_LOG_INFO("HuoDong", msg);
#define HUODONG_WARN(msg) HUODONG_LOG_WARN("HuoDong", msg);
#define HUODONG_ERROR(msg) HUODONG_LOG_WARN("HuoDong", msg);
#define HUODONG_FATAL(msg) HUODONG_LOG_FATAL("HuoDong", msg);    

    
}
}
#endif

