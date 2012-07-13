/*
 * FeedQueueI.h
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef FEEDQUEUEI_H_
#define FEEDQUEUEI_H_

#include "FeedAsync.h"
#include <queue>
#include "Singleton.h"
#include "TaskManager.h"
#include "SqlQueue.h"

//#include "MyQueue.h"
using namespace std;
using namespace MyUtil;
namespace xce {
namespace feed {

class SqlQueue;
typedef IceUtil::Handle<SqlQueue> SqlQueuePtr;
class Unit;
typedef IceUtil::Handle<Unit> UnitPtr;
class QueueManager;
typedef IceUtil::Handle<QueueManager> QueueManagerPtr;

class FeedAsyncI: public FeedAsync, public MyUtil::Singleton<FeedAsyncI> {
public:

  virtual bool AddFeedIndex(const FeedIndexPtr & idx, const Ice::Current& =
      Ice::Current());
  virtual bool RemoveFeedIndex(const MyUtil::LongSeq& feeds, const Ice::Current& =
      Ice::Current());
  virtual bool AddFeedContent(const FeedSeedPtr & seed, const Ice::Current& =
      Ice::Current());
  virtual bool RemoveFeedContent(const MyUtil::LongSeq& feeds, const Ice::Current& =
      Ice::Current());

  virtual bool AddFeedMini(const FeedSeedPtr& seed, const Ice::Current& =
      Ice::Current());
  virtual bool RemoveFeedMini(int user, int stype, Ice::Long miniMerge,
      const Ice::Current& = Ice::Current());
  virtual bool RemoveAllFeedMini(int user, const Ice::Current& = Ice::Current());

  virtual bool AddFeedSchool(int schoolid, const FeedSeedPtr& seed,
      const Ice::Current& = Ice::Current());
  virtual bool AddFeedGroup(int groupid, const FeedSeedPtr& seed, const Ice::Current& =
      Ice::Current());

//  virtual void Restore(const string&, const string&, int thishour,
//      const ::Ice::Current& = ::Ice::Current());
  void Init();
  int Size();
  string DumpToString();
private:
  string getIndexTab(Ice::Long feed) {
    ostringstream tab;
    tab << "feed_index_" << feed % 10;
    return tab.str();
  }
  string getContentTab(Ice::Long feed) {
    ostringstream tab;
    tab << "feed_content_" << feed % 100;
    return tab.str();
  }
  string getMiniTab(int user) {
    ostringstream tab;
    tab << "feed_mini_" << user % 100;
    return tab.str();
  }
  string getSchoolTab(int schoolid) {
    ostringstream tab;
    tab << "feed_school_" << schoolid % 10;
    return tab.str();
  }
  string getGroupTab(int groupid) {
    ostringstream tab;
    tab << "feed_group_" << groupid % 100;
    return tab.str();
  }
  void MakeLogsName(vector<string> & inlogs, vector<string> & outlogs) {
  }
  ;
  QueueManagerPtr qm_;
};

class QueueStatTimer: public Timer, public Singleton<QueueStatTimer> {
public:
  QueueStatTimer() :
    Timer(5 * 1000, 1) {
  }
  void handle();
};
}
}
#endif /* FEEDQUEUEI_H_ */
