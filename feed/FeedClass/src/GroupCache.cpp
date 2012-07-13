/*
 * UserCache.h
 *
 *  Created on: May 23, 2010
 *      Author: yejingwei
 */

#include "GroupCache.h"
#include <exception>
#include <iostream>
#include <ostream>
#include <vector>
#include <QueryRunner.h>
#include <Ice/Exception.h>
#include <stdexcept>
#include "LogWrapper.h"
#include "DbHelper.h"


using namespace std;
using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace boost::multi_index;
using namespace boost;

//---------------------------------------------------------------------------
vector<int> GroupCache::GetMembers(int gid) {
  {
    IceUtil::Mutex::Lock lock(mutex_);
    GidIndex & index = container_.get<gidtag> ();
    GidIndex::iterator git = index.find(gid);
    if (git != index.end()) {
      SeqIndex::iterator sit = container_.project<seqtag> (git);//新查的放到前面
      container_.relocate(container_.get<seqtag> ().begin(), sit);
      set<int> mids = (*git)->members();
      return vector<int> (mids.begin(), mids.end());
    }
  }

  vector<int> members;
  if(!GroupDb::instance().GetMembers(gid, members)){
    MCE_WARN("GroupCache::GetMembers. load db fail, gid:" << gid );
    return vector<int>();
  }
  GroupInfoPtr g = new GroupInfo(gid,set<int>(members.begin(),members.end()));

  {
    IceUtil::Mutex::Lock lock(mutex_);
    SeqIndex & sindex = container_.get<seqtag> ();
    pair<SeqIndex::iterator, bool> p = container_.push_front(g);
    if (!p.second) {
//      (*p.first)->Add(uinfo->classes());
      sindex.relocate(sindex.begin(), p.first);
    } else {
      Kick();
    }
  }
  return members;
}

void GroupCache::AddMember(int mid, int gid) {
  {
    IceUtil::Mutex::Lock lock(mutex_);
    GidIndex & index = container_.get<gidtag> ();
    GidIndex::iterator git = index.find(gid);
    if (git != index.end()) {
      (*git)->Add(mid);
      SeqIndex::iterator sit = container_.project<seqtag> (git);
      container_.relocate(container_.get<seqtag> ().begin(), sit);
      MCE_INFO("GroupCache::AddMember. group in mem. mid:" << mid << " gid:" << gid << " size:" <<(*git)->Size());
    }else{
      MCE_INFO("GroupCache::AddMember. group not in mem. mid:" << mid << " gid:" << gid);
    }
  }
}

void GroupCache::DelMember(int mid, int gid) {
  {
    IceUtil::Mutex::Lock lock(mutex_);
    GidIndex & index = container_.get<gidtag> ();
    GidIndex::iterator git = index.find(gid);
    if (git != index.end()) {
      (*git)->Del(mid);
      SeqIndex::iterator sit = container_.project<seqtag> (git);
      container_.relocate(container_.get<seqtag> ().begin(), sit);
    }
  }
}

void GroupCache::DelGroup(int gid) {
  {
    IceUtil::Mutex::Lock lock(mutex_);
    GidIndex & index = container_.get<gidtag> ();
    index.erase(gid);
  }
}

string GroupCache::DumpToString(){
  ostringstream oss;
  oss << " GroupCache size:" << container_.size();
  return oss.str();
}

void GroupCache::Kick(){
  while (container_.size() > (unsigned)kSize) { //在外层加锁
    container_.pop_back();
  }
}
//---------------------------------------------------------------------------

void PrintTask::handle() {
  MCE_INFO("##STAT## " << GroupCache::instance().DumpToString());
}
