#ifndef NIELSONi_FILTER_H_
#define NIELSON_FILTER_H_
#include <IceUtil/Mutex.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "FeedMemcProxy/client/user_profile_client.h"
#include "AdStruct.h"

namespace xce {
namespace ad {

class NielsonFilter : public MyUtil::Singleton<NielsonFilter> {
public:
  NielsonFilter() {
    Init();
    TaskManager::instance().schedule(new ReloadTimer());
  }
  bool Filter(const UserProfile& userProfile, set<AdGroupPtr>& groups);
  bool Filter(int uid, Ice::Long group_id, int& ad_type);

private:
  bool Init();
  map<int, pair<int, bool> > permissions_;
  set<int> uid_tails_;
  set<Ice::Long> groups_;
  IceUtil::RWRecMutex mutex_;

  class ReloadTimer : public MyUtil::Timer {
  public:
    ReloadTimer() : Timer(1 * 60 * 1000) {}
    virtual void handle();
  };
};

}
}

#endif /* NIELSON_FILTER_H_ */
