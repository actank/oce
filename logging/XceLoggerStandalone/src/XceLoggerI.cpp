#include "XceLoggerI.h"

#include "log4cxx/logger.h"
#include "log4cxx/dailyrollingfileappender.h"
#include "log4cxx/patternlayout.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/file.h"

#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace xce::log;

//***************************************************************************

int main(int argc, char* argv[]) {
	XceLoggerService service;
	return service.main(argc, argv);
}

bool XceLoggerService::start(int argc, char* argv[]) {
	std::string name =
				communicator()->getProperties()->getProperty(
						"Service.XceLogger.AdapterName");
	Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter(name);
	std::string logroot =
			communicator()->getProperties()->getPropertyWithDefault(
					"Service.XceLogger.LogRoot", "../log/XceLog");
	std::string config =
			communicator()->getProperties()->getPropertyWithDefault(
					"Service.XceLogger.Config", "");
	adapter->add(new LoggerI(logroot, config), communicator()->stringToIdentity("M"));
	adapter->activate();
	return true;
}

//***************************************************************************

LoggerI::LoggerI(const std::string& logroot, const std::string& config) :
	logroot_(logroot), config_(config) {
	if (!config_.empty()) {
		log4cxx::PropertyConfigurator::configureAndWatch(log4cxx::File(config_));
	}
}

void xce::log::LoggerI::append(const ::xce::log::LogEntries& messages,
		const Ice::Current& current) {
	for (LogEntries::const_iterator it = messages.begin(); it != messages.end(); ++it) {
		if (it->message.empty())
			continue;
		ostringstream category_stream;
		for (StringSeq::const_iterator categories_iterator =
				it->categories.begin(); categories_iterator
				!= it->categories.end(); ++categories_iterator) {
			category_stream << "/" << *categories_iterator;
		}
		string category = category_stream.str();
		{
			initDefault(category);
		}
		if (it->message.at(it->message.length() - 1) == '\n') {
			LOG4CXX_ERROR(log4cxx::Logger::getLogger(category),
					it->message.substr(0, it->message.length() - 1));
		} else {
			LOG4CXX_ERROR(log4cxx::Logger::getLogger(category), it->message);
		}
	}
}

void xce::log::LoggerI::initDefault(const string& category) {
	if (!config_.empty())
		return;
	IceUtil::RWRecMutex::RLock lock(init_mutex_);
	if (inited_loggers_.find(category) == inited_loggers_.end()) {
		lock.upgrade();
		string logpath = logroot_ + "/logger/" + category;
		string::size_type slash_pos = 0;
		do {
			slash_pos = logpath.find_first_of("/", slash_pos + 1);
			cout << "mkdir " << logpath.substr(0, slash_pos) << endl;
			mkdir(logpath.substr(0, slash_pos).c_str(), S_IRWXU | S_IRWXG
					| S_IROTH | S_IXOTH);
		} while (slash_pos < logpath.find_last_of("/"));
		log4cxx::AppenderPtr appender = new log4cxx::DailyRollingFileAppender(
				log4cxx::PatternLayoutPtr(new log4cxx::PatternLayout(
						"\%d{mm:ss,SSS} \%m\%n")), logpath, "'.'yyyy-MM-dd-HH");
		log4cxx::BasicConfigurator::configure(appender);
		log4cxx::Logger::getLogger(category)->addAppender(appender);

		inited_loggers_.insert(category);
	}
}
