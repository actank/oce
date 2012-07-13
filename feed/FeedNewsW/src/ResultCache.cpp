/*
 * ResultCache.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: yejingwei
 */

#include "ResultCache.h"
#include "IceLogger.h"

using namespace xce::feed;


//-----------------------------------------------------------------------------------

ResultPtr ResultPool::Get(int uid) {
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex & index = mi_.get<uidtag> ();
  UidIndex::iterator uit = index.find(uid);
  if (uit == index.end()) {
    return 0;
  } else {
    return *uit;
  }
}

void ResultPool::Add(const ResultPtr & r) {
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex & index = mi_.get<uidtag> ();
  UidIndex::iterator uit = index.find(r->uid_);
  if (uit == index.end()) {
    index.insert(r);
    MCE_INFO("ResultPool::Add,not exist uid:"<< r->uid_ << ",size:"<< index.size());
  } else {
    //const_cast<ResultPtr >(*uit) = const_cast<ResultPtr & >(r);//TODO ??
    index.erase(uit);
    index.insert(r);
    MCE_INFO("ResultPool::Add,exist uid:"<< r->uid_ << ",size:"<< index.size());
  }
}

void ResultPool::Del(int uid) {
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex & index = mi_.get<uidtag> ();
  index.erase(uid);
}

void ResultPool::Scan() {
  MyUtil::Date date = MyUtil::Date(time(NULL));
  IceUtil::Int64 bound = date.toSeconds() - kValidTimeInSecond;
//  MCE_DEBUG("ResultPool::Scan,bound:"<< bound << ",now time:" << bound + kValidTimeInSecond);
  {
    IceUtil::Mutex::Lock lock(mutex_);
    TimeIndex & index = mi_.get<timetag> ();
    TimeIndex::iterator i = index.begin();
    while( i != index.end() && (*i)->stamp_ < bound ) {
      index.erase(i++);
    }
  }
}

int ResultPool::Size() {
  IceUtil::Mutex::Lock lock(mutex_);
//  UidIndex & index = mi_.get<uidtag> ();
  return mi_.size();
}

//-----------------------------------------------------------------------------------

void ResultCache::Init() {
  gets_ = hits_ = 0;
  for(size_t i = 0; i != kPool; ++i){
    pools_.push_back(new ResultPool());
  }

  TaskManager::instance().scheduleRepeated(new ScanTask);
  MCE_INFO("ResultCache::Init done, pools_ size:" << pools_.size() );
}

ResultPtr ResultCache::Get(int uid, int begin, int limit) {
  if(begin != 0 || limit != 20){//现在只取0到20
    return 0;
  }

  ++gets_;
  ResultPtr res = pools_.at(GetIndex(uid))->Get(uid);
  if (res == 0) {
    MCE_INFO("ResultCache::Get,no in cache,uid:"<< uid);
  }else{
    MCE_INFO("ResultCache::Get,in cache,uid:"<< uid);
    ++hits_;
  }
  return res;
}

void ResultCache::Add(const ResultPtr & r) {
  int index = GetIndex(r->uid_);
  MCE_INFO("ResultCache::Add,uid:"<< r->uid_ << ",index:"<< index );
  pools_.at(index)->Add(r);
}

int ResultCache::GetIndex(int uid) {
  return (uid / 100) % kPool;
}

void ResultCache::Del(int uid){
  int index = GetIndex(uid);
  MCE_INFO("ResultCache::Del,uid:"<< uid << ",index:"<< index );
  pools_.at(index)->Del(uid);
}

void ResultCache::Scan(){
  int total = 0;
  for(size_t i = 0 ;i != pools_.size();++i){
    pools_.at(i)->Scan();
    total += pools_.at(i)->Size();
  }
  double rate = (double)hits_ / (double)gets_;
  MCE_INFO("ResultCache::Scan,total:" << total <<",gets:"<< gets_ << ",hits:"<< hits_ << ",rate:" << rate);
  gets_ = hits_ = 0;
}

//-----------------------------------------------------------------------------------

void ScanTask::handle(){
  ResultCache::instance().Scan();
}
