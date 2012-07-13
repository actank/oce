#ifndef __EDM_FREQUENCY_VALIDATE_H__
#define __EDM_FREQUENCY_VALIDATE_H__
#include "Singleton.h"
#include <ctime>
#include <map>
#include "TaskManager.h"
#include <IceUtil/RWRecMutex.h>
#include <list>
#include <Ice/Ice.h>
namespace xce {
namespace edm {
using namespace  MyUtil;
//define record struct
struct Entry {
  int send_flag_;
  time_t weekly_send_time_;
  time_t daily_send_time_;
  Entry() : send_flag_(0), weekly_send_time_(time(NULL)),daily_send_time_(time(NULL)) {}
};
//define cache
enum SendType { kWeekly = 1, kDaily};
class FrequencyCache : public MyUtil::Singleton<FrequencyCache> {
  public:
    FrequencyCache();
    bool LoadFromDb();
    bool Add(const Ice::Int& user_id, SendType send_type = kWeekly, time_t update_time = time(NULL));
    Ice::Int  CheckFrequencyLimit(const Ice::Int& user_id);
    bool weekly_load_sign() { return weekly_load_sign_;}
    bool daily_load_sign() { return daily_load_sign_;}
    bool load_sign() { return weekly_load_sign_|daily_load_sign_;}
    void EvictCache();
		void ClearCacheByUser(int userid);
    void FlushCache();
    int update_wday() { return update_wday_;}
    void set_update_wday(int wday) { update_wday_ = wday;}
  private:
    typedef std::map<Ice::Int, Entry> UEMap;
    bool _LoadWeeklyData();
    bool _LoadDailyData();
    bool _LoadWday();
    bool _UpdateWday(int wday);
    time_t _GetFlushTime();
  private:
    // this flag cadicates  cache's loading state. If in loading,value is true;else is false
    bool weekly_load_sign_;
    bool daily_load_sign_;
    // to keep data consistency,need mutex to sync
    IceUtil::RWRecMutex mutex_;
    // data storage
    UEMap user_cache_;
    int update_wday_;
  private:
    class CacheCleaner : public Timer {
      public:
        CacheCleaner(Ice::Int s = 60) : Timer(s*1000) {}
        virtual void handle();
    };
    class CacheFlusher : public Timer {
      public:
        CacheFlusher(Ice::Int s = 1); 
        virtual void handle();
      private:
        static time_t last_update_time_;
    };
     
};  

class FrequencyCacheLoadTask : public Task {
public:
  FrequencyCacheLoadTask():Task(TASK_LEVEL_PRELOAD) {}
  virtual void handle();
};


}
}
#endif
