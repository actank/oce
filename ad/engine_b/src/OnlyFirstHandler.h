#ifndef _ONLY_FIRST_HANDLER_H_
#define _ONLY_FIRST_HANDLER_H_

#include <set>
#include <Ice/Ice.h>
//#include <IceUtil/IceUtil.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "IceLogger.h"
#include "AdEngine.h"

namespace xce {
namespace ad {

class FirstFlag : public AdFirstFlag, public MyUtil::Singleton<FirstFlag> {
public:
  FirstFlag() {
  }
  bool First(int uid);
  //void Clear();
  virtual bool Clear(const ::Ice::Current& = ::Ice::Current());

private:
  IceUtil::Mutex mutex_;
  std::set<int> uid_;
};

}
}

#endif // _ONLY_FIRST_HANDLER_H_
