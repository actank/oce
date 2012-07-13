#include "AdGateAdapter.h"


using namespace MyUtil;

namespace xce {
namespace ad {

AdGatePrx AdGateAdapter::getManager(int id){
	return locate<AdGatePrx> (_managers, "M", id, TWO_WAY);
}

AdGatePrx AdGateAdapter::getManagerOneway(int id) {
	return locate<AdGatePrx> (_managersOneway, "M", id, ONE_WAY);
}

}
}

