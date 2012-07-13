#ifndef _ONLY_FIRST_HANDLER_H_
#define _ONLY_FIRST_HANDLER_H_

#include "MatchHandlers.h"

namespace xce {
namespace ad {
//const int DURATION = 24*60*60*1000;
const int DURATION = 5*60*1000;
class FirstFlag : public MyUtil::Singleton<FirstFlag> {
public:
  FirstFlag() {
    int delay = DURATION - time(NULL) % DURATION;
    MCE_DEBUG("[FirstFlag] delay = " << delay);
    TaskManager::instance().schedule(new ClearTimer(delay)); 
  }
  bool First(int uid);
  void Clear();

private:
  IceUtil::Mutex mutex_;
  set<int> uid_;

  class ClearTimer : public MyUtil::Timer {
    public:
      ClearTimer(int time = DURATION) : Timer(time) { }
      virtual void handle() {
        FirstFlag::instance().Clear();
        TaskManager::instance().schedule(new ClearTimer()); 
      }
  };
};

class OnlyFirstHandler : public AdMatchHandler {
public:
  virtual bool handle(MatchHandlerParamter& para, AdResultMap& result);
};

}
}

#endif // _ONLY_FIRST_HANDLER_H_
