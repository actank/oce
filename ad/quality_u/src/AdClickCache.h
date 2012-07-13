#ifndef AD_CLICK_CACHE_H_
#define AD_CLICK_CACHE_H_

#include "AdQualityManagerI.h"
#include <string>
#include <map>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "Singleton.h"

namespace xce {
namespace ad {

class AdClickStat : public IceUtil::Shared {
public:
  AdClickStat(Ice::Long gid) : gid_(gid), gender_factor_(-1.0) {}
  double GenderFactor();
  map<int, double> AgeFactors();

  void AddUserClick(int uid, int count) {
    map<int, int>::iterator it = user_clicks_.find(uid);
    if (it == user_clicks_.end()) {
      user_clicks_[uid] = count;
    } else {
      it->second += count;
    }
  }
private:
  Ice::Long gid_;
  map<int, int> user_clicks_;

  double gender_factor_;
  map<int, double> age_factors_;
};

typedef IceUtil::Handle<AdClickStat> AdClickStatPtr;
typedef map<Ice::Long, AdClickStatPtr> AdClickStatMap;

class AdClickCache : public MyUtil::Singleton<AdClickCache>, public MyUtil::Timer {
public:
  AdClickCache() : Timer(30 * 60 * 1000), loading_(false) {} 
  bool Reload();
  double GenderFactor(Ice::Long gid);
  map<int, double> AgeFactors(Ice::Long gid);
  virtual void handle();
private:
  bool loading_;
  AdClickStatMap ad_user_clicks_;
  IceUtil::Mutex mutex_;
};

#endif // AD_CLICK_CACHE_H_

}
}
