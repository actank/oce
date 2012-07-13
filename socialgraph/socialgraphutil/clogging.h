#ifndef SG_CLOGGING_H_
#define SG_CLOGGING_H_

#include <unistd.h>
#include <sys/stat.h>

#include <iostream>
#include <sstream>
#include <string>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>

namespace xce {
namespace socialgraph {

enum CLoggerSeverity { TRACE = log4cplus::TRACE_LOG_LEVEL,
											 DEBUG = log4cplus::DEBUG_LOG_LEVEL,
											 INFO = log4cplus::INFO_LOG_LEVEL,
											 WARN = log4cplus::WARN_LOG_LEVEL,
											 ERROR = log4cplus::ERROR_LOG_LEVEL,
											 FATAL = log4cplus::FATAL_LOG_LEVEL
										 };

//--------------------------------------------------------------

class CLogger {
public:
	CLogger(const CLoggerSeverity& severity = INFO)
		:severity_(severity) { 
		if (!log4cplus::Logger::exists("sglog")) {		//if not to figure out properties, use default configuration
			log4cplus::BasicConfigurator config;
			config.configure();
		}
	}

	~CLogger() {
		switch (severity_) {
			case TRACE:	
				LOG4CPLUS_TRACE(log4cplus::Logger::getInstance("sglog"), stream_.str());
				break;

			case DEBUG:
				LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance("sglog"), stream_.str());
				break;

			case INFO:
				LOG4CPLUS_INFO(log4cplus::Logger::getInstance("sglog"), stream_.str());
				break;

			case WARN:
				LOG4CPLUS_WARN(log4cplus::Logger::getInstance("sglog"), stream_.str());
				break;

			case ERROR:
				LOG4CPLUS_ERROR(log4cplus::Logger::getInstance("sglog"), stream_.str());
				break;

			case FATAL:
				LOG4CPLUS_FATAL(log4cplus::Logger::getInstance("sglog"), stream_.str());
				break;
		}
	}

	std::ostream& Stream() { return stream_; };

private:
	CLoggerSeverity severity_;
	std::ostringstream stream_;
};

//--------------------------------------------------------------

inline void InitLogger(const std::string& biz) {
	const int max_length = 512;				// max length of absolute path 
	char absolute_path[max_length];
	getcwd(absolute_path, max_length);
	std::string path(absolute_path);
	std::string substr = "SGProject";
	size_t pos = path.find(substr);	
	std::string root_path = path.substr(0, pos + substr.size());

	std::string conf_file = root_path + "/conf/log4cplus.properties";
	if (access(conf_file.c_str(), F_OK) == -1) {			//whether configure file exists
		return;
	}

	log4cplus::helpers::Properties properties(conf_file);

	std::string target_log_directory = root_path + "/logs/" + biz;
	if (access(target_log_directory.c_str(), F_OK) == -1) {				//the target_log_directory does not exist, then create it
		mkdir(target_log_directory.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);			//create log directory
	}
	
	std::string target_log_file = target_log_directory + "/" + biz + ".log"; 
	std::string key = "log4cplus.appender.fileAppender.File";
	properties.setProperty(key, target_log_file);

	log4cplus::PropertyConfigurator config(properties);
	config.configure();
}

//--------------------------------------------------------------

#define INIT_CLOG(biz) \
	InitLogger(biz);

#define CLOG(sev) \
	xce::socialgraph::CLogger(sev).Stream()

}
}

#endif 
