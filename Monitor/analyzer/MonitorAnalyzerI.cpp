#include "MonitorAnalyzerI.h"
#include <MonitorAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>

using monitor::MonitorAnalyzerI;
using monitor::Analyzer;
using monitor::MashineAvailableMemoryAnalyzer;
using monitor::MashineLoadAnalyzer;
using monitor::MashineDiskAnalyzer;
using monitor::MonitorAlerterAdapter;
using xce::serverstate::ServerStateSubscriber;
using Ice::Current;
using Ice::PropertiesPtr;
using Ice::PropertyDict;
using boost::lexical_cast;
using monitor::Limiter;
using monitor::LimiterPtr;
using IceUtil::RWRecMutex;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&MonitorAnalyzerI::instance(), service.createIdentity("M", ""));
  ServerStateSubscriber::instance().initialize("ControllerMonitorAnalyzer", &MonitorAnalyzerI::instance(), 0, 5, new MyUtil::OceChannel, "5000", "5000", "1048576");
  MonitorAnalyzerI::instance().reload();
}

MonitorAnalyzerI::MonitorAnalyzerI() {
  mashineavailablememoryanalyzer_ = new MashineAvailableMemoryAnalyzer();
  mashinediskanalyzer_ = new MashineDiskAnalyzer();
  mashineloadanalyzer_ = new MashineLoadAnalyzer();
}

void MonitorAnalyzerI::reload(const Current& current) {
  MCE_INFO("MonitorAnalyzerI::reload");
  PropertiesPtr properties = Ice::createProperties();
  properties->load(CONFIG);
  mashineavailablememoryanalyzer_->reload(properties);
  mashineloadanalyzer_->reload(properties);
  mashinediskanalyzer_->reload(properties);
}

void MonitorAnalyzerI::analyse(const string& type, const string& message, const Current& current) {
  MCE_INFO("MonitorAnalyzerI::analyse subject: " << type << " size: " << type.size() << " " << message);
  if (type == MASHINE_MEMORY) {
    mashineavailablememoryanalyzer_->analyse(message);
  } else if (type == MASHINE_LOAD) {
    mashineloadanalyzer_->analyse(message);
  } else if (type == MASHINE_DISK) {
    mashinediskanalyzer_->analyse(message);
  }
}

void Analyzer::reload(const PropertiesPtr& properties) {
  MCE_INFO("Analyzer::reload type: " << type_);
  int defaultmin = properties->getPropertyAsIntWithDefault("Analyzer."+type_+".Default.Min", INT_MIN);
  int defaultmax = properties->getPropertyAsIntWithDefault("Analyzer."+type_+".Default.Max", INT_MAX);
  int defaultmoremin = properties->getPropertyAsIntWithDefault("Analyzer."+type_+".Default.MoreMin", INT_MIN);
  int defaultmoremax = properties->getPropertyAsIntWithDefault("Analyzer."+type_+".Default.MoreMax", INT_MAX);
  LimiterPtr defaulter = new Limiter(defaultmoremin, defaultmin, defaultmax, defaultmoremax);

  map<string, LimiterPtr> limits;
  PropertyDict patterns = properties->getPropertiesForPrefix("Analyzer."+type_+".Patterns");
  for (PropertyDict::iterator pattern = patterns.begin(); pattern != patterns.end(); ++pattern) {
    vector<string> strings;
    boost::algorithm::split(strings, pattern->second, boost::algorithm::is_any_of(" "));
    limits[strings.at(0)]=new Limiter(lexical_cast<int>(strings.at(1)),lexical_cast<int>(strings.at(2)),lexical_cast<int>(strings.at(3)),lexical_cast<int>(strings.at(4)));
  }
  {
    RWRecMutex::WLock lock(mutex_);
    limits_ = limits;
    default_ = defaulter;
  }
  MCE_DEBUG("Analyzer::reload done");
}

void Analyzer::analyse(const string& message) {
  MCE_DEBUG("Analyzer::analyse message: " << message);
  pair<string, int> result = parse(message);
  map<string, LimiterPtr> limits;
  {
    RWRecMutex::RLock lock(mutex_);
    limits = limits_;
  }
  bool flag = false;
  for (map<string, LimiterPtr>::iterator i = limits.begin(); i != limits.end(); ++i) {
    if (boost::regex_match(result.first, boost::regex(i->first))) {
      if (result.second > i->second->moremax() || result.second < i->second->moremin()) {
        alert(result.first, result.second, ALERTERALL);
      } else if (result.second > i->second->max() || result.second < i->second->min()) {
        alert(result.first, result.second, ALERTEREMAIL);
      }
      flag = true;
    }
  }
  if (!flag) {
    if (result.second > default_->moremax() || result.second < default_->moremin()) {
      alert(result.first, result.second, ALERTERALL);
    } else if (result.second > default_->max() || result.second < default_->min()) {
      alert(result.first, result.second, ALERTEREMAIL);
    }
  }
  MCE_DEBUG("Analyzer::analyse done");
}

