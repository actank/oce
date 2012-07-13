#include "GroupUnreadCount.h"
#include "FeedGroupI.h"
#include "QueryRunner.h"
#include "WTransitAdapter.h"

using namespace com::xiaonei::xce;

namespace xce {
namespace feed {

template<typename T>
static string Print(const T & container) { //将容器内容打印成字符串
  ostringstream ios;
  ios << "(";
  bool first_item = true;
  int i = 0;
  for (typename T::const_iterator itr = container.begin(); itr
      != container.end(); ++itr, ++i) {
    if (first_item == true) {
      first_item = false;
    } else {
      ios << ",";
    }
    ios << *itr;
  }
  ios << ")";
  return ios.str();
}

static std::string GetUnreadTableName(int id) {
  std::ostringstream os;
  os << "feed_group_max_read_" << id % kDBClusterSize;
  return os.str();
}

void GroupUnreadInfo::AddFeed(const FeedItem& item) {
  Ice::Long feed_id = item.feed;
  int actor = item.actor;
  Ice::IntSeq targets;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    map<int, UnreadCount>::iterator it = member_unread_map_.begin();
    for (; it != member_unread_map_.end(); ++it) {
      if (it->second.max_read_feed_ <= 0 || it->second.max_read_feed_ < feed_id) {
        targets.push_back(it->first);
        ++it->second.count_;
      }
      if (it->second.max_read_feed_ <= 0 && actor != it->first) {
        it->second.max_read_feed_ = feed_id - 1;
        it->second.changed_ = true;
      }
//      MCE_DEBUG("group " << group_id_ << " member=" << it->first << " max_read_feed_="
//          << it->second.max_read_feed_ << " feed="<< feed_id - 1);
    }
  }
  stringstream message;
  //message << "{\"icon\":\"\",\"content\":\"\",\"minigroup\":" << group_id_ << ",\"nid\":0,\"source\":\"10000-1\"}";
  message << "{\"icon\":\"\",\"content\":\"\",\"minigroup\":" << group_id_
      << ",\"feed\":" << feed_id << ",\"actor\":" << actor
      << ",\"nid\":0,\"source\":\"10000-1\"}";
  try {
    talk::adapter::WTransitAdapter::instance().deliverNotifyWebpager(targets,
        message.str());
    MCE_INFO("GroupUnreadInfo::AddFeed  fid:" << feed_id << " actor:" << actor << " targets:" <<Print(targets) );
    if (actor == 220678672 || actor == 347348947) {
      MCE_INFO("GroupUnreadInfo::AddFeed.actor:" << actor << " message: " << message.str() );
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("WTransitAdapter.deliverNotifyWebpager err:" << e);
  }
}

void GroupUnreadInfo::RemoveFeed(Ice::Long feed_id) {
  IceUtil::Mutex::Lock lock(mutex_);
  map<int, UnreadCount>::iterator it = member_unread_map_.begin();
  for (; it != member_unread_map_.end(); ++it) {
    if (it->second.max_read_feed_ < feed_id) {
      if (it->second.count_ > 0) {
        --it->second.count_;
      }
    }
  }
}

void GroupUnreadInfo::RemoveAllFeed() {
  IceUtil::Mutex::Lock lock(mutex_);
  map<int, UnreadCount>::iterator it = member_unread_map_.begin();
  for (; it != member_unread_map_.end(); ++it) {
    it->second.count_ = 0; // >= 0 就是有效的
    it->second.max_read_feed_ = -1; // -1 表示当前全部已读
    it->second.changed_ = true;
  }
}

void GroupUnreadInfo::ReadAllFeed(int member_id, Ice::Long max_feed) {
  IceUtil::Mutex::Lock lock(mutex_);
  map<int, UnreadCount>::iterator it = member_unread_map_.find(member_id);
  if (it != member_unread_map_.end()) {
    it->second.count_ = 0;
    it->second.max_read_feed_ = max_feed;
    it->second.reply_cnt_ = 0;
    it->second.changed_ = true;
  }
}

pair<int,int> GroupUnreadInfo::GetUnreadCount(int member_id) {
  long max_read_feed = 0;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    map<int, UnreadCount>::const_iterator it = member_unread_map_.find(
        member_id);
    if (it != member_unread_map_.end()) {
      max_read_feed = it->second.max_read_feed_;
      if (it->second.count_ >= 0) { //说明原来已经计算过
        return make_pair(it->second.count_ , it->second.reply_cnt_);
      }
    }
  }

