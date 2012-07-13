#ifndef LOGMONITOR_H
#define LOGMONITOR_H

#include <Singleton.h>

namespace MyUtil {

class LogMonitor : public MyUtil::Singleton<LogMonitor> {
public:
  LogMonitor() {
    memset(count, 0, 6 * sizeof(int));
  }
  
  void inc(int level) {
    count[level]++;
  }

  void fetch(int* s) {
    memcpy(s, count, 6 * sizeof(int));
    memset(count, 0, 6 * sizeof(int));
  }
  
private:
  int count[6];
};

}

#endif
