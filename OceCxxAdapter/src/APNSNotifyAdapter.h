#ifndef __APNSADAPTER_H__
#define __APNSADAPTER_H__

#include "AdapterI.h"
#include "APNSNotify.h"

namespace xce {
namespace notify {
namespace apns {
using namespace MyUtil;
class APNSNotifyAdapter: public AdapterI, public AdapterISingleton<
		MyUtil::NotifyChannel, APNSNotifyAdapter> {
public:
	APNSNotifyAdapter(){
        	_managersOneway.resize(cluster() > 0 ? cluster() : 1);

	}
	void addUser(Ice::Int userId, const string& token);
	void addIphoneNotifyBySeq(const MyUtil::IntSeq& userIds, const string& msg);
	int getCluster() {
		return _cluster;
	}
protected:
	virtual string name() {
		return "APNSNotify";
	}
	virtual string endpoints() {
		return "@APNSNotify";
	}
	virtual size_t cluster() {
		return 1;
	}
	vector<APNSManagerPrx> _managersOneway;
	APNSManagerPrx getManagerOneway();
};
}
;
}
}
;

#endif