  int count = FeedGroupManagerI::instance().CountUnread(group_id_,
      max_read_feed, member_id);
  {
    IceUtil::Mutex::Lock lock(mutex_);
    map<int, UnreadCount>::iterator it = member_unread_map_.find(member_id);
    if (it != member_unread_map_.end()) {
      it->second.count_ = count;
      return make_pair(it->second.count_ , it->second.reply_cnt_);
    }else{
      return make_pair(count , 0);
    }
  }
}

void GroupUnreadInfo::AddMember(int member_id, Ice::Long max_read_feed,int reply_cnt) {
  IceUtil::Mutex::Lock lock(mutex_);
  map<int, UnreadCount>::const_iterator it = member_unread_map_.find(member_id);
  if (it == member_unread_map_.end()) {
    // 延迟计算未读数
    member_unread_map_.insert(make_pair(member_id, UnreadCount(-1,
        max_read_feed,reply_cnt)));
  }
//  MCE_DEBUG("add member " << member_id << " group_size=" << member_unread_map_.size());
}

void GroupUnreadInfo::RemoveMember(int member_id) {
  IceUtil::Mutex::Lock lock(mutex_);
  member_unread_map_.erase(member_id);
  MCE_DEBUG("remove member " << member_id << " group_size=" << member_unread_map_.size());
}

vector<int> GroupUnreadInfo::GetMembers(int gid) {
  IceUtil::Mutex::Lock lock(mutex_);
  vector<int> res;
  for (map<int, UnreadCount>::iterator i = member_unread_map_.begin(); i
      != member_unread_map_.end(); ++i) {
    res.push_back(i->first);
  }
  return res;
}

void GroupUnreadInfo::WriteDB() {
  string table_name = GetUnreadTableName(group_id_);
  Statement sql_insert;
  ostringstream os_insert;
  sql_insert << "REPLACE INTO " << table_name
      << " (group_id, member_id, max_read_feed,reply_cnt) VALUES ";
  os_insert << "REPLACE INTO " << table_name
      << " (group_id, member_id, max_read_feed,reply_cnt) VALUES ";
  int insert_count = 0;

  Statement sql_delete;
  ostringstream os_delete;
  sql_delete << "DELETE FROM " << table_name << " WHERE group_id = "
      << group_id_ << " AND member_id IN (";
  os_delete << "DELETE FROM " << table_name << " WHERE group_id = "
      << group_id_ << " AND member_id IN (";
  int delete_count = 0;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    map<int, UnreadCount>::iterator it = member_unread_map_.begin();
    for (; it != member_unread_map_.end(); ++it) {
      if (it->second.changed_) {
        if (it->second.count_ < 0 && it->second.reply_cnt_ == 0) {
          if (delete_count > 0) {
            sql_delete << ',';
            os_delete << ',';
          }
          sql_delete << it->first;
          os_delete << it->first;
          ++delete_count;
        } else {
          if (insert_count > 0) {
            sql_insert << ',';
            os_insert << ',';
          }
          sql_insert << '(' << group_id_ << ',' << it->first << ','
              << it->second.max_read_feed_ << "," << it->second.reply_cnt_ <<')';
          os_insert << '(' << group_id_ << ',' << it->first << ','
              << it->second.max_read_feed_ << "," << it->second.reply_cnt_ <<')';
          ++insert_count;
        }
        it->second.changed_ = false;
      }
    }
  }
  try {
    if (insert_count > 0) {
    	if(group_id_ == 375585059){
    		MCE_INFO("GroupUnreadInfo::WriteDB. gid:" << group_id_<<" insert sql: " << os_insert.str());
    	}
      // sql_insert << " ON DUPLICATE KEY UPDATE max_read_feed=VALUES(max_read_feed)";
      mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbWServer,
          table_name).store(sql_insert);
      if (!res) {
        MCE_WARN("group " << group_id_ << " insert db table group_unread_count error");
      }
    }
    if (delete_count > 0) {
      sql_delete << ")";
      os_delete << ")";
    	if(group_id_ == 375585059){
    		MCE_INFO("GroupUnreadInfo::WriteDB. gid:" << group_id_<<" delete sql: " << os_delete.str());
    	}
      mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbWServer,
          table_name).store(sql_delete);
      if (!res) {
        MCE_WARN("group " << group_id_ << " delete db table group_unread_count error");
      }
    }
  } catch (std::exception& e) {
    MCE_WARN("group " << group_id_ << " write db table group_unread_count error" << e.what());
  }
}

