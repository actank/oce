#ifndef MONITORDISPATCHERI_H
#define MONITORDISPATCHERI_H

#include <Monitor.h>
#include <ServiceI.h>

namespace monitor {

class MonitorDispatcherI : public MonitorDispatcher, public MyUtil::Singleton<MonitorDispatcherI> {
public:
  virtual void report(const std::string& type, const std::string& message, const Ice::Current& = Ice::Current());
};

}

#endif
