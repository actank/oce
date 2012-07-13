/*
 * AdNotifierI.cpp
 *
 *  Created on: Jun 25, 2010
 *      Author: yejingwei
 */

#include "AdNotifierI.h"
#include "AdCache.h"
#include "ServiceI.h"

using namespace xce::ad;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdNotifierI::instance(), service.createIdentity(
      "M", ""));
  CacheManager::instance().Init();
  TaskManager::instance().config(ThreadPoolConfig(1, 10));
  TaskManager::instance().scheduleRepeated(new ScanTask());
  TaskManager::instance().scheduleRepeated(new ClearAdDailyPostedNumTask(ClearAdDailyPostedNumTask::DelayToXClock(1)));
}

//-----------------------------------------------------------------------------------------------
void AdNotifierI::Login(int uid, const Ice::Current&) {
  try {
    TaskManager::instance().execute(new LoginTask(uid));
  } catch (Ice::Exception& e) {
    MCE_WARN("AdNotifierI::Login: fail -->" << e);
  } catch (std::exception& e) {
    MCE_WARN("AdNotifierI::Login:  fail -->" << e.what());
  }

}

void AdNotifierI::Block(int uid, int promotion, const Ice::Current&) {
  TaskManager::instance().execute(new BlockTask(uid, promotion));
}

void AdNotifierI::SetInterval(int uid, Interval interval, const Ice::Current&) {
  TaskManager::instance().execute(new SetIntervalTask(uid, interval));
}

void AdNotifierI::MarkReadByUid(int uid, const Ice::Current&) {
  TaskManager::instance().execute(new MarkReadByUidTask(uid));
}

void AdNotifierI::LoadOnePromotion(int promotion, const Ice::Current&) {
  TaskManager::instance().execute(new LoadOnePromotionTask(promotion));
}

void AdNotifierI::DelPromotion(int promotion, const Ice::Current&) {
  TaskManager::instance().execute(new DelPromotionTask(promotion));
}
//-----------------------------------------------------------------------------------------------

void LoginTask::handle() {
  set<int> test; //测试用
  test.insert(220678672);//yjw大号
  test.insert(322856518);//yjw小号
  test.insert(224693355);//hll小号
  test.insert(128487631);//zta小号
  test.insert(322917514);//gaoxiao 1
  test.insert(325241002);//gaoxiao 2
  test.insert(326688331);//gaoxiao 3
  test.insert(225092473);//ld
  test.insert(322276507);//徐冰
  test.insert(273876354);
  test.insert(287726518);
  test.insert(307830983);
  test.insert(242332421);
  test.insert(239291356);//yj1
  test.insert(227216498);//yj2

  MCE_DEBUG("LoginTask::handle --> test account = " << uid_);
  CacheManager::instance().Login(uid_);
  return;
}

void BlockTask::handle() {
  CacheManager::instance().Block(uid_, promotion_);
}
void SetIntervalTask::handle() {
  CacheManager::instance().SetInterval(uid_, interval_);
}
void MarkReadByUidTask::handle() {
  CacheManager::instance().MarkReadByUid(uid_);
}

void LoadOnePromotionTask::handle() {
  CacheManager::instance().LoadOneAd(promotion_);
}

void DelPromotionTask::handle() {
  set<int> tmp;
  tmp.insert(promotion_);
  CacheManager::instance().DelAds(tmp);
}

