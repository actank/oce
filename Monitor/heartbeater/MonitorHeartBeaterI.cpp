#include "MonitorHeartBeaterI.h"
#include <MonitorAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using monitor::MonitorHeartBeaterI;
using monitor::INTERVAL;
using monitor::RemoveTask;
using monitor::MonitorAlerterAdapter;
using xce::serverstate::ServerStateSubscriber;
using Ice::Current;
using MyUtil::TaskManager;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&MonitorHeartBeaterI::instance(), service.createIdentity("M", ""));
  ServerStateSubscriber::instance().initialize("ControllerMonitorHeartBeater", &MonitorHeartBeaterI::instance(), 0, 5, new MyUtil::OceChannel, "5000", "5000", "1048576");
  MonitorHeartBeaterI::instance().start(128 * 1024).detach();
}

void MonitorHeartBeaterI::remove(const string& name, const Current& current) {
  MCE_INFO("MonitorHeartBeaterI::remove " << name << " size: " << name.size());
  TaskManager::instance().execute(new RemoveTask(name));
}

void MonitorHeartBeaterI::erase(const string& name) {
  MCE_INFO("MonitorHeartBeaterI::erase " << name);
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  beaters_[name] = INT_MAX;
}

void RemoveTask::handle() {
  sleep(10);
  MonitorHeartBeaterI::instance().erase(name_);
}

void MonitorHeartBeaterI::beat(const string& name, const Current& current) {
  MCE_INFO("MonitorHeartBeaterI::beat " << name << " size: " << name.size());
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  beaters_[name] = time(0);
  MCE_DEBUG("MonitorHeartBeaterI::beat " << name << " done");
}

void MonitorHeartBeaterI::run() {
  while (true) {
    try {
      sleep(10);
      map<string, int> beaters;
      {
        beaters = beaters_;
      }
      for (map<string, int>::iterator i = beaters.begin(); i != beaters.end(); ++i) {
        int now = time(0);
        if (now - i->second > INTERVAL) {
          MCE_WARN("MonitorHeartBeaterI::run " << i->first << " size: " << i->first.size() << " overtime " << (now - i->second) << "s");
          MonitorAlerterAdapter::instance().alert(i->first+"_overtime", i->first + " overtime", ALERTERALL, 120);
        }
      }
    } catch (...) {
    }
  }
}
