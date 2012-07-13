#ifndef _LOCAL_LOGGER_HEADER_
#define _LOCAL_LOGGER_HEADER_

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>

inline void init_local_logger(
	const std::string& name, 
	const std::string& path, 
	long max_file_size, 
	int max_backup_index, 
	const std::string& loglevel = "WARN",
	const std::string& pattern = "%m%n")
{
	log4cplus::SharedAppenderPtr append_1(new log4cplus::RollingFileAppender(path, 10*1024*1024, 100000));
	append_1->setName(name);
	append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
	log4cplus::Logger::getInstance(name).addAppender(append_1);
	log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

#define LOCAL_LOG_INIT(logger, path, max_file_size, max_backup_index, level)  \
    init_local_logger(logger, path, max_file_size, max_backup_index, level);

#define LOCAL_LOG_LEVEL(logger, loglevel)  \
    do { \
    	Logger::getInstance(logger).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel)); \
    } while(0);

#define LOCAL_LOG_TRACE(logger, msg)  \
    do { \
        LOG4CPLUS_TRACE(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOCAL_LOG_DEBUG(logger, msg)  \
    do { \
        LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOCAL_LOG_INFO(logger, msg) \
    do { \
        LOG4CPLUS_INFO(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOCAL_LOG_WARN(logger, msg) \
    do { \
       LOG4CPLUS_WARN(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOCAL_LOG_ERROR(logger, msg) \
    do { \
        LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(logger), msg) ; \
    } while(0);

#define LOCAL_LOG_FATAL(logger, msg) \
    do { \
        LOG4CPLUS_FATAL(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOCAL_INIT(path, max_file_size, max_backup_index, level) LOCAL_LOG_INIT("Local", path, max_file_size, max_backup_index, level);
#define LOCAL_LEVEL(level) LOCAL_LOG_LEVEL("Local", level);
#define LOCAL_TRACE(msg) LOCAL_LOG_TRACE("Local", msg);
#define LOCAL_DEBUG(msg) LOCAL_LOG_DEBUG("Local", msg);
#define LOCAL_INFO(msg) LOCAL_LOG_INFO("Local", msg);
#define LOCAL_WARN(msg) LOCAL_LOG_WARN("Local", msg);
#define LOCAL_ERROR(msg) LOCAL_LOG_WARN("Local", msg);
#define LOCAL_FATAL(msg) LOCAL_LOG_FATAL("Local", msg);    

#endif