void GroupUnreadInfo::ReplyFeed(Ice::Long feed_id, int timestamp, int owner, int replier){
//  int actor = owner;
  Ice::IntSeq targets;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    map<int, UnreadCount>::iterator it = member_unread_map_.begin();
    for (; it != member_unread_map_.end(); ++it) {
//      if (it->second.max_read_feed_ <= 0 && replier != it->first) {
      if (replier != it->first) {

      	it->second.reply_cnt_ += 1;
        it->second.changed_ = true;
      }
      MCE_DEBUG("GroupUnreadInfo::ReplyFeed gid: " << group_id_ << " member: " << it->first
      		<< " feed_id:" << feed_id << " owner:" << owner << " replier:" << replier
      		<< " replycnt:" << it->second.reply_cnt_ << " changed_:" << it->second.changed_);
    }
  }
}
string GetMinigroupTableName(int gid) {
  std::ostringstream os;
  os << "minigroup_member_" << gid % kDBClusterSize;
  return os.str();
}

bool UnreadInfoPool::LoadGroup(int gid) {
  // TODO : 数据一致性需要细化
  set<int> members;
  {
    string mini_table_name = GetMinigroupTableName(gid);
    Statement sql;
    sql << "SELECT member_id FROM " << mini_table_name
        << " WHERE minigroup_id=" << gid;
    mysqlpp::StoreQueryResult res = QueryRunner("minigroup", CDbRServer,
        mini_table_name).store(sql);
    if (!res) {
      MCE_WARN("select minigroup null res. gid:" << gid);
      return false;
    }
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
//      MCE_DEBUG("load group " << gid << " member " << row["member_id"]);
      members.insert(row["member_id"]);
    }
    MCE_INFO("UnreadInfoPool::LoadGroup.load unread count info. gid=" << gid);
  }

  GroupUnreadInfoPtr unread_info;
  string table_name = GetUnreadTableName(gid);
  try {
    Statement sql;
    sql << "SELECT member_id, max_read_feed,reply_cnt FROM " << table_name
        << " WHERE group_id=" << gid;

    mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer,
        table_name).store(sql);
    if (!res) {
      MCE_WARN("select group_unread_count null res. gid:" << gid);
      return false;
    }
    if (res.num_rows() == 0) {
      MCE_INFO("UnreadInfoPool::LoadGroup. no group_unread_count data. gid:" << gid);
      // return false;
    }
    MCE_INFO("UnreadInfoPool::LoadGroup.load group_unread_count gid:" << gid << " size:" << res.num_rows());

    unread_info = new GroupUnreadInfo(gid);
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int member_id = row["member_id"];
      if (members.find(member_id) != members.end()) {
//        MCE_DEBUG("group " << gid << " member " << member_id << " max_unread " << row["max_read_feed"]);
        unread_info->AddMember(member_id, row["max_read_feed"],row["reply_cnt"]);
        members.erase(member_id);//对于有最大已读id，或者有未读回复数
      } else {
        MCE_DEBUG("group " << gid << " not found member " << member_id << " max_unread " << row["max_read_feed"]);
        // TODO : 从DB删除失效数据
      }
    }

  } catch (std::exception& e) {
    MCE_WARN("UnreadInfoPool::LoadGroup.load group_unread_count gid:" << gid << " error:" << e.what());
    return false;
  }
  for (set<int>::const_iterator it = members.begin(); it != members.end(); ++it) {
//    MCE_DEBUG("group " << gid << " member " << *it << " add max_unread 0");
    unread_info->AddMember(*it, -1,0);//最大已读id和未读回复数默认值
  }

//  return true;//TODO
  {
    int pool_index = GetPoolIndex(gid);
    IceUtil::Mutex::Lock lock(unread_mutexs_[pool_index]);
    unread_maps_[pool_index][gid] = unread_info;
    MCE_INFO("UnreadInfoPool::LoadGroup. insert mem. gid:" << gid << " pool_index:" << pool_index);
  }
  return true;
}

void UnreadInfoPool::WriteDB() {
  int cnt = 0;
  for (int i = 0; i < 32; ++i) {
    GroupUnreadMap::iterator it = unread_maps_[i].begin();
    for (; it != unread_maps_[i].end(); ++it) {
      it->second->WriteDB();
//      MCE_INFO("UnreadInfoPool::WriteDB. gid:" << it->second->group_id());//TODO del
      ++cnt;
    }
  }
  MCE_INFO("UnreadInfoPool::WriteDB begin. count:" << cnt);
}

