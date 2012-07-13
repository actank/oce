#ifndef SERVICE_MONITOR_H
#define SERVICE_MONITOR_H

#include <Singleton.h>
#include <vector>
#include <IceUtil/IceUtil.h>
#include <MonitorAdapter.h>
#include "util/cpp/InvokeMonitor.h"
#include <sys/resource.h> 
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <ServiceI.h>

namespace MyUtil {

class MonitorThread : public IceUtil::Thread {
public:
  MonitorThread(int time, const std::string& subject) : time_(time), subject_(subject) {}
  MonitorThread(const std::string& subject) : time_(10), subject_(subject) {}
  void run() {
    while (true) {
      try {
        sleep(time_);
        std::vector<std::string> datas = fetch();
        for (std::vector<std::string>::iterator i = datas.begin(); i != datas.end(); ++i) {
          report(*i);
        }
      } catch (const Ice::Exception& e) {
        MCE_WARN("MonitorThread::run Ice::Exception " << e);
      } catch (const std::exception& e)  {
        MCE_WARN("MonitorThread::run std::exception " << e.what());
      } catch (...) {
        MCE_WARN("MonitorThread::run unknown");
      }
    }
  }
  virtual std::vector<std::string> fetch() = 0;
private:
  void report(const std::string& msg) {
    ostringstream oss;
    oss << "NAME: " << ServiceI::instance().getName()
        << " BUSINESS: " << ServiceI::instance().getBusiness()
        << " " << msg;
    monitor::MonitorDispatcherAdapter::instance().report(subject_, oss.str());
  }
  int time_;
  std::string subject_;
};

class LogMonitorThread : public MonitorThread {
public:
  LogMonitorThread() : MonitorThread("MCE_LOG") {}
  virtual ~LogMonitorThread() {}
  std::vector<std::string> fetch() {
    int count[6];
    LogMonitor::instance().fetch(count);
    ostringstream oss;
    oss << "TRACE: " << count[0]
        << " DEBUG: " << count[1]
        << " INFO: " << count[2]
        << " WARN: " << count[3]
        << " ERROR: " << count[4]
        << " FATAL: " << count[5];
    std::vector<std::string> res;
    res.push_back(oss.str());
    return res;
  }
};

class InvokeMonitorThread : public MonitorThread {
public:
  InvokeMonitorThread() : MonitorThread("SERVICE_INVOKE") {}
  virtual ~InvokeMonitorThread() {}
  std::vector<std::string> fetch() {
    std::map<std::string, InvokeStatePtr> states;
    InvokeMonitor::instance().fetch(states);
    std::vector<std::string> res;
    for (std::map<std::string, InvokeStatePtr>::iterator i = states.begin(); i != states.end(); ++i) {
      ostringstream oss;
      oss << i->first << " Count: " << i->second->count() << " TotalCost: " << i->second->totalcost() << " MaxCost: " << i->second->maxcost() << " Unit: us";
      res.push_back(oss.str());
    }
    return res;
  }
};

class LiveMonitorThread : public MonitorThread {
public:
  LiveMonitorThread(int time) : MonitorThread("LIVE"), time_(time) {}
  virtual ~LiveMonitorThread() {}
  std::vector<std::string> fetch() {
  	ostringstream oss;
    oss << " starttime: " << time_;
    std::vector<std::string> res;
    res.push_back(oss.str());
    return res;
  }
private:
  int time_;
};

class MemMonitorThread : public MonitorThread {
public:
  MemMonitorThread() : MonitorThread("SERVICE_MEM") {
  }
  virtual ~MemMonitorThread() {}

