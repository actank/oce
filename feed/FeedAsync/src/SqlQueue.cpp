/*
 * SqlQueue.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "SqlQueue.h"
#include <IceUtil/Monitor.h>
#include <IceUtil/Thread.h>
#include "QueryRunner.h"
#include "Date.h"
#include "Common.h"

using namespace MyUtil;
using namespace xce::feed;
using namespace IceUtil;
using namespace com::xiaonei::xce;

Unit::Unit(Ice::Long id, int retry, const string & instance, int wrflag,
    const string &pattern, const string& statement,bool restore) {
  id_ = id;
  retry_ = retry;
  instance_ = instance;
  wrflag_ = wrflag;
  pattern_ = pattern;
  statement_ = statement;
  restore_ = restore;
}

string Unit::DumpToString() {
  ostringstream oss;
  oss << '\t' << id_ << '\t' << retry_ << '\t' << instance_ << '\t'
      << wrflag_ << '\t' << pattern_ << '\t' << statement_;
  return oss.str();
}

string Unit::MakeInLog(){
  string tag = restore_?kInTag:kInTagNoRestore;
  return tag + DumpToString();
}
string Unit::MakeOutLog(){
  string tag = restore_?kOutTag:kOutTagNoRestore;
  return tag + DumpToString();
}


void SqlQueue::Push(const UnitPtr& unit) {
  {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    q_.push(unit);
    notify();
  }
  MCE_INFO(unit->MakeInLog());
  manager_->Update(1);
  return;
}

void SqlQueue::Init(QueueManager * manager) {
  manager_ = manager;
  exe_cnt_ = 0;
  start();
}

void SqlQueue::run() {
  while (1) {
    UnitPtr unit;
    {
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
      while (q_.size() == 0) {
        wait();
      }
      unit = q_.front();
      q_.pop();
      ++exe_cnt_;
    }
    manager_->Update(-1);
    Execute(unit);
  }
}

void SqlQueue::Execute(const UnitPtr& unit) {
  //  return;//TODO del
  Statement st;
  st << unit->statement_;
  try {
    MCE_INFO(unit->MakeOutLog());
    //    QueryRunner(unit->info_->instance, unit->info_->wrflag,
    //        unit->info_->pattern).store(st);
    usleep(7000);//db平均为7毫秒
    //    sleep(10000);
  } catch (Ice::Exception& e) {
    MCE_WARN("SqlQueue::Execute --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("SqlQueue::Execute --> error:" << e.what());
  }
}

bool SqlQueue::Check() {
  const static unsigned kMaxLen = 1000000;
  return (q_.size() < kMaxLen);
}

int SqlQueue::Size() {
  return q_.size();
}

void QueueManager::Push(long id,int retry,const string & instance,int wrflag,
    const string & pattern,const Statement & statement,bool restore){
  if(id == -1){
    id = IdGenerator::instance().Generate();
  }
  string str = SqlConverter::instance().Convert(statement);
  UnitPtr u = new Unit(id,retry,instance,wrflag,pattern,str,restore);
  queues_.at(NextIndex())->Push(u);
}


string QueueManager::DumpToString() {
  ostringstream oss;
  oss << " Size:" << Size() << " push_cnt:" << push_cnt_ << " exe_cnt:"
      << exe_cnt() << " aborted_cnt:" << abort_cnt_;
  Reset();
  return oss.str();
}

void IdGenerator::Init() {
  time_t t = time(NULL);
  id_ = t << 32;
  MCE_INFO("IdGenerator::Init. id:" << id_);
}
