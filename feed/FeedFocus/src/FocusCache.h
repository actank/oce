/*
 * FocusCache.h
 *
 *  Created on: May 23, 2010
 *      Author: yejingwei
 */

#ifndef __FEED_FEEDCACHE_H__
#define __FEED_FEEDCACHE_H__

#include<climits>
#include <set>
#include <bitset>
#include <iostream>
#include <bitset>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include "Singleton.h"
#include "TaskManager.h"
#include "RFeed.h"

namespace xce {
namespace feed {

using namespace MyUtil;
using namespace std;
using namespace boost::multi_index;

const size_t kFocusSize = 1000000;
const size_t kBitSetSize = 4UL * 1024 * 1024 * 1024;

//-------------------------------------------------------------------------------------
//保存uid_用户所有的关注好友
class FocusConfig : public IceUtil::Shared{
public:
  int source_;
  FocusConfig() : maxfeed_(0), unreadcount_(0), lastAccesstime_(0){}
  void Add(const vector<int> & targets){
    targets_.insert(targets.begin(),targets.end());
  }
  void Add(int target){
    targets_.insert(target);
  }
  void Del(const vector<int> & targets){
    for(size_t i = 0 ; i < targets.size(); ++i){
      targets_.erase(targets.at(i));
    }
  }
  void Del(int target){
    targets_.erase(target);
  }
  set<int> targets(){
    return targets_;
  }
  size_t Size() {
    return targets_.size();
  }
  bool Empty(){
    return targets_.empty();
  }
  set<int> targets_;
  // NewAdd
  long maxfeed_;
  int unreadcount_;
  time_t lastAccesstime_;
  
private:

};

typedef IceUtil::Handle<FocusConfig> FocusConfigPtr;

//-------------------------------------------------------------------------------------
class PrintTask;

class FocusCache :public Singleton<FocusCache>{
  friend class PrintTask;
public:
  void Init();

  set<int> GetTargets(int source);

  int AddTargets(int source,int target);
  void DelTargets(int source,int target);
  // New Add
  bool IsGetIncCount(int user);
  int GetUnreadCount(int user); //取出未读数量
  void SetUnreadCount(int user, int count = 0);
  void UpdateMaxFeed(int user, FeedItemSeqSeq& itemseqseq);
  int GetIncCount(int user, FeedItemSeqSeq& itemseqseq);
private:
//  vector<int> UpdateCache(int source, const vector<int> & targets,
//      bool add_del);
  bool GetDB(int source, vector<int> & targets);
  void AddDB(int source, int target);
  void DelDB(int source, int target);
  void Kick();

  typedef boost::multi_index_container<
  FocusConfigPtr,
      indexed_by<
        sequenced<> ,
        ordered_unique<
              BOOST_MULTI_INDEX_MEMBER(FocusConfig, int,source_)
        >
      >
  > FocusConfigContainer;
  typedef FocusConfigContainer::nth_index<0>::type SeqIndex;
  typedef FocusConfigContainer::nth_index<1>::type UidIndex;
  typedef FocusConfigContainer::iterator iterator;

  FocusConfigContainer container_;
  bitset<kBitSetSize> bitset_;
  IceUtil::Mutex mutex_;
};

//-------------------------------------------------------------------------------------

class PrintTask: public Timer {
public:
  PrintTask() :
    Timer(30 * 1000) {
    ;
  }
  virtual void handle();
};


};
};

#endif