void MashineLoadAnalyzer::alert(const string& key, int value, int type) {
  ostringstream oss;
  oss << key << ": Load is " << value << " percent";
  switch (type) {
    case ALERTEREMAIL:
      MonitorAlerterAdapter::instance().alert(key, oss.str(), type, ALERTEMAILINTERVAL);
      break;
    case ALERTERSMS:
    case ALERTERALL:
      MonitorAlerterAdapter::instance().alert(key, oss.str(), type, ALERTSMSINTERVAL);
      break;
    default:
      MCE_WARN("MashineLoadAnalyzer::alert unknown type: " << type);
  }
}

void MashineAvailableMemoryAnalyzer::alert(const string& key, int value, int type) {
  ostringstream oss;
  oss << key << ": Available Memory is " << value << "KB";
  switch (type) {
    case ALERTEREMAIL:
      MonitorAlerterAdapter::instance().alert(key, oss.str(), type, ALERTEMAILINTERVAL);
      break;
    case ALERTERSMS:
    case ALERTERALL:
      MonitorAlerterAdapter::instance().alert(key, oss.str(), type, ALERTSMSINTERVAL);
      break;
    default:
      MCE_WARN("MashineAvailableMemoryAnalyzer::alert unknown type: " << type);
  }
}

void MashineDiskAnalyzer::alert(const string& key, int value, int type) {
  ostringstream oss;
  oss << key << ": Used Disk is " << value << " percent";
  switch (type) {
    case ALERTEREMAIL:
      MonitorAlerterAdapter::instance().alert(key, oss.str(), type, ALERTEMAILINTERVAL);
      break;
    case ALERTERSMS:
    case ALERTERALL:
      MonitorAlerterAdapter::instance().alert(key, oss.str(), type, ALERTSMSINTERVAL);
      break;
    default:
      MCE_WARN("MashineDiskAnalyzer::alert unknown type: " << type);
  }
}

pair<string, int> MashineLoadAnalyzer::parse(const std::string& message) {
  map<string, string> params;
  vector<string> entries;
  boost::algorithm::split(entries, message, boost::algorithm::is_any_of(" "));
  for (vector<string>::iterator entry = entries.begin(); entry != entries.end(); ++entry) {
    vector<string> kv;
    boost::algorithm::split(kv, *entry, boost::algorithm::is_any_of("="));
    params[kv[0]]=kv[1];
  }
  int value = lexical_cast<int>(params["LoadAvg1"]) / lexical_cast<int>(params["CpuNumber"]);
  return pair<string, int>(params["Name"], value);
}

pair<string, int> MashineDiskAnalyzer::parse(const std::string& message) {
  map<string, string> params;
  vector<string> strings;
  boost::algorithm::split(strings, message, boost::algorithm::is_any_of(" \t"));
  vector<string> entries;
  for (vector<string>::iterator entry = strings.begin(); entry != strings.end(); ++entry) {
    if (entry->size() != 0) {
      entries.push_back(*entry);
    }
  }
  vector<string> names;
  boost::algorithm::split(names, entries[0], boost::algorithm::is_any_of("="));
  string v = entries.at(5).substr(0, entries.at(5).size() - 1);
  int value = lexical_cast<int>(v);
  return pair<string, int>(names[1], value);
}

pair<string, int> MashineAvailableMemoryAnalyzer::parse(const std::string& message) {
  map<string, string> params;
  vector<string> entries;
  boost::algorithm::split(entries, message, boost::algorithm::is_any_of(" \t"));
  for (vector<string>::iterator entry = entries.begin(); entry != entries.end(); ++entry) {
    vector<string> kv;
    boost::algorithm::split(kv, *entry, boost::algorithm::is_any_of("="));
    params[kv[0]]=kv[1];
  }
  int value = lexical_cast<int>(params["MemFree"]) + lexical_cast<int>(params["Buffers"]) + lexical_cast<int>(params["Cached"]) - (lexical_cast<int>(params["SwapTotal"]) - lexical_cast<int>(params["SwapFree"]));
  return pair<string, int>(params["Name"], value);
}