void UnreadInfoPool::ReplyFeed(int gid, Ice::Long feed_id, int timestamp, int owner, int replier){
  GroupUnreadInfoPtr group_unread_info = LocateGroup(gid);
  if (group_unread_info) {
    MCE_INFO("UnreadInfoPool::ReplyFeed. gid=" << gid << " feed: " << feed_id
    		<< " owner:" << owner << " replier:" << replier
    );
			group_unread_info->ReplyFeed(feed_id,timestamp,owner,replier);
  } else {
    MCE_INFO("UnreadInfoPool::ReplyFeed.can not find group gid=" << gid << " feed: " << feed_id
    		<< " owner:" << owner << " replier:" << replier
    );
  }
}

GroupUnreadInfoPtr UnreadInfoPool::LocateGroup(int gid) {
  GroupUnreadInfoPtr group = FindGroup(gid);
  if (group) {
    return group;
  }
  // TODO : return Ptr
  if (!LoadGroup(gid)) {
    MCE_WARN("locate group error. gid=" << gid);
  }

  return FindGroup(gid);
}

GroupUnreadInfoPtr UnreadInfoPool::FindGroup(int gid) {
  int pool_index = GetPoolIndex(gid);

  IceUtil::Mutex::Lock lock(unread_mutexs_[pool_index]);
  GroupUnreadMap::iterator it = unread_maps_[pool_index].find(gid);
  if (it != unread_maps_[pool_index].end()) {
    if (!it->second->is_stale()) {
      MCE_INFO("UnreadInfoPool::FindGroup. find suc. gid:" << gid);
      return it->second;
    } else {
      MCE_INFO("UnreadInfoPool::FindGroup. find fail. not stale. gid:" << gid);
    }
  }
  MCE_INFO("UnreadInfoPool::FindGroup. find fail. not in mem gid:" << gid
      << " pool_index:" << pool_index << " size:" << unread_maps_[pool_index].size());
  return 0;
}

void UnreadInfoPool::AddFeed(int gid, const FeedItem& item) {
  //  int actor = item.actor;
  Ice::Long feed_id = item.feed;
  GroupUnreadInfoPtr group_unread_info = LocateGroup(gid);
  if (group_unread_info) {
    MCE_INFO("unread info pool add feed gid=" << gid << " feed=" << feed_id);
    group_unread_info->AddFeed(item);
  } else {
    MCE_INFO("unread info pool can't find gid=" << gid << " on add feed=" << feed_id);
  }
}

void UnreadInfoPool::RemoveFeed(int gid, Ice::Long feed_id) {
  GroupUnreadInfoPtr group_unread_info = LocateGroup(gid);
  if (group_unread_info) {
    MCE_DEBUG("unread info remove feed gid=" << gid << " feed=" << feed_id);
    group_unread_info->RemoveFeed(feed_id);
  } else {
    MCE_DEBUG("null unread info remove feed gid=" << gid << " feed=" << feed_id);
  }
}

void UnreadInfoPool::RemoveAllFeed(int gid) {
  GroupUnreadInfoPtr unread_info = LocateGroup(gid);
  if (unread_info) {
    MCE_INFO("unread info remove all feed gid=" << gid);
    unread_info->RemoveAllFeed();
  } else {
    MCE_WARN("null unread info remove all feed gid=" << gid);
  }
}

void UnreadInfoPool::ReadAllFeed(int group_id, int member_id,
    Ice::Long max_feed) {
  GroupUnreadInfoPtr group_unread_info = LocateGroup(group_id);
  if (group_unread_info) {
    MCE_INFO("UnreadInfoPool::ReadAllFeed. gid=" << group_id << " member=" << member_id);
    group_unread_info->ReadAllFeed(member_id, max_feed);
  } else {
    MCE_WARN("UnreadInfoPool::ReadAllFeed.null unread info read all feed gid=" << group_id << " member=" << member_id);
  }
}

void UnreadInfoPool::AddMember(int group_id, int member_id) {
  GroupUnreadInfoPtr unread_info = LocateGroup(group_id);
  if (!unread_info) {
    unread_info = new GroupUnreadInfo(group_id);
    unread_maps_[GetPoolIndex(group_id)].insert(
        make_pair(group_id, unread_info));
  }
  if (unread_info) {
    unread_info->AddMember(member_id, -1,0);
  }
}

