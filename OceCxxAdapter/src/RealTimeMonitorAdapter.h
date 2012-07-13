/*
 * RealTimeMonitorI.h
 *
 *  Created on: 2010-4-12
 *      Author: ld
 */

#ifndef __MONITOR_REALTIMEMONITORADAPTER_H__
#define __MONITOR_REALTIMEMONITORADAPTER_H__



#include "AdapterI.h"
#include "RealTimeMonitor.h"

namespace xce {
namespace monitor {
using namespace MyUtil;
class RealTimeMonitorAdapter: public AdapterI, public AdapterISingleton<
    MyUtil::NotifyChannel, RealTimeMonitorAdapter> {
public:
  
  bool RecordException(const MonitoredException& exception_content,
      const Ice::Current* context);

  int getCluster() {
    return _cluster;
  }
protected:
  virtual string name() {
    return "RealTimeMonitor";
  }
  virtual string endpoints() {
    return "@RealTimeMonitor";
  }
  virtual size_t cluster() {
    return 0;
  }
  RealTimeMonitorPrx getManagerOneway();
};

}
}

#endif

