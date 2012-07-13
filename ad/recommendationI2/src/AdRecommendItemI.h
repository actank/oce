/*
 * AdRecommendationHandlerI.h
 *
 *  Created on: 2010-11-22
 *      Author: sunzz
 */

#ifndef ADRECOMMENDITEMI_H_
#define ADRECOMMENDITEMI_H_

#include  <Ice/Ice.h>
//#include "AdRecommendItem.h"
#include "AdRecommendationHandler.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "AdRecommendation.h"
#include "../../util/src/AdCommonUtil.h"
namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;

const int TASK_LEVEL_CALCULATE = -1;
const size_t WaitListNum = 100;
/*class TimeStat {
  timeval _begin, _end;
public:
  TimeStat() {
    reset();
  }
  void reset() {
    gettimeofday(&_begin, NULL);
  }
  float getTime() {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec) + _end.tv_usec
        - _begin.tv_usec;
    timeuse /= 1000;
    return timeuse;
  }
};
*/
class AdRecommendationHandlerI : public AdRecommendationHandler,
    public Singleton<AdRecommendationHandlerI> {

public:
  AdRecommendationHandlerI() :
    init_ok_(false) {
  }
//  virtual AdResult GetAds(int userid, const Ice::Current& = Ice::Current());
//  virtual void Click(int userid, Ice::Long groupid, const Ice::Current& =
//      Ice::Current());
  virtual AdKNN GetKNN(Ice::Long groupid,const Ice::Current& = Ice::Current());

  virtual AdResult GetAds(int userid, const Ice::Current& = Ice::Current());
  virtual AdResultE GetRecommend(int userid, const Ice::Current& = Ice::Current());
  virtual void Click(int userid, Ice::Long groupid, const Ice::Current& =
      Ice::Current());
  virtual void ClickNew(int userid, Ice::Long groupid, bool flag,
      const Ice::Current& = Ice::Current());
//  virtual void ClickNew2(int userid, Ice::Long groupid, bool flag, Ice::Long zoneid,
//      const Ice::Current& =Ice::Current());

  virtual bool AddUserListH(int userid, int server_index, const Ice::Current& =
        Ice::Current());
  virtual bool AddUserListL(int userid, int server_index, const Ice::Current& =
        Ice::Current());
  virtual void CleanUserClick(int userid, const Ice::Current& = Ice::Current());
  virtual AdSeq GetUserItem(int userid, const Ice::Current& = Ice::Current());//为了测试
  virtual double GetUserDistance(int userid1, int userid2, const Ice::Current& =//test
        Ice::Current());//为了测试
  virtual UserSeq GetGidUser(Ice::Long groupid,const Ice::Current& = Ice::Current());//test
  virtual bool IsValidAd(Ice::Long groupid,const Ice::Current& = Ice::Current());//test
  virtual UserSeq GetInvalidList(const Ice::Current& = Ice::Current());//test
//  virtual AdSeq GetUserItem(int userid, const Ice::Current& = Ice::Current());//为了测试
 // virtual double GetUserDistance(int userid1, int userid2, const Ice::Current& =//test
 //     Ice::Current());//为了测试
//  virtual UserSeq GetGidUser(Ice::Long groupid,const Ice::Current& = Ice::Current());//test
//  virtual bool IsValidAd(Ice::Long groupid,const Ice::Current& = Ice::Current());//test
//  virtual UserSeq GetInvalidList(const Ice::Current& = Ice::Current());//test
  void Init();
  bool init_ok() const {
    return init_ok_;
  }
  void set_init_ok(bool init_ok) {
    this->init_ok_ = init_ok;
  }

private:
//  void UpdateGidUid(int userid, Ice::Long groupid);
//  void UpdateUidGid(int userid, Ice::Long groupid);
  bool init_ok_;

};

