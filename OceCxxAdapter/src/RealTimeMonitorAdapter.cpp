#include "RealTimeMonitorAdapter.h"

namespace xce {
namespace monitor {

RealTimeMonitorPrx RealTimeMonitorAdapter::getManagerOneway() {
  return RealTimeMonitorPrx::uncheckedCast(locate<RealTimeMonitorPrx> ("M", ONE_WAY));
}

bool RealTimeMonitor::RecordException(const MonitoredException& exception_content,
     const Ice::Current*  context) {
  return getManagerOneway()->RecordException(exception_content);
}



}
}
