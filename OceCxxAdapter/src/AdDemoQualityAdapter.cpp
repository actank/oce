#include "AdDemoQuality.h"
#include "AdDemoQualityAdapter.h"

namespace xce{
namespace ad{

int AdDemoQualityAdapter::getsindex(int sindex) {
  if(sindex <= 16) {
    return 0;
  } else if(sindex >= 17 && sindex <= 33){
    return 1;
  } else if(sindex >= 34 && sindex <= 49) {
    return 2;
  } /*else if(sindex >=26 && sindex <=35) {
    return 3;
  } else if(sindex >=36 && sindex <=45) {
    return 4;
  } */else {
    return 0;
  }
}

}
}
