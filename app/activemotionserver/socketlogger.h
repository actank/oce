#ifndef __SOCKET_LOGGER_H__
#define __SOCKET_LOGGER_H__

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/socketappender.h>

namespace cwf {
namespace socialgraph {

inline void init_socket_logger(
		const std::string& name, 
		const std::string& host, 
		int port, 
		const std::string& loglevel = "WARN") {
	log4cplus::SharedAppenderPtr append_1(new log4cplus::SocketAppender(host, port));
	append_1->setName(name);
	log4cplus::Logger::getInstance(name).addAppender(append_1);
	log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

#define SOCKET_LOG_INIT(logger, host, port, level)  \
    cwf::socialgraph::init_socket_logger(logger, host, port, level);

#define SOCKET_LOG_LEVEL(logger, loglevel)  \
    do { \
    	Logger::getInstance(logger).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel)); \
    } while(0);

#define SOCKET_LOG_TRACE(logger, msg)  \
    do { \
        LOG4CPLUS_TRACE(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define SOCKET_LOG_DEBUG(logger, msg)  \
    do { \
        LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define SOCKET_LOG_INFO(logger, msg) \
    do { \
        LOG4CPLUS_INFO(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define SOCKET_LOG_WARN(logger, msg) \
    do { \
       LOG4CPLUS_WARN(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define SOCKET_LOG_ERROR(logger, msg) \
    do { \
        LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(logger), msg) ; \
    } while(0);

#define SOCKET_LOG_FATAL(logger, msg) \
    do { \
        LOG4CPLUS_FATAL(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define SOCKET_INIT(host, port, level) SOCKET_LOG_INIT("Socket", host, port, level);
#define SOCKET_LEVEL(level) SOCKET_LOG_LEVEL("Socket", level);
#define SOCKET_TRACE(msg) SOCKET_LOG_TRACE("Socket", msg);
#define SOCKET_DEBUG(msg) SOCKET_LOG_DEBUG("Socket", msg);
#define SOCKET_INFO(msg) SOCKET_LOG_INFO("Socket", msg);
#define SOCKET_WARN(msg) SOCKET_LOG_WARN("Socket", msg);
#define SOCKET_ERROR(msg) SOCKET_LOG_WARN("Socket", msg);
#define SOCKET_FATAL(msg) SOCKET_LOG_FATAL("Socket", msg);    

    
}
}
#endif

