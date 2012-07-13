#include "NotifyGateAdapter.h"


using namespace xce::notify;
using namespace MyUtil;


NotifyGatePrx NotifyGateAdapter::getManager(int id){
	return locate<NotifyGatePrx> (_managers, "M", id, TWO_WAY);
}

StrSeq NotifyGateAdapter::getNotifySeq2(int uid, int view, long lastNotifyId, int limit){
	return getManager(uid)->getNotifySeq2(uid, view, lastNotifyId, limit);
}
