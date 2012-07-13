#ifndef _GROUP_UNREAD_COUNT_
#define _GROUP_UNREAD_COUNT_

#include <map>
#include <IceUtil/IceUtil.h>

#include "Common.h"
#include "RFeed.h"
#include "TaskManager.h"

namespace xce {
namespace feed {

struct UnreadCount {
  UnreadCount(int count, Ice::Long max_read,int reply_cnt) : count_(count), changed_(true), max_read_feed_(max_read),reply_cnt_(reply_cnt) {}
  int count_;
  bool changed_; //是否已经将改动写入DB

  // Ice::Long min_counted_feed_;
  Ice::Long max_read_feed_;
  int reply_cnt_;
};

class GroupUnreadInfo : public IceUtil::Shared {
public:
  GroupUnreadInfo(int gid) : group_id_(gid), load_time_(time(NULL)) {}
  pair<int,int> GetUnreadCount(int member_id);

  void AddFeed(const FeedItem& item);
  void RemoveFeed(Ice::Long feed_id);
  void ReadAllFeed(int member_id, Ice::Long max_feed);
  void RemoveAllFeed();

  void AddMember(int member_id, Ice::Long max_read_feed,int reply_cnt);
  void RemoveMember(int member_id);
  vector<int> GetMembers(int gid);

  void Reload();
  void WriteDB();

  int group_id() const { return group_id_; }
  bool is_stale() {
    return time(NULL) - load_time_ > 60 * 60;
  }

  void ReplyFeed(Ice::Long feed_id, int timestamp, int owner, int replyer);
private:
  int group_id_;
  time_t load_time_;
  mutable IceUtil::Mutex  mutex_;
  map<int, UnreadCount> member_unread_map_;
};
typedef IceUtil::Handle<GroupUnreadInfo> GroupUnreadInfoPtr;

// 全存, 不逐出
class UnreadInfoPool : public MyUtil::Singleton<UnreadInfoPool> {
public:
  void AddFeed(int gid, const FeedItem& item);
  void RemoveFeed(int gid, Ice::Long feed_id);
  void RemoveAllFeed(int group_id);
  void ReadAllFeed(int group_id, int member_id, Ice::Long max_feed);

  MyUtil::Int2IntMap GetUnreadCount(int uid, const MyUtil::IntSeq & groups);
  void AddMember(int group_id, int member_id);
  void RemoveMember(int group_id, int member_id);
  map<int, vector<int> > GetMembers(const vector<int> & gids);
  vector<int> GetMembersInGroup(int gid);

  bool LoadGroup(int gid);
  void WriteDB();
  void ReplyFeed(int gid, Ice::Long feed_id, int timestamp, int owner, int replyer);

private:
  int CountUnread(int group_id, Ice::Long max_read_feed);

  typedef map<int, GroupUnreadInfoPtr> GroupUnreadMap;
  GroupUnreadInfoPtr FindGroup(int gid);
  GroupUnreadInfoPtr LocateGroup(int gid);

  int GetPoolIndex(int group_id) const {
    return (group_id / kUserClusterSize) % 32;
  }
  GroupUnreadMap unread_maps_[32];
  IceUtil::Mutex  unread_mutexs_[32];
};

class LoadGroupUnreadTask : public MyUtil::Task {
public:
  LoadGroupUnreadTask(int group_id) : group_id_(group_id) {}
  virtual void handle();
private:
  int group_id_;
};

class WriteGroupUnreadTimer : public MyUtil::Timer {
public:
  WriteGroupUnreadTimer() : Timer(60 * 1000) {}
  virtual void handle();
};

class ReadAllFeedTask : public MyUtil::Task {
public:
  ReadAllFeedTask(int group_id, int member_id, Ice::Long max_feed) : group_id_(group_id),member_id_(member_id) ,max_feed_(max_feed) {}
  virtual void handle();
private:
  int group_id_;
  int member_id_;
  long max_feed_;
};

}
}

#endif // _GROUP_UNREAD_COUNT_
