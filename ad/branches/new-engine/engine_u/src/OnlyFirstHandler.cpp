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
 
void FirstFlag::Clear() {
  MCE_DEBUG("FirstFlag::Clear()");
  IceUtil::Mutex::Lock lock(mutex_);
  uid_.clear();
}
 
bool OnlyFirstHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  int uid = para.userProfile_.id();
  bool first = FirstFlag::instance().First(uid);
  if(first) {
    AdGroupPtr group = para.adzone_->only_first();
    if(group && group->bid_unit()->Match(para.userProfile_)) {
      SelectedAdGroupPtr sp = new SelectedAdGroup(group);
      sp->pos_ = 1;
      para.zone_selected_groups_.insert(sp);
      para.total_selected_groups_.insert(sp);
    }
    return true;
  } else {
    return true;
  }
}

}
}
