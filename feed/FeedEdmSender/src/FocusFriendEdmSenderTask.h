#ifndef __EDM_FOCUS_FRIEND_EDM_SENDER_TASK_H__
#define __EDM_FOCUS_FRIEND_EDM_SENDER_TASK_H__
#include <string>
#include <map>
#include <IceUtil/RWRecMutex.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "FriendRank.h"
#include "MiscEdmSenderCache.h"
namespace xce {
namespace edm {
using namespace  MyUtil;
class FocusFriendEdmSenderTask : public Task {
  public :
    FocusFriendEdmSenderTask() : Task(TASK_LEVEL_PRELOAD),user_id_(-1) {}
    FocusFriendEdmSenderTask(const int& id) : Task(TASK_LEVEL_PRELOAD),user_id_(id) {}
    virtual void handle();
    void Send();
    static bool Op(const xce::socialgraph::RankData& p1,const xce::socialgraph::RankData& p2);
  private :
    int user_id_;
};
}
}
#endif
