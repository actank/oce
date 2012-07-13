#include "Singleton.h"
#include "TaskManager.h"

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
  bool LoadConfig();
private:
  string DB_;

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
