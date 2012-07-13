#ifndef MONITORADAPTER_H
#define MONITORADAPTER_H

#include <Monitor.h>
#include <AdapterI.h>

namespace monitor {


class MonitorDispatcherAdapter : public MyUtil::ReplicatedClusterAdapterI<MonitorDispatcherPrx>, public MyUtil::Singleton<MonitorDispatcherAdapter> {
public:
  MonitorDispatcherAdapter() : MyUtil::ReplicatedClusterAdapterI <MonitorDispatcherPrx> ("ControllerMonitorDispatcher", 120, 300) {}
  void report(const std::string& type, const std::string& message);
};

class MonitorAlerterAdapter : public MyUtil::ReplicatedClusterAdapterI<MonitorAlerterPrx>, public MyUtil::Singleton<MonitorAlerterAdapter> {
public:
  MonitorAlerterAdapter() : MyUtil::ReplicatedClusterAdapterI <MonitorAlerterPrx> ("ControllerMonitorAlerter", 120, 300) {}
  void alert(const std::string& subject, const std::string& msg, int type, int interval);
  void reload();
};
class MonitorHeartBeaterAdapter : public MyUtil::ReplicatedClusterAdapterI<MonitorHeartBeaterPrx>, public MyUtil::Singleton<MonitorHeartBeaterAdapter> {
public:
  MonitorHeartBeaterAdapter() : MyUtil::ReplicatedClusterAdapterI <MonitorHeartBeaterPrx> ("ControllerMonitorHeartBeater", 120, 300) {}
  void beat(const std::string& subject);
  void remove(const std::string& subject);
};
class MonitorAnalyzerAdapter : public MyUtil::ReplicatedClusterAdapterI<MonitorAnalyzerPrx>, public MyUtil::Singleton<MonitorAnalyzerAdapter> {
public:
  MonitorAnalyzerAdapter() : MyUtil::ReplicatedClusterAdapterI <MonitorAnalyzerPrx> ("ControllerMonitorAnalyzer", 120, 300) {}
  void analyse(const std::string& subject, const std::string& msg);
  void reload();
};


}

#endif