class  UpdateGidUidTask: public MyUtil::Task {
public:
  UpdateGidUidTask(int uid, Ice::Long groupid):uid_(uid),groupid_(groupid) {
  }
  virtual void handle() {
    MCE_DEBUG("UpdateGidUidTask:start: userid:" << uid_ << ",gid:" << groupid_);
    UpdateGidUid(uid_, groupid_);
  }
  void UpdateGidUid(int userid, Ice::Long groupid);
  int uid_;
  Ice::Long groupid_;

};
class  UpdateUidGidTask: public MyUtil::Task {
public:
  UpdateUidGidTask(int uid, Ice::Long groupid):uid_(uid),groupid_(groupid) {
  }
  virtual void handle() {
    MCE_DEBUG("UpdateUidGidTask:start: userid:" << uid_ << ",gid:" << groupid_);
    UpdateUidGid(uid_, groupid_);
  }
  void UpdateUidGid(int userid, Ice::Long groupid);
  int uid_;
  Ice::Long groupid_;
};

class GetAdsFromReSTask:public MyUtil::Task {
public:
  GetAdsFromReSTask(int u):userid(u) {

  }
  virtual void handle() {

  }
  int userid;
};
class GetAdsFromReSTimer : virtual public MyUtil::Timer {
  public:
    GetAdsFromReSTimer() :
      Timer(1 * 1 * 1000) {
    }
    virtual void handle() {
    }
};

/***********************************************************************************************************************/
class InitTask : virtual public MyUtil::Task {
public:
  InitTask() {
  }
  virtual void handle();
};

class ReloadTimer : virtual public MyUtil::Timer {
public:
  ReloadTimer() :
    Timer(1 * 60 * 1000) {
  }
  virtual void handle();
};

/*class UpdateKNNTimer : virtual public MyUtil::Timer {
public:
	UpdateKNNTimer() :
    Timer(60 * 60 * 1000) {
  }
  virtual void handle() {
    ItemUsersPool::instance().UpdateKNNItem();
  }
};*/


class AdRecommendLoader : public MyUtil::Singleton<AdRecommendLoader> {
public:
  AdRecommendLoader() {
    TaskManager::instance().execute(new AdRecommendLoaderMainTask);
  }
  void AddUserId(int user_id);
  int GetUserId();
  bool AddUserListH(int userid);
  bool AddUserListL(int userid);
//  bool Load();
private:
  //bool Execute(int user_id);
  void PrintList(string callname);
  void execute(int userid);


  set<int> set_;
  list<int> listH_;
  list<int> listL_;
  IceUtil::Mutex mutex_;

  class AdRecommendLoaderMainTask : virtual public MyUtil::Task {
  public:
    AdRecommendLoaderMainTask() {
    }
    virtual void handle() {
      while (true) {
        try {
          int uid = AdRecommendLoader::instance().GetUserId();
          if (uid) {
            TaskManager::instance().execute(new AdRecommendLoaderTask(uid));
          } else {
            usleep(20 * 1000);
          }
        } catch (Ice::Exception& e) {
          MCE_WARN("AdRecommendLoaderMainTask --> handle err:" << e << ", line:"
              << __LINE__);
        } catch (...) {
          MCE_WARN("AdRecommendLoaderMainTask --> handle, line:" << __LINE__);
        }
      }
    }
  };

  class AdRecommendLoaderTask : virtual public MyUtil::Task {
  public:
    AdRecommendLoaderTask(int user_id) :
      Task(TASK_LEVEL_CALCULATE), user_id_(user_id) {
    }
    virtual void handle() {
      try {
        AdRecommendLoader::instance().execute(user_id_);
      } catch (Ice::Exception& e) {
        MCE_WARN("AdRecommendLoaderTask --> handle err:" << e << ", line:"
            << __LINE__);
      } catch (...) {
        MCE_WARN("AdRecommendLoaderTask --> handle, line:" << __LINE__);
      }
    }
  private:
    int user_id_;
  };
};



}
}
#endif /* ADRECOMMENDATIONHANDLERI_H_ */
