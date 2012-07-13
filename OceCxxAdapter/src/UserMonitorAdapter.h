#ifndef __USER_MONITOR_ADAPTER_H__
#define __USER_MONITOR_ADAPTER_H__

#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"
#include <UserMonitor.h>

namespace xce {
namespace usermonitor {
namespace adapter {

using namespace MyUtil;
using namespace com::xiaonei::safecenter::usermonitor;

class UserMonitorAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::UserMonitorChannel, UserMonitorAdapter>,
	virtual public IceUtil::Monitor<IceUtil::Mutex>  {
public:
	UserMonitorAdapter() {
//		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void setUserActivity(int id, std::string& ip, std::string& url);
private:
	virtual string name() {
		return "DataMonitor";
	}
	virtual string endpoints() {
		return "@Aegis_DataMonitor0";
	}
	virtual size_t cluster() {
		return 0;
	}
	MonitorPrx getMonitorPrxOneway(int id);
	vector<MonitorPrx > _managersOneway;

};

};
};
};

#endif

