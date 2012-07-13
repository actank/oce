/*
 * FeedFocusInvertI.h
 *
 *  Created on: May 21, 2010
 *      Author: yejingwei
 */
#ifndef __FEED_FOCUS_INVERT_H__
#define __FEED_FOCUS_INVERT_H__
//#include "RFeed.h"
#include "FeedFocusInvert.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "LogWrapper.h"
#include <IceUtil/Thread.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Mutex.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <bitset>
namespace xce {
namespace feed {

using namespace MyUtil;
using namespace std;
using namespace boost::multi_index;


//-------------------------------------------------------------------------------------
//保存uid_用户所有的关注好友
class TargetInfo : public IceUtil::Shared{
private:
  int target_;//被关注人的uid
  set<int> sources_;

public:
  TargetInfo(int target):target_(target){}
  template<typename T>
  void Add(const T & sources){
    sources_.insert(sources.begin(),sources.end());
  }
  void Add(int target){
    sources_.insert(target);
  }
  void Del(const vector<int> & targets){
    for(size_t i = 0 ; i < targets.size(); ++i){
      sources_.erase(targets.at(i));
    }
  }
  void Del(int target){
    sources_.erase(target);
  }
  int GetTarget()const{ return target_;}
  set<int> GetSources()const{
  	//TODO
    return sources_;
  }
  size_t Size()const {
    return sources_.size();
  }
  bool Empty()const{
    return sources_.empty();
  }
};

typedef IceUtil::Handle<TargetInfo> TargetInfoPtr;
//-------------------------------------------------------------------------------------

class FeedFocusInvertI: public FeedFocusInvert, public Singleton<FeedFocusInvertI> {
public:
  virtual void AddFocus(::Ice::Int source, int target, const ::Ice::Current& = ::Ice::Current());
  virtual void DelFocus(::Ice::Int source, int target, const ::Ice::Current& = ::Ice::Current());
  virtual MyUtil::IntSeq GetSources(int target, const Ice::Current& = Ice::Current());
  virtual void KickTarget(int target,const ::Ice::Current& = ::Ice::Current());

private:
  typedef multi_index_container<
  TargetInfoPtr,
      indexed_by<
        sequenced<> ,
				hashed_unique<BOOST_MULTI_INDEX_CONST_MEM_FUN(TargetInfo,int,GetTarget) >
      >
  > TargetInfoContainer;
  typedef TargetInfoContainer::nth_index<0>::type SeqIndex;
  typedef TargetInfoContainer::nth_index<1>::type UidIndex;
  typedef TargetInfoContainer::iterator iterator;

  const static size_t kFocusSize = 200000;
  TargetInfoContainer container_;
  IceUtil::Mutex mutex_;
public:
  void Dump();
private:
  void AddCache(int target,TargetInfoPtr & uinfo);
  //bool true表示用户有设置，false表示用户没设置
  TargetInfoPtr Find(int target, bool relocate = true);
  bool GetDB(int source, TargetInfoPtr & uinfo);
  void Kick();
};
class FeedFocusInvertStatTimer : public Timer, public Singleton<FeedFocusInvertStatTimer>{
public:
	FeedFocusInvertStatTimer() : Timer(30*1000, 150){	}
	void handle(){
		FeedFocusInvertI::instance().Dump();
	}
};

};
};
#endif
