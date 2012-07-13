#ifndef INVOKEMONITOR_H
#define INVOKEMONITOR_H

#include <Singleton.h>
#include <IceUtil/RWRecMutex.h>

namespace MyUtil {

class InvokeState;

typedef IceUtil::Handle<InvokeState> InvokeStatePtr;

class InvokeState : public Ice::Object {
public:
  InvokeState() : count_(1), totalcost_(0), maxcost_(0) {}
  InvokeState(const InvokeStatePtr& state) : count_(state->count_), totalcost_(state->totalcost_), maxcost_(state->maxcost_) {}
  void incCount(int count) {count_ += count;}
  void incTimeCost(long totalcost) {
    totalcost_ += totalcost;
    if (totalcost > maxcost_) {
      maxcost_ = totalcost;
    }
  }
  int count() const {return count_;}
  long totalcost() const {return totalcost_;}
  long maxcost() const {return maxcost_;}
  InvokeStatePtr reset() {
    InvokeStatePtr res = new InvokeState(this);
    count_ = 0; 
    totalcost_ = 0; 
    maxcost_ = 0;
    return res;
  }
private:
  int count_;
  long totalcost_;
  long maxcost_;
};

class InvokeMonitor : public MyUtil::Singleton<InvokeMonitor> {
public:
  void report(const std::string& key, long time) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    std::map<std::string, InvokeStatePtr>::iterator i = states_.find(key);
    if (i == states_.end()) {
      mutex_.upgrade();
      if (states_.find(key) == states_.end()) {
        states_.insert(make_pair<std::string, InvokeStatePtr>(key, new InvokeState));
      }
      mutex_.downgrade();
    }
    states_[key]->incCount(1);
    states_[key]->incTimeCost(time);
  }

  void fetch(std::map<std::string, InvokeStatePtr>& states) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    for (std::map<std::string, InvokeStatePtr>::iterator i = states_.begin(); i != states_.end(); ++i) {
      states.insert(make_pair<std::string, InvokeStatePtr>(i->first, i->second->reset()));
    }
  }
private:
  InvokeMonitor(){}
  friend class MyUtil::Singleton<InvokeMonitor>;

  std::map<std::string, InvokeStatePtr> states_;
  IceUtil::RWRecMutex mutex_;
};

class InvokeClient {
public:
  const static int INVOKE_LEVEL_NONE = 0;
  const static int INVOKE_LEVEL_DEBUG = 1;
  const static int INVOKE_LEVEL_INFO = 2;
  const static int INVOKE_LEVEL_WARN = 3;
  const static int INVOKE_LEVEL_ERROR = 4;

  static InvokeClient create(const Ice::Current& current, const std::string& content = "", int level = INVOKE_LEVEL_NONE) {
    InvokeClient client(current, content, level);
    client.start();
    return client;
  }

  ~InvokeClient() {
    ostringstream os;
    long cost = get_now() - start_;
    os << "Invoke: " << (current_.con ? current_.operation : "none")
        << " Client: " << (current_.con ? current_.con->toString().substr(current_.con->toString().find("remote",25)).substr(17):"localhost:-1" )
        << " TimeCost: "<< cost <<" us" << " Message: " << (content_ == "" ? "none" : content_);
    log(os.str());
    if (current_.con) {
      InvokeMonitor::instance().report(current_.operation, cost);
    }
  }

private:
  Ice::Current current_;
  std::string content_;
  long start_;
  long last_point_;
  int level_;

  InvokeClient(const Ice::Current& current, const std::string& content, int level) : current_(current), content_(content), level_(level) {}

  void log(const string& data) {
    switch (level_) {
      case INVOKE_LEVEL_NONE:
        break;
      case INVOKE_LEVEL_DEBUG:
        MCE_DEBUG(data);
        break;
      case INVOKE_LEVEL_INFO:
        MCE_INFO(data);
        break;
      case INVOKE_LEVEL_WARN:
        MCE_WARN(data);
        break;
      case INVOKE_LEVEL_ERROR:
        MCE_ERROR(data);
        break;
      default:
        MCE_DEBUG(data);
    }
  }

  void start() {
    start_ = get_now();
    last_point_ = start_;
  }

  long get_now() {
    struct timeval tv;
    memset(&tv, 0, sizeof(tv));
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000000+tv.tv_usec;
  }
};

}

#endif
