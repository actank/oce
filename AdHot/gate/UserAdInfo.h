#ifndef _XCE_AD_USERADINFO_H_
#define _XCE_AD_USERADINFO_H_

#include <time.h>
#include <map>
#include <ctemplate/template.h>

#include "TaskManager.h"
#include "Singleton.h"
#include "UserCacheAdapter.h"

using namespace std;
using namespace xce::usercache;

namespace xce {
namespace ad {

struct PromotionItem {
  Ice::Int id_;
  bool is_read_;
  int level_;
  time_t recv_time_;

  map<Ice::Int, UserCachePtr> buddies_;
  // vector<BuddyProfile> buddies_;

  bool operator < (const PromotionItem & item) const {
    if (level_ < item.level_) {
      return true;
    }
    if (recv_time_ < item.recv_time_) {
      return true;
    }
    return id_ < item.id_;
  }
  bool FillItemDict(ctemplate::TemplateDictionary * dict) const;
};

// 用户的所有广告信息
class UserAdInfo : public IceUtil::Shared {
public:
  UserAdInfo(Ice::Int uid) : uid_(uid), pv_time_(time(NULL)) {}
  bool Load();
  void FillDict(ctemplate::TemplateDictionary * dict) const;

  Ice::Int uid() const {
    return uid_;
  }

  void set_pv_time(time_t t) {
    pv_time_ = t;
  }
  time_t pv_time() {
    return pv_time_;
  }

  const PromotionItem * GetPromotionItem(int ad_id) {
    for(set<PromotionItem>::const_iterator it = promotions_.begin(); 
        it != promotions_.end(); ++it) {
      if (it->id_ == ad_id) {
        return &(*it);
      }
    }
    return NULL;
  }
private:
  Ice::Int uid_;
  time_t pv_time_;
  set<PromotionItem> promotions_;
};
typedef IceUtil::Handle<UserAdInfo> UserAdInfoPtr;

class AdInfoCache : public MyUtil::Singleton<AdInfoCache>  {
public:
  UserAdInfoPtr GetUserAdInfo(Ice::Int uid);
  void ClearUserAdInfo(Ice::Int uid);
  void ClearOutdated();
private:
  IceUtil::RWRecMutex rw_mutex_;
  map<Ice::Int, UserAdInfoPtr> uid_ad_map_;
};

class ClearCacheTimer: public MyUtil::Timer {
public:
  ClearCacheTimer();
  void handle();
};

}
}

#endif // _XCE_AD_USERADINFO_H_
