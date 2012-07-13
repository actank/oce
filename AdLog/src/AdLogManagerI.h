#ifndef _ADLOGMANAGER_H_
#define _ADLOGMANAGER_H_

#include "AdLog.h"
#include <iostream>
#include <string>
#include <queue>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include <Ice/Ice.h>
#include "TaskManager.h"
#include <QueryRunner.h>
#include "ServiceI.h"
#include "Singleton.h"
#include "Date.h"

namespace xce{
namespace ad{
using namespace std;
using namespace com::xiaonei::xce;
using namespace MyUtil;

class AdLogManagerI: virtual public AdLogManager,virtual public MyUtil::Singleton<AdLogManagerI> {
public:
  virtual void Pv(const string& log, const Ice::Current& = Ice::Current());
  virtual void PvBySeq(const MyUtil::StrSeq& logs, const Ice::Current& = Ice::Current());
  virtual void ClickForCharge(const string& log, const Ice::Current& = Ice::Current());
  virtual void ClickForBp(const string& log, const Ice::Current& = Ice::Current());
  virtual Ice::Int UpdateOnceCount(const Ice::Int count, const Ice::Current& = Ice::Current());
  
  /*
   *should use functions below unless you know what does them mean
   *
   */
  virtual void PvBatch(const ::MyUtil::StrSeq& logs, const Ice::Current& current);
  virtual void PvForCharge(const ::MyUtil::StrSeq& logs, const Ice::Current& = Ice::Current());
  virtual void EngineLog(const string& log, const Ice::Current& = Ice::Current());
  virtual void EngineLogBatch(const ::MyUtil::StrSeq& logs, const Ice::Current& current);
};

class PvLogHandler : public MyUtil::Singleton<PvLogHandler> {
public:
  PvLogHandler() :
    once_count_(10) {
    TaskManager::instance().execute(new PvLogHandlerTask());
  }
  void PushBack(string log) {
    IceUtil::Mutex::Lock lock(mutex_);
    queue_.push(log);
  }
  void PushBack(const MyUtil::StrSeq& logs);
  int once_count() const {
    return once_count_;
  }
  void set_once_count(int once_count) {
    this->once_count_ = once_count;
  }
private:
  bool Execute();

  queue<string> queue_;
  int once_count_;
  IceUtil::Mutex mutex_;

  class PvLogHandlerTask : virtual public MyUtil::Task {
  public:
    PvLogHandlerTask() {
    }
    virtual void handle() {
      while (true) {
        try {
          if (!PvLogHandler::instance().Execute()) {
            usleep(SLEEP_DURATION);
          }
        } catch (Ice::Exception& e) {
          MCE_WARN("PvLogHandlerTask --> handle Execute err:" << e << ", line:"
              << __LINE__);
        } catch (...) {
          MCE_WARN("PvLogHandlerTask --> handle Execute , line:" << __LINE__);
        }
      }
    }
  private:
    static const int SLEEP_DURATION = 200 * 1000; // 停顿时间
  };
};

}
}
#endif
