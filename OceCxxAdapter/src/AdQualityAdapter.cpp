#include "AdQualityAdapter.h"

using namespace MyUtil;
using namespace xce::ad;
using namespace boost;

AdQualityManagerPrx AdQualityAdapter::getManager(int id) {
  return locate<AdQualityManagerPrx> (_managers, "M", id, TWO_WAY);
}

AdQualityManagerPrx AdQualityAdapter::getManagerOneway(int id) {
  return locate<AdQualityManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}



