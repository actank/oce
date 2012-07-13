#ifndef ADNOTIFY_ICE
#define ADNOTIFY_ICE

#include <Util.ice>
module xce {
module ad { 

enum IntervalType {
  Immediate,
  Week,
  Month
};

enum HotAdState {
  kAsUnloaded,         // db中存在，但内存中尚未加载
  kAsActive,
  kAsReachDailyLimit,  // 发送数目达到当日最大, 暂停
  kAsPaused,
  kAsExpired
};

struct HotAdInfo {
  int ad;
  HotAdState state;
  int level;
  int begintime;
  int endtime;
  int minPartners;
  int maxPartners;
  int maxDailyPost;
  int dailyPosted;
  int partnerCount;
};

struct UserHotAdState {
  int uid;
  int adId;
  int weeklyReceived;
  int unreadCount;

  int totalBuddies;
  int partnerBuddies;
  bool hasBlocked;
  bool hasReceived;
  bool isPartner;
};

sequence<HotAdInfo> HotAdInfoSeq;
dictionary<int, MyUtil::IntSeq> AdPartners;

class AdNotifier {
  AdPartners GetAdPartners(int uid, MyUtil::IntSeq ads);
  void Login(int uid);
  void Block(int uid, int promotion);
  void SetInterval(int uid, IntervalType time);
  void MarkReadByUid(int uid);

  void LoadOnePromotion(int promotion);
  void AddPromotion(int promotion, MyUtil::IntSeq partners, long starttime, long endtime, int level, bool blockalways);
  void DelPromotion(int promotion);
  
  // 获取内存中的 ad 状态信息列表, 因为内存跟db可能不一致
  HotAdInfoSeq GetAdInfoSeq();
  void Reload();

  bool ResetUserAd(int uid, int aid);
  UserHotAdState GetUserAdState(int uid, int aid);
};

};
};

#endif
