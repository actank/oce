/*
 * Queue.h
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "FeedQueue.h"
#include <vector>
#include <queue>
#include "IceUtil/Thread.h"
#include "IceUtil/Monitor.h"
#include "Singleton.h"


using namespace std;
namespace xce {
namespace feed {

const static string kInTag = "##INTAG##";
const static string kOutTag = "##OUTTAG##";

const static string kInTagNoRestore = "##INTAG_NORESTORE##";
const static string kOutTagNoRestore = "##OUTTAG_NORESTORE##";

class Unit: public IceUtil::Shared {
public:
  Unit(const SqlInfoPtr & info);
  Unit(Ice::Long id, int retry, const string & instance, int wrflag,
      const string &pattern, const string& statement);
  string DumpToString();
  string MakeInLog();
  string MakeOutLog();
  SqlInfoPtr info_;
  Ice::Long id_;
  int retry_;
};
typedef IceUtil::Handle<Unit> UnitPtr;

class MyQueue: public IceUtil::Monitor<IceUtil::Mutex>, public IceUtil::Thread {
public:
  void Init();
  int Push(const SqlInfoPtr& info);
  void PushUnit(const UnitPtr& unit);
  virtual void run();
  int Size();
  int exe_cnt(bool reset = true){
    int tmp = exe_cnt_;
    if(reset){
      exe_cnt_ = 0;
    }
    return tmp;
  }
private:
  void Execute(const UnitPtr& unit);
  bool Check();
  queue<UnitPtr> q_;
  int exe_cnt_;
};

typedef IceUtil::Handle<MyQueue> MyQueuePtr;

class QueueManager: public IceUtil::Shared {
public:
  void Init(int size, int sqlsize) {
    for (int i = 0; i != size; ++i) {
      MyQueuePtr mq = new MyQueue;
      mq->Init();
      qs_.push_back(mq);
    }
    cur_index_ = 0;
    size_sql_ = sqlsize;
    Reset();
  }
  int Push(const SqlInfoPtr& info) {
    if(!Check()){
      ++abort_cnt_;
      return 0;
    }
    qs_.at((cur_index_++)% qs_.size())->Push(info);
    ++push_cnt_;
    return 1;
  }
  void PushUnit(const UnitPtr& unit);
  int Size() {
    int cnt = 0;
    for (unsigned i = 0; i != qs_.size(); ++i) {
      cnt += qs_.at(i)->Size();
    }
    return cnt;
  }

  int exe_cnt() {
    int exe_cnt = 0;
    for (unsigned i = 0; i != qs_.size(); ++i) {
      exe_cnt += qs_.at(i)->exe_cnt();
    }
    return exe_cnt;
  }
  bool Check() {
    return (Size() < size_sql_);
  }
  void Reset(){
    push_cnt_ = abort_cnt_ = 0;
  }
  string DumpToString();
private:
  vector<MyQueuePtr> qs_;
  //  int size_q_;
  int size_sql_;
  int cur_index_;

  int push_cnt_,abort_cnt_;
};
typedef IceUtil::Handle<QueueManager> QueueManagerPtr;

class IdGenerator: public MyUtil::Singleton<IdGenerator> {
public:
  void Init();
  Ice::Long Generate() {
    return id_++;
  }
private:
  Ice::Long id_;
};

}
}

#endif /* QUEUE_H_ */
