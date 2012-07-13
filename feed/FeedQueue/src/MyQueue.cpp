/*
 * MyQueue.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "MyQueue.h"
#include <IceUtil/Monitor.h>
#include <IceUtil/Thread.h>
#include "QueryRunner.h"
#include "Date.h"

using namespace MyUtil;
using namespace xce::feed;
using namespace IceUtil;
using namespace com::xiaonei::xce;
Unit::Unit(const SqlInfoPtr & info) {
  info_ = info;
  retry_ = 0;
  id_ = IdGenerator::instance().Generate();
}

Unit::Unit(Ice::Long id, int retry, const string & instance, int wrflag,
    const string &pattern, const string& statement) {
  id_ = id;
  retry_ = retry;
  info_ = new SqlInfo();
  info_->instance = instance;
  info_->wrflag = wrflag;
  info_->pattern = pattern;
  info_->statement = statement;
}

string Unit::DumpToString() {
  ostringstream oss;
  oss << '\t' << id_ << '\t' << retry_ << '\t' << info_->instance << '\t'
      << info_->wrflag << '\t' << info_->pattern << '\t' << info_->statement;
  return oss.str();
}

string Unit::MakeInLog(){
  string tag = (info_->flag & 0x000001)?kInTag:kInTagNoRestore;
  return tag + DumpToString();
}
string Unit::MakeOutLog(){
  string tag = (info_->flag & 0x000001)?kOutTag:kOutTagNoRestore;
  return tag + DumpToString();
}

int MyQueue::Push(const SqlInfoPtr& info) {
  //  if (!Check()) {
  //    return 0;
  //  }
  UnitPtr unit;
  {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    unit = new Unit(info);
    q_.push(unit);
    notify();
  }
  if (unit->info_->flag & 0x00000001) {
    MCE_INFO(kInTag << unit->DumpToString());
  } else {
    MCE_INFO(kInTagNoRestore << unit->DumpToString());
  }
  return 1;
}

void MyQueue::PushUnit(const UnitPtr& unit) {
  {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
    q_.push(unit);
    notify();
  }
  if (unit->info_->flag & 0x00000001) {
    MCE_INFO(kInTag << unit->DumpToString());
  } else {
    MCE_INFO(kInTagNoRestore << unit->DumpToString());
  }
  return;
}

void MyQueue::Init() {
  exe_cnt_ = 0;
  start();
}

void MyQueue::run() {
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
    Execute(unit);
  }
}

void MyQueue::Execute(const UnitPtr& unit) {
  //  return;//TODO del
  Statement st;
  st << unit->info_->statement;
  try {
    if (unit->info_->flag & 0x000001) {
      MCE_INFO(kOutTag << "\t" << unit->id_);
    } else {
      MCE_INFO(kOutTagNoRestore << "\t" << unit->id_);
    }
    //    QueryRunner(unit->info_->instance, unit->info_->wrflag,
    //        unit->info_->pattern).store(st);
    usleep(7000);//db平均为7毫秒
    //    sleep(10000);
  } catch (Ice::Exception& e) {
    MCE_WARN("MyQueue::Execute --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("MyQueue::Execute --> error:" << e.what());
  }
}

bool MyQueue::Check() {
  const static unsigned kMaxLen = 1000000;
  return (q_.size() < kMaxLen);
}

int MyQueue::Size() {
  return q_.size();
}

void QueueManager::PushUnit(const UnitPtr& unit) {
  while (Size() > size_sql_) {
    usleep(500);//TODO
  }
  MCE_INFO("QueueManager::PushUnit Size:" << Size() << " id:" << unit->id_ );
  ++push_cnt_;
  qs_.at((cur_index_++) % qs_.size())->PushUnit(unit);
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
