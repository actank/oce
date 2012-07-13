/*
 *
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#ifndef FEEDCLASSI_H_
#define FEEDCLASSI_H_

#include "FeedClass.h"
#include "Singleton.h"
#include "TaskManager.h"

//#include "MyQueue.h"
using namespace std;
using namespace MyUtil;
namespace xce {
namespace feed {

class TimeCost{
public:
  TimeCost():buddy(-1.0f),loadmembers(-1.0f),mini(-1.0f),memc(-1.0f),build(-1.0f),total(-1.0f){}
  float buddy,loadmembers,mini,memc,build,total;
  string DumpToString();
};


class FeedClassI : public FeedClass, public MyUtil::Singleton<FeedClassI>{
public:
  virtual ::MyUtil::IntSeq GetMembers(::Ice::Int, const ::Ice::Current& = ::Ice::Current());
  virtual void AddMember(::Ice::Int, ::Ice::Int, const ::Ice::Current& = ::Ice::Current());
  virtual void DelMember(::Ice::Int, ::Ice::Int, const ::Ice::Current& = ::Ice::Current());
  virtual void DelGroup(int gid,const ::Ice::Current& = ::Ice::Current());
  virtual FeedDataSeq GetFeedDataOfFriendsInGroups(int user, const vector<int>& gids,int begin, int limit
      , const ::Ice::Current& = ::Ice::Current()) ;
private:
  map<int,vector<int> > GetBuddiesInGroups(int uid,const vector<int> & gids,TimeCost & cost);

};


}
}
#endif /* FEEDQUEUEI_H_ */
