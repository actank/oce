#ifndef __EDM_MISC_EDM_SENDER_CACHE_H__
#define __EDM_MISC_EDM_SENDER_CACHE_H__
#include <map>
#include <string>
#include <IceUtil/RWRecMutex.h>
#include "Singleton.h"
#include "TaskManager.h"
namespace xce {
namespace edm {
using namespace  MyUtil;
enum MiscType { kFriend = 8006, kAddFriend = 8007, kMayKnow = 8009, kFocusFriend = 8011};
typedef std::map<Ice::Int,unsigned int> IUMap;
struct TypeInfo {
  MiscType misc_type_;
  int count_;
  std::string var_name_;
  int max_count_;
  unsigned int user_send_limit_;
  unsigned int mask_;
  unsigned int read_mask_;
  unsigned int set_mask_;
  unsigned int right_shift_;
  int type_inDb_;
  double period_;
  TypeInfo(MiscType misc_type, int count, std::string var_name, int max_count,unsigned int user_send_limit, unsigned int mask, unsigned int read_mask, unsigned int set_mask,
      unsigned int right_shift,int type_inDb, double period) : misc_type_(misc_type),count_(count),var_name_(var_name),max_count_(max_count),user_send_limit_(user_send_limit), 
                                                  mask_(mask),read_mask_(read_mask), set_mask_(set_mask),right_shift_(right_shift),type_inDb_(type_inDb),period_(period) {}
};
typedef TypeInfo* TypeInfoPtr;
class MiscEdmSenderCache : public MyUtil::Singleton<MiscEdmSenderCache> {
  public:
    MiscEdmSenderCache();
    ~MiscEdmSenderCache();
    void Initialize();
    void Add(const Ice::Int& user_id, MiscType misc_type, bool load = false);
    bool EvictCache(MiscType misc_type);
    bool OverCount(const Ice::Int& user_id, MiscType misc_type);
    bool LoadFromDb();
  private:
    bool _OverMaxCount(MiscType misc_type);
    bool _OverMaxUserCount(const Ice::Int& user_id, MiscType misc_type);
    int _ReadCountFromDB(TypeInfoPtr type_info);
    TypeInfoPtr _GetTypeInfo(MiscType misc_type);
    TypeInfoPtr _GetTypeInfoFromDbType(int db_type);
    void _ResetCountToDB(MiscType misc_type);
    void _InsertSendRecord(const Ice::Int& user_id, int type); 
  private:
    IceUtil::RWRecMutex mutex_;
    IUMap cache_;
    TypeInfoPtr friend_;
    TypeInfoPtr addfriend_;
    TypeInfoPtr mayknow_;
    TypeInfoPtr focusfriend_;
    /*
    static int friend_count_;
    static int addfriend_count_;
    static int mayknow_count_;
    static int focusfriend_count_;
    static std::string friend_var_name_;
    static std::string addfriend_var_name_;
    static std::string mayknow_var_name_;
    static std::string focusfriend_var_name_;
    static int friend_max_count_;
    static int addfriend_max_count_;
    static int mayknow_max_count_;
    static int focusfriend_max_count_;
    static unsigned int friend_user_send_limit;
    static unsigned int addfriend_user_send_limit;
    static unsigned int mayknow_user_send_limit;
    static unsigned int focusfriend_user_send_limit;
  */
    bool init_flag_;
  private:
    class MiscEdmSendCountTimer : public Timer {
      public:
        MiscEdmSendCountTimer(Ice::Int s = 60) : Timer(s*1000) {}
        virtual void handle();
    };
    class FriendCacheCleaner : public Timer {
      public:
        FriendCacheCleaner(Ice::Int s = 60 * 60 * 24) : Timer(s*1000) {}
        virtual void handle();
    };
    class AddFriendCacheCleaner : public Timer {
      public:
        AddFriendCacheCleaner(Ice::Int s = 60 * 60 * 24) : Timer(s*1000) {}
        virtual void handle();
    };
    class MayKnowCacheCleaner : public Timer {
      public:
        MayKnowCacheCleaner(Ice::Int s = 60 * 60 * 24) : Timer(s*1000) {}
        virtual void handle();
    };
    class FocusFriendCacheCleaner : public Timer {
      public:
        FocusFriendCacheCleaner(Ice::Int s = 60 * 60 * 24 * 7) : Timer(s*1000) {}
        virtual void handle();
    };
};
class MiscEdmSenderCacheLoadTask : public Task {
public:
  MiscEdmSenderCacheLoadTask():Task(TASK_LEVEL_PRELOAD) {}
  virtual void handle();
};
}
}

#endif
