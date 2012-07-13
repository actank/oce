#include "AdMemoryAdapter.h"

using namespace MyUtil;
using namespace xce::ad;
using namespace boost;

AdMemoryPrx AdMemoryAdapter::getManager(int id) {
  return locate<AdMemoryPrx> (_managers, "M", id, TWO_WAY);
}

AdMemoryPrx AdMemoryAdapter::getManagerOneway(int id) {
  return locate<AdMemoryPrx> (_managersOneway, "M", id, ONE_WAY);
}



