#include "OnlyFirstHandler.h"

namespace xce {
namespace ad {

bool FirstFlag::First(int uid) {
  IceUtil::Mutex::Lock lock(mutex_);
  set<int>::iterator it = uid_.find(uid);
  if(it != uid_.end()) {
    return false;
  } else {
    uid_.insert(uid);
    return true;
  }
}
 
bool FirstFlag::Clear(const ::Ice::Current&) {
  MCE_INFO("FirstFlag::Clear()");
  IceUtil::Mutex::Lock lock(mutex_);
  uid_.clear();
  return true;
}
}
}
