/*
 * UserCache.h
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
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include "Singleton.h"
#include "TaskManager.h"
#include "RFeed.h"

namespace xce {
namespace feed {

using namespace MyUtil;
using namespace std;
using namespace boost::multi_index;


//-------------------------------------------------------------------------------------

class GroupInfo: public IceUtil::Shared{
public:
  GroupInfo(int gid,const set<int> & m):gid_(gid),members_(m){}
  set<int> members()const{
    return members_;
  }
  void Add(int m){
    members_.insert(m);
  }
//  void Add(const vector<int> & ms){
//    members_.insert(members.end(),ms.begin(),ms.end());
//  }
  void Del(int m){
    members_.erase(m);
  }
  int Size(){
    return members_.size();
  }
  int gid_;
private:
  set<int> members_;
};

typedef IceUtil::Handle<GroupInfo> GroupInfoPtr;

class GroupCache:public Singleton<GroupCache>{
public:
  const static int kSize = 10000000;
  vector<int> GetMembers(int gid);
  void AddMember(int mid,int gid);
  void DelMember(int mid,int gid);
  void DelGroup(int gid);
  string DumpToString();
private:
  void Kick();

  struct seqtag{};
  struct gidtag{};

  typedef boost::multi_index_container<
  GroupInfoPtr,
      indexed_by<
        sequenced<tag<seqtag> > ,
        hashed_unique<tag<gidtag>,BOOST_MULTI_INDEX_MEMBER(GroupInfo, int,gid_)>
      >
  > GroupInfoContainer;

  typedef GroupInfoContainer::index<seqtag>::type SeqIndex;
  typedef GroupInfoContainer::index<gidtag>::type GidIndex;

  GroupInfoContainer container_;
  IceUtil::Mutex mutex_;
};


class PrintTask: public Timer {
public:
  PrintTask() :
    Timer(5 * 1000) {
    ;
  }
  virtual void handle();
};


};
};

#endif
