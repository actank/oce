/*
 * FeedQueueI.h
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef FEEDNAIVEBAYESI_H_
#define FEEDNAIVEBAYESI_H_

#include <queue>
#include "Singleton.h"
#include "TaskManager.h"
#include "FeedNaiveBayes.h"
#include "ScoreCache.h"
#include "ServiceI.h"
//#include "UserPool.h"

//#include "MyQueue.h"
using namespace std;
using namespace MyUtil;
using namespace xce::feed;
namespace xce {
namespace feed {


class FeedNaiveBayesI: public FeedNaiveBayes, public MyUtil::Singleton<FeedNaiveBayesI> {
public:
	void Init(){
		registered_ = false;
		int testers[] = {239556923,221489137,238489851,240599893,128487631,
		         225092473,241610785,220678672,322856518,34344507,343518722,347348947,347349486,200000032};
		testers_.insert(testers,testers + sizeof(testers));
		MCE_INFO("FeedNaiveBayesI. Init done, testers size:" << testers_.size());
	}
  virtual map<int,int>  GetScoreDict(int fromid,const vector<int> & toids, int stype, const Ice::Current& =
      Ice::Current());
  virtual map<int,int>  GetScoreDictReverse(int to,const vector<int> & fromids, int stype, const Ice::Current& =
      Ice::Current());

  virtual int GetSendFrequency(int fromid, const Ice::Current& =
      Ice::Current());
  virtual void Reload();
  void RegisterController();
  virtual bool Registered( const Ice::Current& =
  		Ice::Current()){
  	return registered_;
  }
  virtual void MergeNew(const string & new_score,const string & dump,const Ice::Current& =
      Ice::Current());
  void IsTester(int uid){

  }
private:
  set<int> GetFocusSource(int target);

  set<int> testers_;
  bool registered_;
};


class ReloadTask: public MyUtil::Task, public MyUtil::Singleton<ReloadTask> {
public:
	ReloadTask() :
		Task() {
	}
	virtual void handle() {
//		MCE_INFO("ReloadTask::handle. BEGIN");
//		FrequencyCache::instance().Reload();
//		MCE_INFO("ReloadTask::handle. END");
//		xce::feed::FeedNaiveBayesI::instance().RegisterController();
		if (ScoreCache::instance().Reload()) {
			xce::feed::FeedNaiveBayesI::instance().RegisterController();
		}
	}
private:

};

class TimeStat{
  timeval _begin, _end;
  bool _log;
public:
  TimeStat(bool log=false){
    _log = log;
    reset();
  }
  ~TimeStat(){
    if(_log){
      MCE_INFO("######### " << getTime());
    }
  }
  void reset(){
    gettimeofday(&_begin, NULL);
  }
  float getTime(){
    gettimeofday(&_end, NULL);
    float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
    -_begin.tv_usec;
    timeuse/=1000;
    return timeuse;
  }


};


}
}
#endif /* FEEDQUEUEI_H_ */
