#ifndef AD_DEMOQUALITY_ADAPTER_H_
#define AD_DEMOQUALITY_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdDemoQuality.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdDemoQualityAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdDemoQualityAdapter> {
public:
  AdDemoQualityAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  AdQualityMap GetDemoPool(int sindex){
    return getManager(getsindex(sindex))->GetDemoPool();
  }
  
  AdQualityMap GetAppPool(int sindex){
    return getManager(getsindex(sindex))->GetAppPool();
  }

  FreshAdQualityMap GetFreshAdMap(int sindex){
    return getManager(getsindex(sindex))->GetFreshAdMap();
  }
  FreshAdQualityMap GetDisplayLessAds(int sindex){
    return getManager(getsindex(sindex))->GetDisplayLessAds();
  }
  AdPosMap GetAverageEcpm(int sindex){
    return getManager(getsindex(sindex))->GetAverageEcpm();
  }
  AdSortPosMap GetAppSortPos(int sindex) {
    return getManager(getsindex(sindex))->GetAppSortPos();
  }

  //
  AdQualityScoreMap GetAdsQuality(int sindex) {
    return getManager(getsindex(sindex))->GetAdsQuality();
  }
  //

  CpmAdgroupMap GetCpmAdGroupMap(int sindex) {
    return getManager(getsindex(sindex))->GetCpmAdGroupMap();
  }

  LimitedAdGroupMap GetLimitedAdGroupMap(int sindex) {
    return getManager(getsindex(sindex))->GetLimitedAdGroupMap();
  }
private:
  AdDemoQualityManagerPrx getManager(int id){
      return locate<AdDemoQualityManagerPrx> (_managers, "M", id, TWO_WAY);
  }
  AdDemoQualityManagerPrx getManagerOneway(int id){
      return locate<AdDemoQualityManagerPrx> (_managersOneway, "M", id, ONE_WAY);
  }

  /*int getsindex(int sindex) {
    if(sindex == 18) {
      return 1;
    }else if(sindex == 19) {
      return 2;
    }else if(sindex == 20) {
      return 3;
    }else if(sindex == 21) {
      return 4;
    }else if (sindex == 5 || sindex == 6 || sindex == 7 || sindex == 8 || sindex == 9 || sindex == 10 || sindex == 11 || sindex == 17 ){
      return 5;
    }
    else{
      return 0;
    }
  }*/
  
  //move to cpp
  int getsindex(int sindex);
/*  int getsindex(int sindex) {
    if(sindex <= 17) {
      return 0;
    } else if( sindex >= 18 && sindex <= 21){
      return 1;
    } else if(sindex >=22 && sindex <=25) {
      return 2;
    } else if(sindex >=26 && sindex <=35) {
      return 3;
    } else if(sindex >=36 && sindex <=45) {
      return 4;
    } else {
      return 0;
    }
  }*/
  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@AdDemoLoaderCache";
  }
  virtual size_t cluster() {
    return 10; 
  }

  vector<AdDemoQualityManagerPrx> _managersOneway;
  vector<AdDemoQualityManagerPrx> _managers;
};

}
}


#endif // AD_DEMOQUALITY_ADAPTER_H_
