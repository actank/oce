#ifndef __EDM_ADD_FRIEND_EDM_SENDER_TASK_H__
#define __EDM_ADD_FRIEND_EDM_SENDER_TASK_H__
#include <string>
#include <map>
#include <IceUtil/RWRecMutex.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "MiscEdmSenderCache.h"
namespace xce {
namespace edm {
using namespace  MyUtil;

class AddFriendEdmSenderTask : public Task {
  public :
    AddFriendEdmSenderTask() : Task(TASK_LEVEL_PRELOAD),user_id_(-1) {}
    AddFriendEdmSenderTask(const int& id) : Task(TASK_LEVEL_PRELOAD),user_id_(id) {}
    virtual void handle();
    void Send();
  private :
    int user_id_;
};
}
}
#endif
