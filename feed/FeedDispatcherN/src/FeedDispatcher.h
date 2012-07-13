#ifndef __FEED_DISPATCHER_H__
#define __FEED_DISPATCHER_H__

#include "RFeed.h"

#include "ServiceI.h"
#include "QueryRunner.h"
#include "ExpressionCache.h"
#include "LogWrapper.h"
#include "boost/foreach.hpp"
namespace xce {
namespace feed {

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;

const static string FEED_DISPATCHER = "FD";

class FeedDispatcherNI: public FeedDispatcher, public Singleton<
    FeedDispatcherNI> {
public:
  FeedDispatcherNI();
  virtual void dispatch(const FeedSeedPtr&, const FeedConfigPtr&,
      const Ice::Current& = Ice::Current());
  virtual void dispatchReply(const ReplyDataPtr&, const FeedConfigPtr&,
      const Ice::Current& = Ice::Current());
  virtual void Reload(LoadType type, int id, const Ice::Current& =
      Ice::Current());
  virtual void ReloadBatch(LoadType type, const vector<int> & ids,
      const Ice::Current& = Ice::Current());
  virtual void addBuddyNotify(int host, int guest, const Ice::Current& =
      Ice::Current());
  virtual void buddyApplyNotify(int host, int guest, const Ice::Current& =
      Ice::Current());
  virtual void joinPageNotify(int page, int fans, const Ice::Current& =
      Ice::Current());
  virtual void guideBuddyReplyNotify(int newUser, int buddy,
      const Ice::Current& = Ice::Current());

private:
  void AppendNewsFeed(int from, int to, int limit);
  void AppendNewsFeed(int from, int to, int type, int limit);
  string GetIp(const Ice::Current& current){
  	if(!current.con){
  		return string();
  	}
  	string ip = current.con->toString();
  	size_t found = ip.find('\n');
  	if (found == string::npos) {
  		return ip;
  	}
  	string res ( ip.begin() + found + 1,ip.end());
  	return res;
  }
  //  string MakeExpression(LoadType type,int id);
};

class DispatchTask: public Task {
public:
  DispatchTask(FeedSeedPtr inFeed, FeedConfigPtr inConfig) :
    Task((inFeed->actor / 10) % 100), feed_(inFeed), config_(inConfig) {

    MCE_DEBUG("run DispatchTask, id:" << inFeed->feed);
  }
  virtual void handle();
  int GetAppId();
private:
  map<int,int> GetBayesScores(int actor,const set<int> & target,int stype);
  void GetFocusSource(const FeedSeedPtr & seed,map<int,int> & rank);
  FeedSeedPtr feed_;
  FeedConfigPtr config_;
};

class DispatchReplyTask: public Task {
public:
  DispatchReplyTask(ReplyDataPtr reply, FeedConfigPtr config) :
    Task((reply->actor / 10) % 100), reply_(reply), config_(config) {
    MCE_DEBUG("run DispatchTask");
  }
  virtual void handle();
private:
  ReplyDataPtr reply_;
  FeedConfigPtr config_;

};

struct Record {
  Record() :
    count_(0), target_(0), time_(0.0f) {
  }
  int count_;
  long target_;
  float time_;
};

//class StatTimer: public Timer,public Singleton<StatTimer>  {
//public:
//  StatTimer():Timer(10*1000, 0),idx_(0){
//
//  }
//  void Update(int uid,const string & type,float t,int target){
//    if(!idx_){
//      idx_ = uid % 10;
//    }
//
//    map<string,Record>::iterator i = stat_.find(type);
//    if(i != stat_.end()){
//      ++(i->second.count_);
//      i->second.time_ += t;
//      i->second.target_ += target;
//    }else{
//      Record r;
//      r.count_ = 1;
//      r.target_ = target;
//      r.time_ = t;
//      stat_[type] = r;
//    }
//  }
//  virtual void handle(){
//    MCE_INFO("StatTimer::handle. " << Dump());
//  }
//  string Dump();
//private:
//  map<string,Record> stat_;
//  int idx_;
//
//};


class TimeStat {
  timeval _begin, _end;
  bool _log;
public:
  TimeStat(bool log = false) {
    _log = log;
    reset();
  }
  ~TimeStat() {
    if (_log) {
      MCE_INFO("######### " << getTime());
    }
  }
  void reset() {
    gettimeofday(&_begin, NULL);
  }
  float getTime(bool clear = false) {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec) + _end.tv_usec
        - _begin.tv_usec;
    timeuse /= 1000;
    if (clear) {
      reset();
    }
    return timeuse;
  }
};

struct DispatchCost {
  DispatchCost() :
    send_cfg_(0.0f), expr_(0.0f), recv_cfg_(0.0f), friend_(0.0f), mini_(0.0f),
        log_(0.0f), wtran_(0.0f), news_(0.0f), interaction_(0.0f),
        photo_(0.0f), total_(0.0f) ,range_(0.0f){
  }
  float send_cfg_, expr_, recv_cfg_, friend_, mini_, log_, wtran_, news_,
      interaction_, photo_, total_,range_;
  string ToString();
};

class PrintHelper {
public:
  template<typename T>
  static string Print(const T & container,int limit = 30) {
    ostringstream ios;

    ios << "(";
    int idx = 0;
    typedef typename T::value_type vt;
    using namespace boost;
    BOOST_FOREACH(const vt & v,container){
    	if(idx++ > 0) ios << ",";

    	ios << v;

      if(idx >= limit) break;
    }
    ios << ")";

    return ios.str();
  }
};


class InteractionUploader : public MyUtil::Singleton<InteractionUploader>{
public:
	void UploadReply(long fid,int r);
	void UploadRange(long fid,int r);
private:
  map<int,long> map_;
	IceUtil::Mutex mutex_;
};

}
;
}
;

#endif
