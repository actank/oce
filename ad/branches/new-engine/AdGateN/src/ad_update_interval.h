#ifndef _AD_UPDATE_INTERVAL_H_
#define _AD_UPDATE_INTERVAL_H_

#include <map>
#include <string>
#include <IceUtil/RWRecMutex.h>

#include "Singleton.h"
#include "TaskManager.h"

namespace xce {
namespace ad {

using namespace std;
using namespace MyUtil;

class AdUpdateIntervalCache : public MyUtil::Singleton<AdUpdateIntervalCache> {
public:
  void Reload();
  pair<int, int> GetUpdateInterval(const string & url);
private:
  mutable IceUtil::RWRecMutex mutex_;
  map<string, pair<int, int> > url_update_interval_;
};

class AdUpdateIntervalReloadTask : public Task {
public:
  virtual void handle();
};

}
}

#endif // _AD_UPDATE_INTERVAL_H_

