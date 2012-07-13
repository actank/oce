/*
 * AdNotifierI.h
 *
 *  Created on: Jun 25, 2010
 *      Author: yejingwei
 */

#ifndef ADNOTIFIERI_H_
#define ADNOTIFIERI_H_

#include "AdNotify.h"
#include "Singleton.h"
#include "TaskManager.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;

class AdNotifierI: public AdNotifier, public Singleton<AdNotifierI> {
  virtual void Login(int uid, const Ice::Current& = Ice::Current());
  // 用户block一种类型的promotion
  virtual void Block(int uid, int promotion, const Ice::Current& = Ice::Current());
  virtual void SetInterval(int uid, IntervalType time,const Ice::Current& = Ice::Current());
  virtual void MarkReadByUid(int uid, const Ice::Current& = Ice::Current()); //将uid用户的所有message设为已读

  // 将一种promotion的信息加载到内存
  virtual void LoadOnePromotion(int promotion, const Ice::Current& = Ice::Current());
  virtual void AddPromotion(int promotion, const ::MyUtil::IntSeq& partners, //取消该函数
      long starttime, long endtime, int level, bool blockable,
      const Ice::Current& = Ice::Current()) {}
  //删除一种promotion
  virtual void DelPromotion(int promotion, const Ice::Current& = Ice::Current());
  virtual xce::ad::AdPartners GetAdPartners(Ice::Int, const MyUtil::IntSeq&, const Ice::Current&);

  // 管理接口
  virtual HotAdInfoSeq GetAdInfoSeq(const Ice::Current& = Ice::Current());
  virtual void Reload(const Ice::Current& = Ice::Current());

  // 方便测试
  virtual bool ResetUserAd(Ice::Int uid, Ice::Int ad_id, const Ice::Current& = Ice::Current()); // 将 uid 的 ad_id 状态清空, 便于重新接收 ad_id
  virtual UserHotAdState GetUserAdState(Ice::Int uid, Ice::Int ad_id, const Ice::Current& = Ice::Current());
};

class LoginTask: public Task {
public:
  LoginTask(int uid) :
    uid_(uid) {
  }
  virtual void handle();
private:
  int uid_;
};

class BlockTask: public Task {
public:
  BlockTask(int uid, int promotion) :
    uid_(uid), promotion_(promotion) {
  }
  virtual void handle();
private:
  int uid_;
  int promotion_;
};

class MarkReadByUidTask: public Task {
public:
  MarkReadByUidTask(int uid) :
    uid_(uid) {
  }
  virtual void handle();
private:
  int uid_;
};

class DelPromotionTask: public Task  {
public:
  DelPromotionTask(int promotion) : promotion_(promotion) {
  }
  virtual void handle();
private:
  int promotion_;
};

}
}

#endif /* ADNOTIFIERI_H_ */

