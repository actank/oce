#ifndef _AD_GATE_CONFIG_H_
#define _AD_GATE_CONFIG_H_
#include <netdb.h>
#include <arpa/inet.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "IceLogger.h"

namespace xce {
namespace ad {

class GateConfig : public MyUtil::Singleton<GateConfig> {
public:
  GateConfig();
  string DB() {
    return DB_;
  }
  void set_DB(string DB) {
    DB_ = DB;
  }
  string server_name() {
    return server_name_;
  }
  void set_server_name(string server_name) {
    ostringstream oss;
    oss << server_name << "@" << GetIp();
    server_name_ = oss.str();
  }
  int ping_seq_size() {
    return ping_seq_size_;
  }
  string engine_host() {
    return engine_host_;
  }
  int engine_port() {
    return engine_port_;
  }
  bool env_test() {
    return env_test_;
  }
  bool LoadConfig();
private:
  string GetIp();
  string DB_;
  string server_name_;
  int ping_seq_size_;
  string engine_host_;
  int engine_port_;
  bool env_test_;

  class GateConfigLoadTimer : public MyUtil::Timer{
  public:
    GateConfigLoadTimer(int time = 0):Timer(time){}
    virtual void handle() {
      MyUtil::TaskManager::instance().schedule(new GateConfigLoadTimer(60*1000));//1分钟
      GateConfig::instance().LoadConfig();
    }
  };
};

}
}
#endif //_AD_GATE_CONFIG_H_
