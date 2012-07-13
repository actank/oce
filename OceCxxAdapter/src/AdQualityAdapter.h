#ifndef AD_QUALITY_ADAPTER_H_
#define AD_QUALITY_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdQuality.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdQualityAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdQualityAdapter> {
public:
  AdQualityAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  AdQualityMap GetQualitys(string user_key) {
    return getManager(0)->GetQualitys(user_key);
  }
  AdQualityMap GetQualitysByZone(Ice::Long zone_id) {
    return getManager(0)->GetQualitysByZone(zone_id);
  }

  AdQualityUMap GetQualityUs(string user_key) {
    return getManager(0)->GetQualityUs(user_key);
  }
  AdQualityUMap GetQualityUsByZone(Ice::Long zone_id) {
    return getManager(0)->GetQualityUsByZone(zone_id);
  }
  AdQualityUMap GetQualityUsByZone(Ice::Long zone_id, int uid) {
    int index = uid%10;
    if(index == 7 || index == 8 || index == 9) {
      return getManager(index)->GetQualityUsByZone(zone_id);
    } else if(index == 2 || index == 3) {
      return getManager(1)->GetQualityUsByZone(zone_id);
    } else {
      return getManager(0)->GetQualityUsByZone(zone_id);
    }
  }
private:
  AdQualityManagerPrx getManager(int id);
  AdQualityManagerPrx getManagerOneway(int id);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdQuality";
  }
  virtual size_t cluster() {
    return 10; 
  }

  vector<AdQualityManagerPrx> _managersOneway;
  vector<AdQualityManagerPrx> _managers;
};

}
}


#endif // AD_QUALITY_ADAPTER_H_
