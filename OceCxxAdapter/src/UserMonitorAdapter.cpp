#include "UserMonitorAdapter.h"
using namespace xce::usermonitor::adapter;
using namespace com::xiaonei::safecenter::usermonitor;
using namespace std;

//---------------------------------------------------------------------------

void UserMonitorAdapter::setUserActivity(int id, string& ip, string& url){
	getMonitorPrxOneway(id)->setUserActivity(id,ip,url);
}

MonitorPrx UserMonitorAdapter::getMonitorPrxOneway(int id){
	return locate<MonitorPrx>(name(),endpoints(), DATAGRAM, 300);
}
