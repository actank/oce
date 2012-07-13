#include "APNSNotifyAdapter.h"

namespace xce {
namespace notify {
namespace apns {
APNSManagerPrx APNSNotifyAdapter::getManagerOneway() {
	return locate<APNSManagerPrx> (_managersOneway, "M", 0, ONE_WAY);
}

void APNSNotifyAdapter::addUser(Ice::Int userId, const string& token) {
	getManagerOneway()->addUser(userId, token);
}
void APNSNotifyAdapter::addIphoneNotifyBySeq(const MyUtil::IntSeq& userIds,
		const string& msg) {
	getManagerOneway()->addIphoneNotifyBySeq(userIds, msg);
}
}
;
}
;
}
;
