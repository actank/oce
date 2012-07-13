#ifndef MONITORANALYZERI_H
#define MONITORANALYZERI_H

#include <ServiceI.h>
#include <Monitor.h>

namespace monitor {

const std::string MASHINE_MEMORY = "MASHINE_MEMORY";
const std::string MASHINE_DISK = "MASHINE_DISK";
const std::string MASHINE_LOAD = "MASHINE_LOAD";
const std::string CONFIG = "../etc/analyzer.config";
const int ALERTSMSINTERVAL = 900;
const int ALERTEMAILINTERVAL = 300;

class Limiter : public Ice::Object {
public:
  Limiter(int moremin, int min, int max, int moremax) : moremin_(moremin), min_(min), max_(max), moremax_(moremax) {}
  int min() const {return min_;}
  int max() const {return max_;}
  int moremax() const {return moremax_;}
  int moremin() const {return moremin_;}
private:
  int moremin_;
  int min_;
  int max_;
  int moremax_;
};

typedef IceUtil::Handle<Limiter> LimiterPtr;

class Analyzer : public Ice::Object {
public:
  Analyzer(const std::string& type) : type_(type) {}
  virtual ~Analyzer() {}
  void reload(const Ice::PropertiesPtr& properties);
  void analyse(const std::string& message);
  virtual std::pair<std::string, int> parse(const std::string& message) = 0;
  virtual void alert(const std::string& key, int value, int type) = 0;

protected:
  std::string type_;
  LimiterPtr default_;
  std::map<std::string, LimiterPtr> limits_;
  IceUtil::RWRecMutex mutex_;
};

typedef IceUtil::Handle<Analyzer> AnalyzerPtr;

class MashineLoadAnalyzer : public Analyzer {
public:
  MashineLoadAnalyzer() : Analyzer("MashineLoad") {}
  virtual ~MashineLoadAnalyzer() {}
  virtual std::pair<std::string, int> parse(const std::string& message);
  virtual void alert(const std::string& key, int value, int type);
};

class MashineDiskAnalyzer : public Analyzer {
public:
  MashineDiskAnalyzer() : Analyzer("MashineDisk") {}
  virtual ~MashineDiskAnalyzer() {}
  virtual std::pair<std::string, int> parse(const std::string& message);
  virtual void alert(const std::string& key, int value, int type);
};

class MashineAvailableMemoryAnalyzer : public Analyzer {
public:
  MashineAvailableMemoryAnalyzer() : Analyzer("MashineAvailableMemory") {}
  virtual ~MashineAvailableMemoryAnalyzer() {}
  virtual std::pair<std::string, int> parse(const std::string& message);
  virtual void alert(const std::string& key, int value, int type);
};

class MonitorAnalyzerI : public MonitorAnalyzer, public MyUtil::Singleton<MonitorAnalyzerI> {
public:
  MonitorAnalyzerI();
  virtual ~MonitorAnalyzerI() {}
  virtual void reload(const Ice::Current& = Ice::Current());
  virtual void analyse(const std::string& type, const std::string& message, const Ice::Current& = Ice::Current());
private:
  std::string configfile_;

  AnalyzerPtr mashineavailablememoryanalyzer_;
  AnalyzerPtr mashineloadanalyzer_;
  AnalyzerPtr mashinediskanalyzer_;
};

}

#endif
