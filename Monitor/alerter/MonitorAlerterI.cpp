#include "MonitorAlerterI.h"
#include <boost/algorithm/string/split.hpp>
#include <util/cpp/AlertClient.h>
#include <ServiceI.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <Ice/Ice.h>
#include <boost/regex.hpp>
#include <curl/curl.h>
#include <util/cpp/InvokeMonitor.h>
#include <Monitor/client/service/ServiceMonitor.h>

using Ice::Current;
using Ice::PropertiesPtr;
using Ice::PropertyDict;
using IceUtil::Mutex;
using IceUtil::RWRecMutex;
using MyUtil::AlertClient;
using monitor::UserPtr;
using monitor::User;
using monitor::Filter;
using monitor::MonitorAlerterI;
using xce::serverstate::ServerStateSubscriber;
using MyUtil::InvokeClient;
using MyUtil::ServiceMonitorManager;


void MonitorAlerterI::jira(const string& assignee, const vector<string>& names, const string& summary) {
  MCE_DEBUG("MonitorAlerterI::jira assignee: " << assignee << " summary: " << summary);
  if (names.size() <= 0) {
    return;
  }
  ostringstream nss;
  for (vector<string>::const_iterator i = names.begin(); i != names.end(); ++i) {
    if (i != names.begin()) {
      nss << ",";
    }
    nss << *i;
  }
  ostringstream post;
  post << "os_username=XCE_alarmRobot&os_password=alarmrobot&os_cookie=true&summary=" << summary << "&priority=3&customfield_10020=" << nss.str() << "&assignee=" << assignee << "&pid=10150&issuetype=3";
  ostringstream oss;
  oss << "wget -q --load-cookies ../etc/cookies.txt --post-data '" << post.str() << "' http://tracker.d.xiaonei.com/jira/secure/CreateIssueDetails.jspa";
  system(oss.str().c_str());
  MCE_WARN("MonitorAlerterI::jira: " << oss.str()); 
  ServiceMonitorManager::instance().start();
}

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&MonitorAlerterI::instance(), service.createIdentity("M", ""));
  ServerStateSubscriber::instance().initialize("ControllerMonitorAlerter", &MonitorAlerterI::instance(), 0, 5, new MyUtil::OceChannel, "5000", "5000", "1048576");
  MonitorAlerterI::instance().reload();
}

void MonitorAlerterI::reload(const Current& current) {

  InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
  map<string, set<UserPtr, UserCompare> > temppattern2users;
  map<string, UserPtr> tempusers;
  set<UserPtr, UserCompare> tempdefaultusers;
  PropertiesPtr properties = Ice::createProperties();
  properties->load(CONFIG);
  string defaultuser = properties->getPropertyWithDefault("Alerter.Default.User", "");
  vector<string> defaultusers;
  boost::algorithm::split(defaultusers, defaultuser, boost::algorithm::is_any_of(" "));
  set<string> defaults(defaultusers.begin(), defaultusers.end());
  PropertyDict users = properties->getPropertiesForPrefix("Alerter.User.Name");
  for(PropertyDict::iterator ui = users.begin(); ui != users.end(); ++ui) {
    PropertyDict fields = properties->getPropertiesForPrefix("Alerter.User."+ui->second);
    vector<string> patterns;
    boost::algorithm::split(patterns, fields["Alerter.User."+ui->second+".Patterns"], boost::algorithm::is_any_of(" "));
    UserPtr user = new User(ui->second, fields["Alerter.User."+ui->second+".Phone"], fields["Alerter.User."+ui->second+".Email"], fields["Alerter.User."+ui->second+".Leader"], fields["Alerter.User."+ui->second+".Jira"]);
    tempusers[ui->second] = user;
    for (vector<string>::iterator pattern = patterns.begin(); pattern != patterns.end(); ++pattern) {
      temppattern2users[*pattern].insert(user);
    }
  }
  for (map<string, UserPtr>::iterator ii = tempusers.begin(); ii != tempusers.end(); ++ii) {
    map<string, UserPtr>::iterator l = tempusers.find(ii->second->leader());
    if (l != tempusers.end()) {
      temppattern2users[ii->second->name()].insert(l->second);
    }
    if (defaults.count(ii->second->name()) > 0) {
      tempdefaultusers.insert(ii->second);
    }
  }
  map<string, string> jirass;
  PropertyDict jiras = properties->getPropertiesForPrefix("Alerter.Jira");
  for (PropertyDict::iterator ji = jiras.begin(); ji != jiras.end(); ++ji) {
    vector<string> jiraitems;
    boost::algorithm::split(jiraitems, ji->second, boost::algorithm::is_any_of(" "));
    if (jiraitems.size() == 2) {
      jirass[jiraitems.at(1)] = jiraitems.at(0);
    }
  }
  RWRecMutex::WLock lock(mutex_);
  users_ = tempusers;
  pattern2users_ = temppattern2users;
  defaultusers_ = tempdefaultusers;
  jiras_ = jirass;
}

