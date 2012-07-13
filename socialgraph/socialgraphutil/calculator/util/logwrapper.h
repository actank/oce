// **********************************************************************
//
// Copyright (c) 2005-2006 DuDu, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#ifndef __LOGWRAPPER_I_H__
#define __LOGWRAPPER_I_H__

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>


namespace MyUtil
{

inline void init_logger(
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

#define LOG_INIT(logger, path, level)  \
    MyUtil::init_logger(logger, path, level);

#define LOG_LEVEL(logger, loglevel)  \
    do { \
    	Logger::getInstance(logger).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel)); \
    } while(0);

#define LOG_TRACE(logger, msg)  \
    do { \
        LOG4CPLUS_TRACE(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOG_DEBUG(logger, msg)  \
    do { \
        LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOG_INFO(logger, msg) \
    do { \
        LOG4CPLUS_INFO(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOG_WARN(logger, msg) \
    do { \
       LOG4CPLUS_WARN(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define LOG_ERROR(logger, msg) \
    do { \
        LOG4CPLUS_ERROR(log4cplus::Logger::getInstance(logger), msg) ; \
    } while(0);

#define LOG_FATAL(logger, msg) \
    do { \
        LOG4CPLUS_FATAL(log4cplus::Logger::getInstance(logger), msg); \
    } while(0);

#define MCE_INIT(path, level) LOG_INIT("Mce", path, level);
#define MCE_LEVEL(level) LOG_LEVEL("Mce", level);
#define MCE_TRACE(msg) LOG_TRACE("Mce", msg);
#define MCE_DEBUG(msg) LOG_DEBUG("Mce", msg);
#define MCE_INFO(msg) LOG_INFO("Mce", msg);
#define MCE_WARN(msg) LOG_WARN("Mce", msg);
#define MCE_ERROR(msg) LOG_WARN("Mce", msg);
#define MCE_FATAL(msg) LOG_FATAL("Mce", msg);    
}
#endif