void UnreadInfoPool::RemoveMember(int group_id, int member_id) {
  GroupUnreadInfoPtr group_unread_info = LocateGroup(group_id);
  if (group_unread_info) {
    group_unread_info->RemoveMember(member_id);
  }
}

map<int, vector<int> > UnreadInfoPool::GetMembers(const vector<int> & gids) {
  map<int, vector<int> > res;
  int gid = gids.at(0);
  GroupUnreadInfoPtr group_unread_info = LocateGroup(gid);
  MCE_INFO("UnreadInfoPool::GetMembers. gid:" << gid);
  vector<int> ms;
  if (group_unread_info) {
    ms = group_unread_info->GetMembers(gid);
    MCE_INFO("UnreadInfoPool::GetMembers. Get suc. gid:" << gid << " member size:" << ms.size());
  } else {
    MCE_INFO("UnreadInfoPool::GetMembers. Get fail. gid:" << gid );
  }
  res[gid] = ms;
  //  for (int i = 0; i != gids.size(); ++i) {
  //    GroupUnreadInfoPtr group_unread_info = LocateGroup(gids.at(i));
  //    MCE_INFO("UnreadInfoPool::GetMembers. gid:" << gids.at(i));
  //    vector<int> ms;
  //    if (group_unread_info) {
  //      ms = group_unread_info->GetMembers(gids.at(i));
  //      MCE_INFO("UnreadInfoPool::GetMembers. Get suc. gid:" << gids.at(i) << " member size:" << ms.size());
  //    } else {
  //      MCE_INFO("UnreadInfoPool::GetMembers. Get fail. gid:" << gids.at(i) );
  //    }
  //    res[gids.at(i)] = ms;
  //  }
  MCE_INFO("UnreadInfoPool::GetMembers. res size:" << res.size());
  return res;
}

vector<int> UnreadInfoPool::GetMembersInGroup(int gid) {
  vector<int> mids;
  GroupUnreadInfoPtr group_unread_info = LocateGroup(gid);
  MCE_INFO("UnreadInfoPool::GetMembers. gid:" << gid);
  vector<int> ms;
  if (group_unread_info) {
    mids = group_unread_info->GetMembers(gid);
    MCE_INFO("UnreadInfoPool::GetMembers. Get GroupUnreadInfoPtr suc. gid:" << gid << " member size:" << mids.size());
  } else {
    MCE_INFO("UnreadInfoPool::GetMembers. Get GroupUnreadInfoPtr fail. gid:" << gid );
  }
  return mids;
}

MyUtil::Int2IntMap UnreadInfoPool::GetUnreadCount(int uid,
    const MyUtil::IntSeq & groups) {
  MyUtil::Int2IntMap res;
  for (size_t i = 0; i < groups.size(); ++i) {
    GroupUnreadInfoPtr group_unread_info = FindGroup(groups[i]);
    pair<int,int> cnts;

//    int count = 0;

    bool blocking = true; // 是否阻塞等待加载
    if (!group_unread_info) {
      if (blocking) {
        LoadGroup(groups[i]);
        group_unread_info = FindGroup(groups[i]);
      } else {
        MyUtil::TaskManager::instance().execute(new LoadGroupUnreadTask(
            groups[i]));
      }
    }

    if (group_unread_info) {
    	cnts = group_unread_info->GetUnreadCount(uid);
//      MCE_INFO("user " << uid << " group " << groups[i] << " unread_count="
//      		<< cnts.first << " reply_cnts:" << cnts.second);
    }
    int all = cnts.first + cnts.second;
//    if (all > 0) {
     res.insert(make_pair(groups[i], all));
//    }
  }
  return res;
}

void LoadGroupUnreadTask::handle() {
  MCE_DEBUG("LoadGroupUnreadTask handled. gid=" << group_id_);
  UnreadInfoPool::instance().LoadGroup(group_id_);
}

void WriteGroupUnreadTimer::handle() {

  UnreadInfoPool::instance().WriteDB();
  MyUtil::TaskManager::instance().schedule(new WriteGroupUnreadTimer());
}

void ReadAllFeedTask::handle() {
  UnreadInfoPool::instance().ReadAllFeed(group_id_, member_id_, max_feed_);
}

}
}

