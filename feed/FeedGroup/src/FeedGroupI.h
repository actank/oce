#ifndef FEEDITEMI_H_
#define FEEDITEMI_H_

#include <map>
#include <vector>
#include <string>

#include "Common.h"
#include "RFeed.h"
#include "GroupFeedCache.h"

using namespace std;
using namespace MyUtil;

namespace xce {
namespace feed {

class TimeRec{
public:
  TimeRec():load(0.0f),merge(0.0f),readall(0.0f),memc(0.0f),memcloader(0.0f),build(0.0f),total(0.0f){}
  float load,merge,readall,memc,memcloader,build,total;
  string DumpToString();
};

class FeedGroupManagerI: public FeedGroupManager, public MyUtil::Singleton<FeedGroupManagerI> {
public:
  virtual FeedDataSeq GetFeedData(int gid, int member_id, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataSeq GetFeedDataByType(int gid, int member_id, const MyUtil::IntSeq& types, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataSeq GetFeedDataByStype(int gid, int member_id, const MyUtil::IntSeq& stypes, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedItemSeq GetOriginalFeedData(int gid, const Ice::Current& = Ice::Current());
  virtual vector<FeedItemSeq> GetFeedItemSeqSeqByGroups(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());

  virtual void AddFeed(const FeedItem& item, const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight, const Ice::Current& = Ice::Current());
  virtual void RemoveFeed(int gid, int stype, Ice::Long merge, const Ice::Current& = Ice::Current());
  virtual void RemoveFeedById(int gid, Ice::Long feedId, const Ice::Current& = Ice::Current());
  virtual void RemoveAllFeed(int gid, const Ice::Current& = Ice::Current());
  virtual bool HasFeed(int gid, const Ice::Current& = Ice::Current());
  virtual void RefreshFeed(int gid, Ice::Long feed_id, int timestamp, const Ice::Current& = Ice::Current());
  virtual void ReplyFeed(int gid, Ice::Long feed_id, int timestamp, int owner, int replyer, const Ice::Current& = Ice::Current());

  virtual MyUtil::Int2IntMap GetGroupsUnreadCount(Ice::Int uid, const MyUtil::IntSeq & groups, const Ice::Current& = Ice::Current());
  virtual void AddMember(int group_id, int member_id, const Ice::Current& = Ice::Current());
  virtual void RemoveMember(int group_id, int member_id, const Ice::Current& = Ice::Current());

  virtual ::MyUtil::Int2IntSeqMap GetMembers(const ::MyUtil::IntSeq&, const ::Ice::Current& = ::Ice::Current());
  virtual void GetMembersTest(const ::MyUtil::IntSeq&, const ::Ice::Current& = ::Ice::Current());//接口有问题
//  virtual vector<int> GetMembersInGroup(int gid, const ::Ice::Current& = ::Ice::Current());



  virtual void Test(Ice::Int gid, Ice::Int mid, const Ice::Current&);

  virtual void ReadAllFeed(int gid,int uid,const Ice::Current& = Ice::Current());

  int CountUnread(int group_id, Ice::Long max_read_feed,int member_id);
protected:
  FeedDataSeq GetFeedDataImpl(int gid, int member_id, const IntSeq& types, bool stype, int begin, int limit);
  FeedDataPtr BuildAFeed(int gid, const vector<vector<GroupFeedItem> >& group, const FeedContentDict& fdict);
  FeedDataSeq BuildFeedContents(int gid, TimeRec& tr, vector<vector<vector<GroupFeedItem> > >& feeds);
  void CheckLoad(int gid, vector<Ice::Long> & fids, FeedContentDict & dict) ;
};


}
}

#endif // FEEDITEMI_H_

