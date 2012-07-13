/*
 * AdRecommendationHandlerI.h
 *
 *  Created on: 2010-11-22
 *      Author: sunzz
 */

#ifndef ADRECOMMENDATIONHANDLERI_H_
#define ADRECOMMENDATIONHANDLERI_H_

#include  <Ice/Ice.h>
#include "AdRecommendationHandler.h"
#include "Singleton.h"
#include "TaskManager.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;

class TimeStat {
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

class AdRecommendationHandlerI : public AdRecommendationHandler,
    public Singleton<AdRecommendationHandlerI> {

public:
  AdRecommendationHandlerI() :
    init_ok_(false) {
  }
  virtual AdResult GetAds(int userid, const Ice::Current& = Ice::Current());
  virtual void Click(int userid, long groupid, const Ice::Current& =
      Ice::Current());
  virtual AdSeq GetUserItem(int userid, const Ice::Current& = Ice::Current());//为了测试
  virtual float GetUserDistance(int userid1, int userid2, const Ice::Current& =//test
      Ice::Current());//为了测试
  virtual UserSeq GetGidUser(long groupid,const Ice::Current& = Ice::Current());//test
  virtual bool IsValidAd(long groupid,const Ice::Current& = Ice::Current());//test
  virtual UserSeq GetInvalidList(const Ice::Current& = Ice::Current());//test
  void Init();
  bool init_ok() const {
    return init_ok_;
  }
  void set_init_ok(bool init_ok) {
    this->init_ok_ = init_ok;
  }

private:
  void UpdateGidUid(int userid, long groupid);
  void UpdateUidGid(int userid, long groupid);
  bool init_ok_;

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
class CheckValidTimer : public MyUtil::Timer {
public:
  CheckValidTimer() :
    Timer(12 * 60 * 60 * 1000) { //12 hour check

  }
  virtual void handle();
};
/***********************************************************************************************************************/
class RecommendLoader;
typedef IceUtil::Handle<RecommendLoader> RecommendLoaderPtr;

class RecommendLoader : public IceUtil::Shared  {
public:
  RecommendLoader(int index) : index_(index) {
    TaskManager::instance().execute(new RecommendLoaderTask(this));
  }
  void AddUserId(int user_id);
  bool Load();
private:
  void execute(int user_id);

  int index_;
  set<int> set_;
  list<int> list_;
  IceUtil::RWRecMutex mutex_;

  class RecommendLoaderTask : virtual public MyUtil::Task {
  public:
    RecommendLoaderTask(RecommendLoaderPtr loader) : loader_(loader) {
    }
    virtual void handle() {
      while (true) {
        try {
          if (!loader_->Load()) {
            usleep(200 * 1000);
          }
        } catch (Ice::Exception& e) {
          MCE_WARN("RecommendLoaderTask --> handle load err:" << e << ", line:"
              << __LINE__);
        } catch (...) {
          MCE_WARN("RecommendLoaderTask --> handle load , line:" << __LINE__);
        }
      }
    }
  private:
    RecommendLoaderPtr loader_;
  };
};

class LoaderManager : public MyUtil::Singleton<LoaderManager> {
public:
  LoaderManager() {
    for (int i = 0; i < THREAD_COUNT; i++) {
      RecommendLoaderPtr loader = new RecommendLoader(i);
      loader_list_.push_back(loader);
    }
  }
  void AddUserId(int user_id) {
    loader_list_[(user_id /10)% THREAD_COUNT]->AddUserId(user_id);
  }

private:
  vector<RecommendLoaderPtr> loader_list_;
  static const int THREAD_COUNT = 10; //
};


}
}
#endif /* ADRECOMMENDATIONHANDLERI_H_ */