void MonitorAlerterI::alert(const string& subject, const string& message, int type, int interval = ALERT_INTERVAL, const Current& current) {
  ostringstream oss;
  oss << "subject: " << subject << " message: " << message << " type: " << type << " interval: " << interval;
  InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  if (!filter_->check(subject, type, interval)) {
    return;
  } else {
    MCE_WARN("MonitorAlerterI::alert " << current.con->toString().substr(current.con->toString().find("remote",25)+17)
	 << " subject: " << subject << " message: " << message << " type: " << type << " interval: " << interval);
  }
  map<string, set<UserPtr, UserCompare> > pattern2users;
  set<UserPtr, UserCompare> defaultusers;
  map<string, string> jiras;
  {
    RWRecMutex::RLock lock(mutex_);
    pattern2users = pattern2users_;
    defaultusers = defaultusers_;
    jiras = jiras_;
  }
  map<string, UserPtr> users;
  for (map<string, set<UserPtr, UserCompare> >::iterator pattern = pattern2users.begin(); pattern != pattern2users.end(); ++pattern) {
    if (boost::regex_match(subject, boost::regex(pattern->first))) {
      for (set<UserPtr, UserCompare>::iterator user = pattern->second.begin(); user != pattern->second.end(); ++user) {
          users.insert(make_pair<string, UserPtr>((*user)->name(), (*user)));
      }
    }
  }
  if (users.size() == 0) {
    MCE_DEBUG("MonitorAlerterI::alert default: " << defaultusers.size());
    for (set<UserPtr, UserCompare>::iterator user = defaultusers.begin(); user != defaultusers.end(); ++user) {
      (*user)->sendEmail(subject, message);
    }
  } else {
    vector<string> names;
    for (map<string, UserPtr>::iterator i = users.begin(); i != users.end(); ++i) {
      switch (type) {
        case ALERTERSMS:
          i->second->sendSMS(message);
          if (!i->second->jira().empty()) {
            names.push_back(i->second->jira());
          }
          break;
        case ALERTEREMAIL:
          i->second->sendEmail(subject, message);
          break;
        case ALERTERALL:
          i->second->sendSMS(message);
          if (!i->second->jira().empty()) {
            names.push_back(i->second->jira());
          }
          i->second->sendEmail(subject, message);
          break;
        default:
          MCE_WARN("MonitorAlerterI::alert unknown type: " << type);
      }
    }
    bool flag = false;
    for (map<string, string>::iterator ji = jiras.begin(); ji != jiras.end(); ++ji) {
      if (flag) {
        break;
      }
      if (boost::regex_match(subject, boost::regex(ji->first))) {
        jira(users[ji->second]->jira(), names, message);
        flag = true;
      }
    }
    if (!flag) {
      MCE_DEBUG("MonitorAlerterI::alert default jira: " << defaultusers.size());
      for (set<UserPtr, UserCompare>::iterator user = defaultusers.begin(); user != defaultusers.end(); ++user) {
        switch (type) {
          case ALERTERSMS:
            (*user)->sendEmail(subject, "no jira");
            break;
          case ALERTEREMAIL:
            break;
          case ALERTERALL:
            (*user)->sendEmail(subject, "no jira");
            break;
          default:
            MCE_WARN("MonitorAlerterI::alert unknown type: " << type);
        }
      }
    }
  }
  MCE_DEBUG("MonitorAlerterI::alert done");
}

void MonitorAlerterI::alertmerely(const int type, 
                                 const string &phone, 
                                 const string &email, const string &subject, const string &message, 
                                 const Current& current) {

  MCE_WARN("MonitorAlerterI::alertmerely-->" << type << "," << phone << "," << email << "," << subject << "," << message);

  if (ALERTERALL == type || ALERTERSMS == type) {
    AlertClient::alertSMS(phone, message);
  } 

  if (ALERTERALL == type || ALERTEREMAIL == type) {
    AlertClient::alertEmail(email, subject, message);
  }

}



bool Filter::check(const string& subject,int type,  int interval = ALERT_INTERVAL) {
  int now = time(0);
  {
    RWRecMutex::RLock lock(mutex_);
    map<string, map<int, int> >::iterator i = table_.find(subject);
    if (i != table_.end()) {
      map<int, int>::iterator j = i->second.find(type);
      if (j != i->second.end() && now - j->second <= interval) {
        return false;
      }
    }
  }
  {
    RWRecMutex::WLock lock(mutex_);
    table_[subject][type] = now;
    return true;
  }
}

void User::sendSMS(const string& message) {
  AlertClient::alertSMS(phone_, message);
  int now = time(0);
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  times_.push_back(now);
  if (times_.size() > UPGRADE_COUNT) {
    if (times_.back() - times_.front() < UPGRADE_INTERVAL) {
      MonitorAlerterI::instance().alert(name_, name_+" too sms", ALERTERSMS);
    }
    times_.pop_front();
  }
}

void User::sendEmail(const string& subject, const string& content) {
  AlertClient::alertEmail(email_, subject, content);
}

string User::str() const {
  ostringstream oss;
  oss << "name: " << name_ << " phone: " << phone_ << " email: " << email_ << " leader: " << leader_;
  return oss.str();
}
