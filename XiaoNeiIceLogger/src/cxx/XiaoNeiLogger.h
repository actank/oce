#ifndef XIAONEILOGGER_H_
#define XIAONEILOGGER_H_

#include <Ice/Ice.h>

extern "C"
{
    Ice::Logger* createSimple(const Ice::CommunicatorPtr&, const Ice::StringSeq&);
}

#include <log4cxx/logger.h>
namespace com
{
namespace xiaonei
{
namespace service
{
namespace logging
{
class SimpleLogger : virtual public Ice::Logger
{
public:
    SimpleLogger(const ::std::string& prop, const ::std::string& name, int watchDelay);
    virtual ~SimpleLogger();
    virtual void print(const ::std::string& message);
    virtual void trace(const ::std::string& category, const ::std::string& message);
    virtual void warning(const ::std::string& message);
    virtual void error(const ::std::string& message);
private:
    log4cxx::LoggerPtr _logger;
};

}
}
}
}
#endif /*XIAONEILOGGER_H_*/
