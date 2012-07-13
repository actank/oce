#include "AdUnionGateAdapter.h"

using namespace MyUtil;

namespace xce {
namespace ad {

AdUnionGatePrx AdUnionGateAdapter::getManager(int id){
	return locate<AdUnionGatePrx> (_managers, "M", id, TWO_WAY);
}

AdUnionGatePrx AdUnionGateAdapter::getManagerOneway(int id) {
	return locate<AdUnionGatePrx> (_managersOneway, "M", id, ONE_WAY);
}

}
}


