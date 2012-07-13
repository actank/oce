/*
 * FeedQueueI.h
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef FEEDQUEUEI_H_
#define FEEDQUEUEI_H_

#include "FeedQueue.h"
#include <queue>
#include "Singleton.h"
#include "TaskManager.h"

//#include "MyQueue.h"
using namespace std;
using namespace MyUtil;
namespace xce {
namespace feed {

class MyQueue ;
typedef IceUtil::Handle<MyQueue> MyQueuePtr;
class Unit;
typedef IceUtil::Handle<Unit> UnitPtr;
class QueueManager;
typedef IceUtil::Handle<QueueManager> QueueManagerPtr;

class FeedQueueI : public FeedQueue, public MyUtil::Singleton<FeedQueueI>{
public:
  virtual int Push(const SqlInfoPtr& sql,const Ice::Current& =  Ice::Current());
  virtual void TestPush(const SqlInfoPtr& sql,const Ice::Current& =  Ice::Current());
  virtual void Restore(const string&, const string&, int thishour,const ::Ice::Current& = ::Ice::Current());
  void Init();
  int Size();
  string DumpToString();
private:
  void MakeLogsName(vector<string> & inlogs,vector<string> & outlogs){};
  QueueManagerPtr qm_;
};

class QueueStatTimer: public Timer, public Singleton<QueueStatTimer> {
public:
  QueueStatTimer() :
    Timer(5 * 1000, 1) {
  }
  void handle() ;
};
}
}
#endif /* FEEDQUEUEI_H_ */
