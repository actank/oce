#include "AdNotifierI.h"
#include "AdCache.h"
#include "ServiceI.h"

using namespace xce::ad;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdNotifierI::instance(), service.createIdentity("M", ""));
  CacheManager::instance().Init();
  TaskManager::instance().config(ThreadPoolConfig(1, 10));
  TaskManager::instance().scheduleRepeated(new ScanTask());
  int delay = ClearAdDailyPostedNumTask::DelayToXClock(1);
  TaskManager::instance().schedule(new ClearAdDailyPostedNumTask(delay));
  TaskManager::instance().schedule(new ClearUserWeeklyReceivedTask(5 * 60 * 1000)); // 五分钟后
}

void AdNotifierI::Login(int uid, const Ice::Current&) {
  TaskManager::instance().execute(new LoginTask(uid));
}

void AdNotifierI::Block(int uid, int promotion, const Ice::Current&) {
  MCE_INFO("AdNotifierI::Block uid=" << uid);
  TaskManager::instance().execute(new BlockTask(uid, promotion));
}

void AdNotifierI::Reload(const Ice::Current&) {
  CacheManager::instance().Init();
}

void AdNotifierI::SetInterval(int uid, IntervalType interval, const Ice::Current&) {
  return;
}

void AdNotifierI::MarkReadByUid(int uid, const Ice::Current&) {
  MCE_INFO("AdNotifierI::MarkReadByUid uid=" << uid);
  TaskManager::instance().execute(new MarkReadByUidTask(uid));
}

void AdNotifierI::LoadOnePromotion(int promotion, const Ice::Current&) {
  MCE_INFO("function LoadOnePromotion() outdated");
  return;
}

void AdNotifierI::DelPromotion(int promotion, const Ice::Current&) {
  TaskManager::instance().execute(new DelPromotionTask(promotion));
}

HotAdInfoSeq AdNotifierI::GetAdInfoSeq(const Ice::Current&) {
  return CacheManager::instance().GetAdInfoSeq();
}

bool AdNotifierI::ResetUserAd(Ice::Int uid, Ice::Int ad_id, const Ice::Current&) {
  return CacheManager::instance().ResetUserAd(uid, ad_id);
}

UserHotAdState AdNotifierI::GetUserAdState(Ice::Int uid, Ice::Int ad_id, const Ice::Current&) {
  return CacheManager::instance().GetUserAdState(uid, ad_id);
}

void LoginTask::handle() {
  CacheManager::instance().Login(uid_);
}

void BlockTask::handle() {
  CacheManager::instance().Block(uid_, promotion_);
}

void MarkReadByUidTask::handle() {
  CacheManager::instance().MarkReadByUid(uid_);
}

void DelPromotionTask::handle() {
  CacheManager::instance().DelAd(promotion_);
}

