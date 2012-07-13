#ifndef __MONITOR_REALTIMEMONITOR_ICE__
#define __MONITOR_REALTIMEMONITOR_ICE__
  
module xce{
  module monitor {


    struct MonitoredException {
      string type; 
      string para;
      string serviceName;
      string fileName;
      string className;
      string method;
      int lineNo;
    };


    class RealTimeMonitor {
      bool RecordException(MonitoredException exceptionContent);
    };
  };
};
#endif
