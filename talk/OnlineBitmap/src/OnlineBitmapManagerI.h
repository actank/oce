#ifndef __ONLINEBITMAPMANAGER_H__
#define __ONLINEBITMAPMANAGER_H__

#include <string>
#include <map>
#include <vector>
#include <Ice/Ice.h>
#include "Singleton.h"
#include "TaskManager.h"
#include "OnlineBitmap.h"
#include "OnlineCenter.h"
#include "TalkCommon.h"
#include "string.h"

namespace talk{
namespace onlinebitmap{
  using namespace std;
  using namespace MyUtil;
  using namespace com::xiaonei::talk::common; 
  using namespace ::talk::online;

  static int BITMAP_SIZE = 0x7fffffff;

  class OnlineBitmapManagerI : public OnlineBitmapManager, public Singleton<OnlineBitmapManagerI>{
  public:
    OnlineBitmapManagerI() : bitmap_(NULL) {}

    virtual void setUserStatByOnlineType(const UserOnlineTypePtr& onlineTypePtr, const Ice::Current& = Ice::Current());
    virtual void load(const UserOnlineTypeSeq& onlineTypeSeq, const Ice::Current& = Ice::Current());

    virtual void Reload(const Ice::Current& = Ice::Current());
    virtual void SyncBitmap(const SyncBitmapDataPtr & sync_data, const Ice::Current& = Ice::Current());

		//获取用户在线状态
    virtual UserOnlineTypePtr getUserStat(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual UserOnlineTypeSeq getUserStatByIdSeq(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
		
		//获取用户在线类型
    virtual UserOnlineTypePtr getUserType(Ice::Int userId, const Ice::Current& = Ice::Current());
    virtual UserOnlineTypeSeq getUserTypeByIdSeq(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());

		//获取在线用户
    virtual UserOnlineTypeSeq getOnlineUsers(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());

		//获取在线用户数
    virtual Ice::Int getOnlineCount(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());

    void init();
  private:
    char* bitmap_;
  };
};
};


#endif
