#ifndef __EDM_MAY_KNOW_EDM_SENDER_TASK_H__
#define __EDM_MAY_KNOW_EDM_SENDER_TASK_H__ 
#include <string>
#include <map>
#include <IceUtil/RWRecMutex.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "MiscEdmSenderCache.h"
namespace xce {
namespace edm {
using namespace  MyUtil;

class MayKnowEdmSenderTask : public Task {
  public :
    MayKnowEdmSenderTask() : Task(TASK_LEVEL_PRELOAD), user_id_(-1) {}
    MayKnowEdmSenderTask(const int& id) : Task(TASK_LEVEL_PRELOAD),user_id_(id) {}
    virtual void handle();
    void Send();
  private :
    int user_id_;
};



}
}
#endif
