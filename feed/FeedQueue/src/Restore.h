/*
 * Restore.h
 *
 *  Created on: Feb 22, 2011
 *      Author: yejingwei
 */

#ifndef RESTORE_H_
#define RESTORE_H_

#include <map>
#include "TaskManager.h"
#include "Singleton.h"
#include <vector>
#include "Date.h"

using namespace std;
using namespace MyUtil;
namespace xce {
namespace feed {

class Unit;
typedef IceUtil::Handle<Unit> UnitPtr;
class MyQueue;
typedef IceUtil::Handle<MyQueue> MyQueuePtr;
class QueueManager;
typedef IceUtil::Handle<QueueManager> QueueManagerPtr;

class Restorer: public Singleton<Restorer> {
  static const int MAX_RETRY = 2;
  static const int MAX_LOG = 24 * 10;
public:
  void Init();
  void Restore(const string & begintime, const string & endtime,int thishour);
  void SetServiceIndex(int i);
  string DumpToString();
private:
  bool SelectLogs(const string & begintime, const string & endtime,int thishour);
  void ParseIntags();
  void ParseOuttags();
  void ParseIntagsFromFile(string & name);
  void ParseOuttagsFromFile(string & name);
  UnitPtr ExtractUnitFromLine(string & s);
  void Clear();
  string AddZero(int i);
  bool CopyOceLog();
  string DateToLogName(MyUtil::Date & date);

  vector<string> inlogs_;
  vector<string> outlogs_;
  //map<Ice::Long, int> outtag_ids_;//查询出来的id
  set<Ice::Long> outtag_ids_;//查询出来的id
  string path_;//绝对路径
  bool ready_;
  //  MyQueuePtr restore_queue_;
  QueueManagerPtr qm_;

};

class RestoreTask: public Task {
public:
  RestoreTask(const string & begin, const string & end, int thishour) :
    begin_(begin), end_(end), this_hour_(thishour) {
  }
  virtual void handle() {
    Restorer::instance().Restore(begin_, end_,this_hour_);
  }
private:
  string begin_, end_;
  int this_hour_;
};

}
}
#endif /* RESTORE_H_ */
