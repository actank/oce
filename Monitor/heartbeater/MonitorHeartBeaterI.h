#ifndef MONITORHEARTBEATERI_H
#define MONITORHEARTBEATERI_H

#include <ServiceI.h>
#include <Monitor.h>

namespace monitor {

const int INTERVAL = 120;

class MonitorHeartBeaterI : public MonitorHeartBeater, public MyUtil::Singleton<MonitorHeartBeaterI>, public IceUtil::Thread {
public:
  virtual ~MonitorHeartBeaterI() {}
  void beat(const std::string& name, const Ice::Current& = Ice::Current());
  void remove(const std::string& name, const Ice::Current& = Ice::Current());
  void erase(const std::string& name);
  void run();
private:

  std::map<std::string,int> beaters_;
  IceUtil::Monitor<IceUtil::Mutex> mutex_;  
};

class RemoveTask : public MyUtil::Task {
public:
  RemoveTask(const std::string& name) : name_(name) {}
  virtual ~RemoveTask() {}
  void handle();
private:
  std::string name_;
};

}

#endif
