#ifndef __EDM_FRIEND_EDM_SENDER_TASK_H__
#define __EDM_FRIEND_EDM_SENDER_TASK_H__
#include <string>
#include <map>
#include <IceUtil/RWRecMutex.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "MiscEdmSenderCache.h"
namespace xce {
namespace edm {
using namespace  MyUtil;

class FriendEdmSenderTask : public Task {
  public :
    FriendEdmSenderTask() : Task(TASK_LEVEL_PRELOAD),user_id_(-1) {}
    FriendEdmSenderTask(const int& id) : Task(TASK_LEVEL_PRELOAD),user_id_(id) {}
    virtual void handle();
    int GetFriendToRecommend();
    void Send(const int& id);
  private :
    int user_id_;
};


}
}
#endif
