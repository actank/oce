/*
 * Queue.h
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef QUEUE_H_
#define QUEUE_H_


#include <vector>
#include <queue>
#include "IceUtil/Thread.h"
#include "IceUtil/Monitor.h"
#include "Singleton.h"
#include "Ice/Config.h"
#include "QueryRunner.h"

using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace std;
namespace xce {
namespace feed {

const static string kInTag = "##INTAG##";
const static string kOutTag = "##OUTTAG##";

const static string kInTagNoRestore = "##INTAG_NORESTORE##";
const static string kOutTagNoRestore = "##OUTTAG_NORESTORE##";

class Unit: public IceUtil::Shared {
public:
  Unit(Ice::Long id, int retry, const string & instance, int wrflag,
      const string &pattern, const string& statement,bool restore);
  string DumpToString();
  string MakeInLog();
  string MakeOutLog();
//  SqlInfoPtr info_;
  string instance_;
  int wrflag_;
  string pattern_;
  string statement_;
  Ice::Long id_;
  int retry_;
  bool restore_;
};
typedef IceUtil::Handle<Unit> UnitPtr;

class QueueManager;
typedef IceUtil::Handle<QueueManager> QueueManagerPtr;

class SqlQueue: public IceUtil::Monitor<IceUtil::Mutex>, public IceUtil::Thread {
public:
  void Init(QueueManager * manager);
  void Push(const UnitPtr& unit);
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
  QueueManagerPtr manager_;
};

typedef IceUtil::Handle<SqlQueue> SqlQueuePtr;

class QueueManager: public IceUtil::Shared {
public:
  void Init(int size, int sqlsize) {
    for (int i = 0; i != size; ++i) {
      SqlQueuePtr mq = new SqlQueue;
      mq->Init(this);
      queues_.push_back(mq);
    }
    cur_index_ = 0;
    size_sql_ = sqlsize;
    Reset();
  }
  void Push(long id,int retry,const string & instance,int wrflag,const string & pattern,const Statement & statement,bool restore = false);

  int Size() {
    return count_;
  }

  int exe_cnt() {
    int exe_cnt = 0;
    for (unsigned i = 0; i != queues_.size(); ++i) {
      exe_cnt += queues_.at(i)->exe_cnt();
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
  void Update(int i){
    IceUtil::Mutex::Lock lock(mutex_);
    count_ += i;
  }
  int NextIndex(){
    cur_index_ = ++cur_index_ % queue_size_;
    return cur_index_;
  }
private:
  vector<SqlQueuePtr> queues_;
  //  int size_q_;
  int size_sql_;
  int cur_index_;
  int queue_size_;
  int push_cnt_,abort_cnt_;
  int count_;
  IceUtil::Mutex mutex_;

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
