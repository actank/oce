#ifndef __USER_ONLINE_ADAPTER_H__
#define __USER_ONLINE_ADAPTER_H__

#include "UserOnline.h"
#include "Channel.h"
#include "Singleton.h"
#include "DataWrapperI.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::online;

const int OT_IM=1;
const int OT_WEB=2;

class UserOnlineAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::OceChannel, UserOnlineAdapter> {
public:

	void imOnline(int userId);
	void webOnline(int userId);

	void imOffline(int userId);
	void webOffline(int userId);

	void imOfflineBatch(IntSeq userIds);
	void webOfflineBatch(IntSeq userIds);

	int size();
private:
	virtual string name() {
		return "UserOnline";
	}
	virtual string endpoints() {
		return "@UserOnline";
	}
	virtual size_t cluster() {
		return 0;
	}
	virtual void initialize();

	OnlineManagerPrx _manager;
	OnlineManagerPrx _managerOneway;
};

}
;
}
;
}
;
}
;

#endif

