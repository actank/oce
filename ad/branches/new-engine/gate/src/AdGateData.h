#ifndef ADGATEDATA_H_
#define ADGATEDATA_H_

#include "Singleton.h"
#include "TaskManager.h"

namespace xce {
namespace ad {

class RefreshTime : public MyUtil::Singleton<RefreshTime> {
public:
  RefreshTime() {};
  void Load(string test = "60000*29");
  int next_load_time(Ice::Long zone_id, int refresh_index);

private:
  map<Ice::Long, vector<int> > times_; 
  IceUtil::RWRecMutex mutex_;
};
 
class RefreshTimeTimer : public MyUtil::Timer {
public:
  RefreshTimeTimer(int time = 0):Timer(time) {}
  virtual void handle() {
    //RefreshTime::instance().Load();
    MyUtil::TaskManager::instance().schedule(new RefreshTimeTimer(60000));
  };
};

}
}

#endif
