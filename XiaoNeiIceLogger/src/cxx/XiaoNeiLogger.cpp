#include "XiaoNeiLogger.h"
#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

using namespace com::xiaonei::service::logging;
using namespace std;

extern "C"{
Ice::Logger* createSimple(const Ice::CommunicatorPtr& communicator,
	const Ice::StringSeq& args){
    string name = args.size()>=1 ? args.at(0) : "ServiceI";
    string prop = args.size()>=2 ? args.at(1) : "/opt/XiaoNei/etc/logging.properties";
    Ice::Logger* logger = new SimpleLogger(prop, name, 60);
    return logger;
}
}

SimpleLogger::SimpleLogger(const std::string& prop, const std::string& name, int watchDelay)
	: _logger(log4cxx::Logger::getLogger(name))
{
    log4cxx::PropertyConfigurator::configureAndWatch(prop, watchDelay);
}

SimpleLogger::~SimpleLogger()
{
    // Just needs a virtual destructor
}

void SimpleLogger::print(const ::std::string& message)
{
    LOG4CXX_INFO(_logger, message);
}

void SimpleLogger::trace(const ::std::string& category, const ::std::string& message)
{
    ostringstream oss;
    oss << " [" << category << "] " << message;
    LOG4CXX_DEBUG(_logger, oss.str());
}

void SimpleLogger::warning(const ::std::string& message)
{
    LOG4CXX_WARN(_logger, message);
}

void SimpleLogger::error(const ::std::string& message)
{
    LOG4CXX_ERROR(_logger, message);
}

