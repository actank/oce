#ifndef __COUNTCACHE_ADAPTER_H__
#define __COUNTCACHE_ADAPTER_H__

#include "CountCache.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::count;

const int CLUSTER = 10;

class CountCacheAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::OceChannel, CountCacheAdapter> {
public:
	CountCacheAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void incImLeavewordCount(int id, int count=1);
	void decImLeavewordCount(int id, int count=1);
	void incGuestRequestCount(int id, int count=1);
	void decGuestRequestCount(int id, int count=1);
	int getGuestRequestCount(int id);
	CounterPtr getAll(int id);

private:
	virtual string name() {
		return "CountCacheN";
	}
	virtual string endpoints() {
		return "@CountCacheN";
	}
	virtual size_t cluster() {
		return 10;
	}
	CountManagerPrx getCountManager(int id);
	CountManagerPrx getCountManagerOneway(int id);

	vector<CountManagerPrx> _managers;
	vector<CountManagerPrx> _managersOneway;
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

