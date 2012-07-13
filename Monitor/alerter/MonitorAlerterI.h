#ifndef MONITORALERTERI_H
#define MONITORALERTERI_H

#include <Monitor.h>
#include <Singleton.h>
#include <set>
#include <IceUtil/IceUtil.h>

namespace monitor {

const std::string CONFIG = "../etc/alerter.config";
const int UPGRADE_INTERVAL = 3600;
const unsigned int UPGRADE_COUNT = 10;
const int ALERT_INTERVAL = 900;

class User : public Ice::Object {
public:
  User(const std::string& name, const std::string& phone, const std::string& email, const std::string& leader, const string& jira) :
    name_(name), phone_(phone), email_(email), leader_(leader), jira_(jira) {}
  virtual ~User() {}
  std::string name() const {return name_;}
  std::string phone() const {return phone_;}
  std::string email() const {return email_;}
  std::string leader() const {return leader_;}
  std::string jira() const {return jira_;}
  void sendSMS(const std::string& message);
  void sendEmail(const std::string& subject, const std::string& content);
  string str() const;

private:
  std::string name_;
  std::string phone_;
  std::string email_;
  std::string leader_;
  std::string jira_;

  deque<int>  times_;
  IceUtil::Monitor<IceUtil::Mutex> mutex_;
};

typedef IceUtil::Handle<User> UserPtr;

class Filter : public Ice::Object {
public:
  virtual ~Filter() {}
  bool check(const std::string& subject, int type, int interval);
private:
  std::map<std::string, std::map<int, int> > table_;
  IceUtil::RWRecMutex mutex_;
};

typedef IceUtil::Handle<Filter> FilterPtr;

class UserCompare {
public:
  bool operator()(const UserPtr &a, const UserPtr &b) const {
    return a->name() < b->name();
  }
};

class MonitorAlerterI : public MonitorAlerter, public MyUtil::Singleton<MonitorAlerterI> {
public:
  MonitorAlerterI() : filter_(new Filter) {}
  virtual ~MonitorAlerterI() {}
  virtual void reload(const Ice::Current& = Ice::Current());
  virtual void alert(const std::string& subject, const std::string& message, int type, int interval, const Ice::Current& = Ice::Current());
  virtual void alertmerely(const int type, 
                                 const string &phone, 
                                 const string &email, const string &subject, const string &message, 
                                 const Ice::Current& = Ice::Current());
private:
  void jira(const std::string& assignee, const std::vector<std::string>& names, const std::string& summary);

  std::map<std::string, std::string> jiras_;
  std::map<std::string, UserPtr> users_;
  std::map<std::string, std::set<UserPtr, UserCompare> > pattern2users_;
  std::set<UserPtr, UserCompare> defaultusers_;
  FilterPtr filter_;
  IceUtil::RWRecMutex mutex_;
};

}

#endif
