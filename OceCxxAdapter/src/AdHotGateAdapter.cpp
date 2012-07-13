#include "AdHotGateAdapter.h"


using namespace xce::ad;
using namespace MyUtil;


AdHotGatePrx AdHotGateAdapter::getManagerOneway(int id){
	return locate<AdHotGatePrx> (_managers_oneway, "M", id, ONE_WAY);
}

