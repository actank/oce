#include "GroupFeedCache.h"
#include "QueryRunner.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;

namespace xce {
namespace feed {

void GroupFeeds::RefreshFeedItem(Ice::Long feed_id, int timestamp) {
  IceUtil::Mutex::Lock lock(feeds_mutex_);
  list<GroupFeedItem>::iterator it = feeds_.begin();
  list<GroupFeedItem>::iterator new_pos = feeds_.end();
  for(; it != feeds_.end(); ++it) {
    if (new_pos == feeds_.end() && timestamp >= it->time_) {
      new_pos = it;
    }

    if (it->id_ == feed_id) {
      break;
    }
  }

  if (it != feeds_.end() && new_pos != feeds_.end()) {
    list<GroupFeedItem>::iterator nit = it;
    it->time_ = timestamp;
    if (it != new_pos) {
      feeds_.splice(new_pos, feeds_, it, ++nit);
    }
  }
}

void GroupFeeds::AddFeedItem(const FeedItem& item) {
  if (max_feed_ < item.feed) {
    max_feed_  = item.feed;
  }

  GroupFeedItem group_item(item);
  {
    IceUtil::Mutex::Lock lock(feeds_mutex_);
    feeds_.push_back(group_item);
  }
  if (time_oldest_ > item.time) {
    time_oldest_ = item.time;
  }
}

void GroupFeeds::AddNewFeedItem(const FeedItem& item) {
  // 还是作一下检查好，不直接push_front
  if (max_feed_ < item.feed) {
    max_feed_  = item.feed;
  }

  IceUtil::Mutex::Lock lock(feeds_mutex_);
  // feeds_.push_front(GroupFeedItem(item));
  list<GroupFeedItem>::iterator it = feeds_.begin();
  for(; it != feeds_.end(); ++it) {
    if (item.time >= it->time_) {
      break;
    }
  }
  feeds_.insert(it, GroupFeedItem(item));
}

// 要删除所有可以merge的项
vector<GroupFeedItem> GroupFeeds::Remove(int stype, Ice::Long merge) {
  vector<GroupFeedItem> res;
  IceUtil::Mutex::Lock lock(feeds_mutex_);
  list<GroupFeedItem>::iterator it = feeds_.begin();
  while(it != feeds_.end()) {
    if (it->SmallType() == stype && it->merge_ == merge) {
      res.push_back(*it);
      it = feeds_.erase(it);
    } else {
      it++;
    }
  }
  return res;
}

bool GroupFeeds::CountUnread(Ice::Long max_read_feed, int * unread_count,int member_id) const {
  *unread_count = 0;
  IceUtil::Mutex::Lock lock(feeds_mutex_);

  if (feeds_.back().id_ > max_read_feed) {
    return false;
  }
  list<GroupFeedItem>::const_iterator it = feeds_.begin();
  for(; it != feeds_.end(); ++it) {
    if (it->id_ <= max_read_feed) {
      break;
    }
    if(it->actor_ != member_id){
      ++(*unread_count);
    }
  }
  return true;
}

// merge, 把内存中的所有feed item合并一遍
void GroupFeeds::MergeFeed(vector<vector<GroupFeedItem*> >& merged_feeds, multimap<Ice::Long, int>& order_by_time) const {
  vector<int> update_times;
  IceUtil::Mutex::Lock lock(feeds_mutex_);

  // SmallType() + merge_ 是key, merged_feeds vector index 是 value
  map<pair<int, Ice::Long>, int> merge_index;
  list<GroupFeedItem>::const_iterator it = feeds_.begin();
  for(; it != feeds_.end(); ++it) {
    GroupFeedItem* group_item = const_cast<GroupFeedItem*>(&(*it));
    map<pair<int, Ice::Long>, int>::iterator it = merge_index.find(make_pair(group_item->SmallType(), group_item->merge_));
    if (it == merge_index.end()) {
      merged_feeds.push_back(vector<GroupFeedItem*>());
      merged_feeds.back().push_back(group_item);

      update_times.push_back(group_item->time_);
      merge_index[make_pair(group_item->SmallType(), group_item->merge_)] = merged_feeds.size() - 1;
    } else {
      if(group_item->MiniMergeType() == FeedMergeReplace 
          || (group_item->MiniMergeType() == FeedMergeAppend && merged_feeds[it->second].size() >= 5)){
         continue;
      }

      merged_feeds[it->second].push_back(group_item);
      // 不覆盖，因为后来的一定比先前的老
      // update_times[it->second] = group_item->time_;
    }
  }
  for(size_t i = 0; i < update_times.size(); ++i) {
    // key 是时间, value 是 index
    order_by_time.insert(make_pair<Ice::Long, int>(update_times.at(i), i));
  }
}

vector<GroupFeedItem> GroupFeeds::RemoveById(Ice::Long feed_id, bool by_merge) {
  Ice::Long merge_id = 0;
  int stype = 0;
  {
    IceUtil::Mutex::Lock lock(feeds_mutex_);

    for(list<GroupFeedItem>::iterator it = feeds_.begin(); it != feeds_.end(); ++it) {
      if (it->id_ == feed_id) {
        if (!by_merge) {
          vector<GroupFeedItem> seq;
          seq.push_back(*it);
          feeds_.erase(it);
          return seq;
        }
        merge_id = it->merge_;
        stype = it->SmallType();
        break;
      }
    }
  }
  return Remove(stype, merge_id); 
}

void GroupFeeds::RemoveAll(){
  IceUtil::Mutex::Lock lock(feeds_mutex_);
  list<GroupFeedItem>().swap(feeds_);
}

void GroupFeeds::EvictOversize() {
  IceUtil::Mutex::Lock lock(feeds_mutex_);
  int count = feeds_.size() - kLoadFeedCount;
  if (count <= 0) {
    return;
  }

  for(int i = 0; i < count; ++i) {
    feeds_.pop_back();
  }
  all_loaded_ = false;
}

vector<FeedItem> GroupFeeds::GetAllItems() {
  vector<FeedItem> items;
  IceUtil::Mutex::Lock lock(feeds_mutex_);
  for(list<GroupFeedItem>::iterator it = feeds_.begin(); it != feeds_.end(); ++it) {
    FeedItem item;
    MCE_DEBUG("GetAllItems feed=" << it->id_ << " time=" << it->time_);
    item.feed = it->id_;
    item.actor = it->actor_;
    item.merge = it->merge_;
    item.time = it->time_;
    item.type = it->type_;
    items.push_back(item);
  }
  return items;
}

// 返回的是合并后limit条, 不是合并前
vector<FeedItem> GroupFeeds::GetLatestItems(int limit) {
  vector<vector<GroupFeedItem*> > merged_feeds;
  multimap<Ice::Long, int> order_by_time;
  MergeFeed(merged_feeds, order_by_time);

  vector<FeedItem> feed_items;
  int count = 0;
  for(multimap<Ice::Long, int>::reverse_iterator it = order_by_time.rbegin(); it != order_by_time.rend(); ++it){
    if (++count > limit) {
      break;
    }
    for(unsigned i = 0; i < merged_feeds.at(it->second).size(); i++) {
      GroupFeedItem* group_item = merged_feeds.at(it->second).at(i);
      FeedItem item;
      item.feed = group_item->id_;
      item.actor = group_item->actor_;
      item.merge = group_item->merge_;
      item.time = group_item->time_;
      item.type = group_item->type_;
      feed_items.push_back(item);
    }
  }
  return feed_items;
}
//
//vector<vector<vector<GroupFeedItem> > >  GroupFeeds::GetFeed(const TypeFilter & type_filter, int begin, int limit) {
//  vector<vector<GroupFeedItem*> > merged_feeds;
//  multimap<Ice::Long, int> order_by_time;
//  MergeFeed(merged_feeds, order_by_time);
//
//  vector<vector<vector<GroupFeedItem> > > res;
//  MCE_INFO("GetFeed gid=" << group_id_ << " size=" << merged_feeds.size() << " begin=" << begin);
//  int count = 0;
//  for(multimap<Ice::Long, int>::reverse_iterator it = order_by_time.rbegin(); it != order_by_time.rend(); ++it) {
//    if (!type_filter.IsValid(merged_feeds.at(it->second).front()))
//      continue;
//    if (count++ < begin) {
//      continue;
//    }
//    if (count > begin + limit) {
//      break;
//    }
//    res.push_back(vector<vector<GroupFeedItem> >());
//    res.back().push_back(vector<GroupFeedItem>());
//    for(unsigned i = 0; i < merged_feeds.at(it->second).size(); ++i) {
//      res.back().back().push_back(*merged_feeds.at(it->second).at(i));
//    }
//  }
//  return res;
//}
vector<vector<vector<GroupFeedItem> > >  GroupFeeds::GetFeed(const TypeCheckerPtr & checker, int begin, int limit) {
  vector<vector<GroupFeedItem*> > merged_feeds;
  multimap<Ice::Long, int> order_by_time;
  MergeFeed(merged_feeds, order_by_time);

  vector<vector<vector<GroupFeedItem> > > res;
  int count = 0;
  for(multimap<Ice::Long, int>::reverse_iterator it = order_by_time.rbegin(); it != order_by_time.rend(); ++it) {
    if (!checker->Check(merged_feeds.at(it->second).front()))
      continue;
    if (count++ < begin) {
      continue;
    }
    if (count > begin + limit) {
      break;
    }
    res.push_back(vector<vector<GroupFeedItem> >());
    res.back().push_back(vector<GroupFeedItem>());
    for(unsigned i = 0; i < merged_feeds.at(it->second).size(); ++i) {
      res.back().back().push_back(*merged_feeds.at(it->second).at(i));
    }
  }
  MCE_INFO("GetFeed gid=" << group_id_ << " size=" << merged_feeds.size() << " begin=" << begin << " size:" << res.size());
  return res;
}
int GroupFeedCache::PoolTotalSize() {
  int count = 0;
  for(int i=0; i<POOL_SIZE; i++){
    IceUtil::Mutex::Lock lock(mutexs_[i]);
    count += pool_[i].size(); 
  }
  return count;
}

GroupFeedsPtr GroupFeedCache::Find(int gid, bool relocate) {
  int index = GetPoolIndex(gid);
  IceUtil::Mutex::Lock lock(mutexs_[index]);

  UidIndex& uid_index = pool_[index].get<1> ();
  UidIndex::iterator it = uid_index.find(gid);
  if (it != uid_index.end()) {
    if (relocate) {
      // 将 gid 放到LRU最新的位置
      SeqIndex& seq_index = pool_[index].get<0>();
      seq_index.relocate(seq_index.begin(), pool_[index].project<0>(it));
    }
    return *it;
  }
  return NULL;
}

void GroupFeedCache::Remove(int group_id) {
  int index = GetPoolIndex(group_id);
  IceUtil::Mutex::Lock lock(mutexs_[index]);

  UidIndex& uid_index = pool_[index].get<1>();
  UidIndex::iterator it = uid_index.find(group_id);
  if (it != uid_index.end()) {
    uid_index.erase(it);    
    MCE_INFO("remove success. gid=" << group_id);
  } else {
    MCE_INFO("remove not found. gid=" << group_id);
  }
}

void GroupFeedCache::Add(const GroupFeedsPtr & group_feeds) {
  int index = GetPoolIndex(group_feeds->group_id());
  IceUtil::Mutex::Lock lock(mutexs_[index]);

  pair<SeqIndex::iterator, bool> p = pool_[index].push_front(group_feeds);
  MCE_INFO("add gid=" << group_feeds->group_id());
  if (!p.second) {
    // 按照 LRU 调整位置
    pool_[index].relocate(pool_[index].begin(), p.first);
  } else if (pool_[index].size() > MAX_USER_COUNT) {
    int gid = pool_[index].back()->group_id();
    pool_[index].pop_back(); // LRU evict
    MCE_INFO("evict gid " << gid << " after add gid " << group_feeds->group_id());
  }
}

bool GroupFeedCache::Load(int gid, int max_time, int count) {
  string table_name = GetFeedTableName(gid);
  try {
    Statement sql;
    sql << "SELECT feed,time,mini_merge,type ,actor FROM "<< table_name << " WHERE group_id=" << gid << " AND time<FROM_UNIXTIME(" << max_time
        <<  ") ORDER BY time DESC LIMIT " << count;
    mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer, table_name).store(sql);
    if (!res) {
      MCE_INFO("select feed null res. gid:" << gid);
      return false;
    }
    if (res.num_rows() == 0) {
      MCE_INFO("no more feed. gid:" << gid);
    }

    MCE_INFO("load feed gid:" << gid << " max_time:" << max_time << " count:" << count << " size:" << res.num_rows());

    GroupFeedsPtr group_feeds = Find(gid);
    if (!group_feeds) {
      Add(new GroupFeeds(gid));
    }
    group_feeds = Find(gid);
    if (!group_feeds) {
      return false;
    }
//    for(int i = res.num_rows() - 1; i >= 0; --i) {
    for(unsigned i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      FeedItem item;
      item.feed = row["feed"];
      item.merge = row["mini_merge"];
      item.time = (time_t)mysqlpp::DateTime(row["time"]);
      item.type = row["type"];
      item.actor = row["actor"];
      group_feeds->AddFeedItem(item);
    }

    if (res.num_rows() < (unsigned)count) {
      group_feeds->set_all_loaded(true);
      MCE_INFO("no adequate feed. gid:" << gid << " count:" << res.num_rows() << '/' << count);
    }

    if (res.num_rows() > kLoadFeedCount) {
      MCE_DEBUG("insert into evict map. gid=" << gid);
      InsertEvictMap(gid, time(NULL));
    }
  } catch (std::exception& e) {
    MCE_WARN("load feed gid:" << gid << " count" << count <<  " error:" << e.what());
    return false;
  }

  return true;
}

void GroupFeedCache::InsertEvictMap(int user, int timestamp) {
  IceUtil::Mutex::Lock lock(evict_mutex_);
  evict_map_[user] = timestamp;
}

void GroupFeedCache::Evict() {
  map<int, int> evict_map;
  {
    IceUtil::Mutex::Lock lock(evict_mutex_);
    evict_map.swap(evict_map_);
  }
  int count = 0;
  for(map<int, int>::iterator it = evict_map.begin(); it != evict_map.end(); ++it){
    if (time(NULL) - it->second > 5* 60) {
      GroupFeedsPtr feeds = GroupFeedCache::instance().Find(it->first);
      if (feeds) {
        int oldsize = feeds->Size();
        feeds->EvictOversize(); // 超过一分钟则逐出
        MCE_INFO("GroupFeedCache::Evict. groupid:" << feeds->group_id() << " oldsize:" << oldsize << " newsize:" << feeds->Size());
        ++count;
      }
    } else {
      InsertEvictMap(it->first, it->second);
    }
  }
  MCE_INFO("Evict large : " << count << " of " << evict_map.size() << " evicted.");
}

}
}