  std::vector<std::string> fetch() {
    std::vector<std::string> res;
    struct rusage r;
    int ret = getrusage(RUSAGE_SELF, &r);
    if (!ret) {
  	  ostringstream oss;
      oss << " utime.sec: " << r.ru_utime.tv_sec << " utime.usec: " << r.ru_utime.tv_usec
          << " stime.sec: " << r.ru_stime.tv_sec << " stime.usec: " << r.ru_stime.tv_usec
          << " maxrss: "  << r.ru_maxrss
          << " ixrss: " << r.ru_ixrss
          << " idrss: " << r.ru_idrss
          << " isrss: " << r.ru_isrss
          << " minflt: "  << r.ru_minflt
          << " majflt: "  << r.ru_majflt
          << " nswap: " << r.ru_nswap
          << " inblock: " << r.ru_inblock
          << " oublock: " << r.ru_oublock
          << " msgsnd: "  << r.ru_msgsnd
          << " msgrcv: "  << r.ru_msgrcv
          << " nsignals: "  << r.ru_nsignals
          << " nvcsw: " << r.ru_nvcsw
          << " nivcsw: "  << r.ru_nivcsw;
      res.push_back(oss.str());
    }
    return res;
  }
};

class CpuMonitorThread : public MonitorThread{
public:
  CpuMonitorThread(): MonitorThread("SERVICE_CPU") {
		pid_ = getpid();
    numCore_ = getNumCore();
		string content=readFile("/proc/stat");
    last_total_ = getTotal();
    last_process_ = getProcess();
  }

  virtual ~CpuMonitorThread() {}

  std::vector<std::string> fetch() {
      vector<string> res;
  	  ostringstream oss;
      long total = getTotal();
      long process = getProcess();
      long rate = (process - last_process_) * numCore_ * 100 / (total - last_total_);
      last_total_ = total;
      last_process_ = process;
      oss << "RATE: " << rate;
      res.push_back(oss.str());
      return res;
  }

private:
  int pid_;
  int numCore_;
  long last_total_;
  long last_process_;

  long getProcess() {
    ostringstream oss;
    oss << "/proc/" << pid_ << "/stat";
    string file = readFile(oss.str().c_str());
    vector<string> ss;
    boost::algorithm::split(ss, file, boost::algorithm::is_any_of(" "));
    long sum = 0;
    for (int i=13;i<17;++i) {
      sum += boost::lexical_cast<long>(ss[i]);
    }
    return sum;
  }
  
  long getTotal() {
    string file = readFile("/proc/stat");
    vector<string> ss;
    boost::algorithm::split(ss, file, boost::algorithm::is_any_of("\n"));
    for (vector<string>::iterator i = ss.begin(); i != ss.end(); ++i) {
      if (!boost::find_first(*i, "cpu ").empty()) {
        vector<string> cpus;
        boost::algorithm::split(cpus, *i, boost::algorithm::is_any_of(" "));
        long sum = 0;
        for (size_t j = 2; j < cpus.size(); ++j) {
          sum += boost::lexical_cast<long>(cpus[j]);
        }
        return sum;
      }
    }
    return 0;
  }

  int getNumCore(){ 
		string file = readFile("/proc/cpuinfo");
    deque<string> strings;
    boost::find_all(strings, file, "processor");
    return strings.size();
  }

  std::string readFile(const std::string& file) {
    ifstream stream(file.c_str());
    ostringstream oss;
    string line;
    while (stream.good()) {
      getline(stream, line);
      oss << line << std::endl;
    }
    stream.close();
    return oss.str();
  }

};

class ServiceMonitorManager : public MyUtil::Singleton<ServiceMonitorManager>{
public:
  ServiceMonitorManager(const int timestamp = time(NULL)) {
    threads_.push_back(new LogMonitorThread);
//    threads_.push_back(new LiveMonitorThread(timestamp));
    threads_.push_back(new InvokeMonitorThread);
    threads_.push_back(new MemMonitorThread);
    threads_.push_back(new CpuMonitorThread);
    //threads_.push_back(new SamplerMonitorThread);
  }
  void start() {
	  //rce::monitor::SamplerClient::instance().initialize();
    for (vector<IceUtil::ThreadPtr>::iterator i = threads_.begin(); i != threads_.end(); ++i) {
      (*i)->start(128 * 1024).detach();
	}

  }
private:
  std::vector<IceUtil::ThreadPtr> threads_;
};


}

#endif
