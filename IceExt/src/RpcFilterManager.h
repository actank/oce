#ifndef RPCFILTERMANAGER_H
#define RPCFILTERMANAGER_H

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include "UtilCxx/src/hash_map_string.h"
#include <RpcFilter.h>
#include <TaskManager.h>
#include <XceStorm.h>

namespace MyUtil {

const int TASK_LEVEL_PASSPORT_CONTROL = -100;

class RpcFilterI : public RpcFilter, public IceUtil::Thread {
public:
  RpcFilterI(const std::string& type) : flag_(false), type_(type) {start(128*1024).detach();}
  virtual void reload(const Ice::Current & = Ice::Current());
  virtual bool isValid(const Ice::Current& = Ice::Current());
  bool filter(const std::string& ip, const std::string& type);
  void run();
  string type() const {return type_;}
private:
  bool isAdmin(const std::string& user, const std::string& pwd, const std::string& ip) const;
  bool flag_;
  std::string type_;
  __gnu_cxx::hash_map<std::string, vector<string> > ip_field_list_;
  IceUtil::RWRecMutex mutex_;
};

class PassportControlTypeHandler : public com::xiaonei::xce::ResultHandler {
public:
  PassportControlTypeHandler(bool& flag, const std::string& type) : flag_(flag), type_(type) {}
  bool handle(mysqlpp::Row& res) const;
private:
  bool& flag_;
  std::string type_;
};

class PassportControlHandler : public com::xiaonei::xce::ResultHandler {
public:
  PassportControlHandler(vector<string>& ips, vector<string>& fields) : ips_(ips), fields_(fields) {}
  bool handle(mysqlpp::Row& res) const;
private:
  std::vector<std::string>& ips_;
  std::vector<std::string>& fields_;
};

typedef IceUtil::Handle<RpcFilterI> RpcFilterIPtr;

class RpcFilterObserver : public xce::storm::Observer {
public:
  RpcFilterObserver(const RpcFilterIPtr& filter) : filter_(filter) {}
  virtual bool update(const MyUtil::Str2StrMap& context, const Ice::Current& e);
private:
  RpcFilterIPtr filter_;
};

class RpcFilterReloadTask : public MyUtil::Task {
public:
  RpcFilterReloadTask(const RpcFilterIPtr& filter) : MyUtil::Task(TASK_LEVEL_PASSPORT_CONTROL), filter_(filter) {}
  virtual void handle();
private:
  RpcFilterIPtr filter_;
};
class RpcFilterManager : public MyUtil::Singleton<RpcFilterManager> {
public:
  void initialize(const std::string& type);
  bool filter(const Ice::Current& current, const std::string& type);
private:
  RpcFilterIPtr filter_;
};

}

#endif
