#ifndef FEED_GROUP_COMMON_H_
#define FEED_GROUP_COMMON_H_

#include <string>
#include <cmath>
#include "TaskManager.h"
#include "IceLogger.h"
#include "Singleton.h"
namespace xce {
namespace feed {

extern const char * FeedDBInstance;
extern const int kUserClusterSize;
extern const int kDBClusterSize;

std::string GetFeedTableName(int id);

class TimeStat {
  timeval _begin, _end;
  bool _log;
public:
  TimeStat(bool log = false) {
    _log = log;
    reset();
  }
  ~TimeStat() {
//    if (_log) {
//      MCE_INFO("######### " << getTime());
//    }
  }
  void reset() {
    gettimeofday(&_begin, NULL);
  }
  float getTime() {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec) + _end.tv_usec
        - _begin.tv_usec;
    timeuse /= 1000;
    return timeuse;
  }

};


}
}

#endif // FEED_GROUP_COMMON_H_

