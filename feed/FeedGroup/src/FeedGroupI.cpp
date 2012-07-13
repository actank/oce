#include "FeedGroupI.h"

#include "GroupUnreadCount.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "FeedContentAdapter.h"
#include "FeedMemcLoaderAdapter.h"
#include "WTransitAdapter.h"

using namespace com::xiaonei::xce;
using namespace MyUtil;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  using namespace xce::feed;
  service.getAdapter()->add(&FeedGroupManagerI::instance(),
      service.createIdentity("M", ""));
  MyUtil::TaskManager::instance().scheduleRepeated(new EvictTimer());
  MyUtil::TaskManager::instance().schedule(new WriteGroupUnreadTimer());
}

namespace xce {
namespace feed {

void FeedGroupManagerI::CheckLoad(int gid, vector<Ice::Long> & fids,
    FeedContentDict & dict) {
  vector<Ice::Long> miss_ids;
  for (unsigned i = 0; i != fids.size(); ++i) {
    if (dict.find(fids.at(i)) == dict.end()) {
      miss_ids.push_back(fids.at(i));
    }
  }
  if (miss_ids.empty()) {
    return;
  }
  //if (!dict.empty()) {
  if (false) {
    try {
      FeedMemcLoaderAdapter::instance().LoadFeedSeq(miss_ids);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedGroupManagerI::CheckLoad LoadFeedSeq --> load gid:" << gid << " size:" << miss_ids.size() << " err, " << e);
      return;
    }
    MCE_INFO("FeedGroupManagerI::CheckLoad LoadFeedSeq --> load " << gid << " size:" << miss_ids.size());
  } else {
    FeedContentDict dict_loader;
    try {
      dict_loader
          = FeedMemcLoaderAdapter::instance().GetFeedDict(miss_ids, 100);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedGroupManagerI::CheckLoad GetFeedDict --> load gid:" << gid << " size:" << miss_ids.size() << " err, " << e);
      return;
    }
    MCE_INFO("FeedGroupManagerI::CheckLoad GetFeedDict --> load " << gid << " size:" << miss_ids.size() << " res:" << dict_loader.size());
    dict.insert(dict_loader.begin(), dict_loader.end());
  }
}

int FeedGroupManagerI::CountUnread(int group_id, Ice::Long max_read_feed,
    int member_id) {
  MCE_DEBUG("FeedGroupManagerI::CountUnread : gid=" << group_id << " max_read_feed=" << max_read_feed);
  if (max_read_feed <= 0) {
    return 0;
  }
  GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(group_id, false);
  int count = 0;
  if (group_feeds && group_feeds->CountUnread(max_read_feed, &count, member_id)) {
    return count;
  }

  string table_name = GetFeedTableName(group_id);
  Statement sql;
  sql << "SELECT feed FROM " << table_name << " WHERE group_id=" << group_id
      << " AND feed>" << max_read_feed << " AND actor !=" << member_id
      << " LIMIT 105";
  mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer,
      table_name).store(sql);
  if (res) {
    return res.num_rows();
  }
  return 0;
}

MyUtil::Int2IntMap FeedGroupManagerI::GetGroupsUnreadCount(Ice::Int uid,
    const MyUtil::IntSeq & groups, const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::GetGroupsUnreadCount uid=" << uid << " groups_size=" << groups.size());
  return UnreadInfoPool::instance().GetUnreadCount(uid, groups);
}

void FeedGroupManagerI::AddMember(int group_id, int member_id,
    const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::AddMember group_id=" << group_id << " member_id=" << member_id);
  UnreadInfoPool::instance().AddMember(group_id, member_id);
}

void FeedGroupManagerI::RemoveMember(int group_id, int member_id,
    const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::RemoveMember group_id=" << group_id << " member_id=" << member_id);
  UnreadInfoPool::instance().RemoveMember(group_id, member_id);
}

Int2IntSeqMap FeedGroupManagerI::GetMembers(const ::MyUtil::IntSeq& gids,
    const ::Ice::Current&) {
  Int2IntSeqMap res;
  for (unsigned i = 0; i != gids.size(); ++i) {
    res[gids.at(i)] = UnreadInfoPool::instance().GetMembersInGroup(gids.at(i));
  }
  MCE_INFO("FeedGroupManagerI::GetMembers gids size:" << gids.size() << " res size:" << res.size() );
  return res;
}

void FeedGroupManagerI::GetMembersTest(const ::MyUtil::IntSeq& group_ids,
    const ::Ice::Current&) {
  Int2IntSeqMap res1 = UnreadInfoPool::instance().GetMembers(group_ids);
  Int2IntSeqMap res;

  MCE_INFO("FeedGroupManagerI::GetMembers group_ids size:" << group_ids.size() << " res size:" << res.size() );
  return;
}

//vector<int> FeedGroupManagerI::GetMembersInGroup(int gid,const ::Ice::Current&){
//  vector<int> res = UnreadInfoPool::instance().GetMembersInGroup(gid);
//  MCE_INFO("FeedGroupManagerI::GetMembersInGroup gid:" << gid << " res size:" << res.size() );
//   return res;
//}

void FeedGroupManagerI::AddFeed(const FeedItem& item,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight,
    const Ice::Current&) {

  MCE_INFO("FeedGroupManagerI::AddFeed feed:" << item.feed << " actor:" << item.actor << " stype:" << (item.type&0xffff)

      << " mergetype:" << (((unsigned int)item.type) >> 30) << " mergeid:" << item.merge << " time:" << item.time
      << " uptime:" << config->updateTime);

  Int2IntMap::const_iterator it = id2weight.begin();
  for (; it != id2weight.end(); ++it) {
    GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(it->first);
    MCE_INFO("addFeed gid=" << it->first << " feed=" << item.feed);
    if (group_feeds) {
      group_feeds->AddNewFeedItem(item);
    }
    UnreadInfoPool::instance().AddFeed(it->first, item);
  }
}

void FeedGroupManagerI::RemoveFeed(int gid, int stype, Ice::Long merge,
    const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::RemoveFeed gid:" << gid << " stype:" << stype << " merge:" << merge);
  GroupFeedsPtr feeds = GroupFeedCache::instance().Find(gid);
  if (feeds) {
    vector<GroupFeedItem> removed_items = feeds->Remove(stype, merge);
    for (size_t i = 0; i < removed_items.size(); ++i) {
      UnreadInfoPool::instance().RemoveFeed(gid, removed_items[i].id_);
    }
  }

  Statement sql;
  sql << "DELETE FROM " << GetFeedTableName(gid) << " WHERE group_id=" << gid
      << " AND small_type=" << stype << " AND mini_merge=" << merge;
  mysqlpp::StoreQueryResult dbres = QueryRunner(FeedDBInstance, CDbWServer,
      GetFeedTableName(gid)).store(sql);
  if (!dbres) {
    MCE_WARN("FeedGroupManagerI::RemoveFeed db err, gid:" << gid << " stype:" << stype << " merge:" << merge);
  }
}

void FeedGroupManagerI::RemoveFeedById(int gid, Ice::Long feed_id,
    const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::RemoveFeedById gid=" << gid << " feedid=" << feed_id);
  vector<GroupFeedItem> removed_items;
  GroupFeedsPtr feeds = GroupFeedCache::instance().Find(gid);
  if (feeds) {
    removed_items = feeds->RemoveById(feed_id);
    for (size_t i = 0; i < removed_items.size(); ++i) {
      UnreadInfoPool::instance().RemoveFeed(gid, removed_items[i].id_);
    }
  } else {
    UnreadInfoPool::instance().RemoveFeed(gid, feed_id);
  }

  mysqlpp::StoreQueryResult dbres;
  bool need_del = false;
  int stype = 0;
  int merge = 0;
  if (!removed_items.empty()) {
    stype = removed_items.front().SmallType();
    merge = removed_items.front().merge_;
    need_del = true;
  } else {
    Statement sql;
    sql << "SELECT small_type, mini_merge FROM " << GetFeedTableName(gid)
        << " WHERE group_id=" << gid << " AND feed=" << feed_id << " LIMIT 1";
    dbres
        = QueryRunner(FeedDBInstance, CDbRServer, GetFeedTableName(gid)).store(
            sql);
    if (dbres && dbres.num_rows() > 0) {
      need_del = true;
      mysqlpp::Row& row = dbres.at(0);
      stype = (int) row["small_type"];
      merge = (Ice::Long) row["mini_merge"];
    }
  }

  MCE_INFO("FeedGroupManagerI::RemoveFeedById gid:" << gid << " feedid:" << feed_id << " size:" << removed_items.size());

  if (!need_del) {
    MCE_INFO("FeedGroupManagerI::RemoveFeedById need not del db, gid:" << gid << " feed:" << feed_id);
    return;
  }

  Statement del_sql;
  del_sql << "DELETE FROM " << GetFeedTableName(gid) << " WHERE group_id="
      << gid << " AND small_type=" << stype << " AND mini_merge=" << merge;
  dbres = QueryRunner(FeedDBInstance, CDbWServer, GetFeedTableName(gid)).store(
      del_sql);
  if (!dbres) {
    MCE_WARN("RemoveFeedById db err, gid:" << gid << " feedid:" << feed_id);
  }
}

void FeedGroupManagerI::RemoveAllFeed(int gid, const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::RemoveAllFeed gid:" << gid);
  GroupFeedsPtr feeds = GroupFeedCache::instance().Find(gid);
  if (feeds) {
    feeds->RemoveAll();
  }
  UnreadInfoPool::instance().RemoveAllFeed(gid);

  Statement sql;
  sql << "DELETE FROM " << GetFeedTableName(gid) << " WHERE group_id=" << gid;

  mysqlpp::SimpleResult res = QueryRunner(FeedDBInstance, CDbWServer,
      GetFeedTableName(gid)).execute(sql);
  if (!res) {
    MCE_WARN("RemoveAllFeed db err. gid=" << gid);
  }
}

bool FeedGroupManagerI::HasFeed(int gid, const Ice::Current&) {
  GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(gid);
  if (group_feeds) {
    return group_feeds->Size() > 0;
  }

  Statement sql;
  sql << "SELECT feed FROM " << GetFeedTableName(gid) << " WHERE group_id="
      << gid << " LIMIT 1";
  mysqlpp::StoreQueryResult res = QueryRunner(FeedDBInstance, CDbRServer,
      GetFeedTableName(gid)).store(sql);
  return res && (res.num_rows() > 0);
}

void FeedGroupManagerI::RefreshFeed(int gid, Ice::Long feed_id, int timestamp,
    const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::RefreshFeed. gid:" << gid << " fid:" << feed_id << " should not invoke");

  GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(gid);
  if (group_feeds) {
    group_feeds->RefreshFeedItem(feed_id, timestamp);
  }


//  vector<int> gids;
//  gids.push_back(gid);
//  ::MyUtil::Int2IntSeqMap mem_dict = GetMembers(gids);
//  ::MyUtil::Int2IntSeqMap::iterator mi = mem_dict.find(gid);
//  if (mi == mem_dict.end()) {
//    return;
//  }
//  vector<int> &targets = mi->second;
//  stringstream message;
//  message << "{\"icon\":\"\",\"content\":\"\",\"minigroup\":" << gid
//      << ",\"nid\":0,\"source\":\"10001-1\"}";
//  try {
//    talk::adapter::WTransitAdapter::instance().deliverNotifyWebpager(targets,
//        message.str());
//    MCE_INFO("FeedGroupManagerI::RefreshFeed  fid:" << feed_id << " gid:" << gid << " targets size:" <<targets.size() );
//  } catch (Ice::Exception& e) {
//    MCE_WARN("FeedGroupManagerI::RefreshFeed  fid:" << feed_id << " gid:" << gid << " targets:" <<targets.size()  << " call WTransist err:" << e);
//  }
}

void FeedGroupManagerI::ReplyFeed(int gid,
		Ice::Long feed_id, int timestamp, int owner, int replier, const Ice::Current&){
//  Int2IntMap::const_iterator it = id2weight.begin();

  GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(gid);
//	MCE_INFO("FeedGroupManagerI::RefreshFeed. addFeed gid=" << it->first << " feed=" << item.feed);
	UnreadInfoPool::instance().ReplyFeed(gid, feed_id,timestamp,owner,replier);
}

FeedItemSeq FeedGroupManagerI::GetOriginalFeedData(int gid, const Ice::Current&) {
  GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(gid);
  if (group_feeds) {
    return group_feeds->GetAllItems();
  }
  return FeedItemSeq();
}

vector<FeedItemSeq> FeedGroupManagerI::GetFeedItemSeqSeqByGroups(
    const MyUtil::IntSeq& ids, const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::GetFeedItemSeqSeqByGroups. ids size:" << ids.size());
  const IntSeq types;
  FeedItemSeqSeq itemseqseq;
  for (vector<int>::const_iterator vit = ids.begin(); vit != ids.end(); ++vit) {
    int gid = (*vit);
    GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(gid);
    if (!group_feeds) {
      GroupFeedCache::instance().Load(gid, time(NULL), kLoadFeedCount);
      group_feeds = GroupFeedCache::instance().Find(gid);
    }
    if (group_feeds) {
      itemseqseq.push_back(group_feeds->GetLatestItems(10));
    }
  }
  MCE_INFO("GetFeedItemSeqSeqByGroups ids:" << ids.size() << " res:" << itemseqseq.size());
  return itemseqseq;
}

FeedDataSeq FeedGroupManagerI::GetFeedData(int gid, int member_id, int begin,
    int limit, const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::GetFeedData. gid:" << gid << " mid:" << member_id << " begin:" << begin << " limit:" << limit);
  return GetFeedDataImpl(gid, member_id, IntSeq(), true, begin, limit);
}

FeedDataSeq FeedGroupManagerI::GetFeedDataByStype(int gid, int member_id,
    const MyUtil::IntSeq& stypes, int begin, int limit, const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::GetFeedDataByStype. gid:" << gid << " mid:" << member_id << " stypes size:" << stypes.size() << " begin:" << begin << " limit:" << limit);
  return GetFeedDataImpl(gid, member_id, stypes, true, begin, limit);
}

FeedDataSeq FeedGroupManagerI::GetFeedDataByType(int gid, int member_id,
    const MyUtil::IntSeq& types, int begin, int limit, const Ice::Current&) {
  MCE_INFO("FeedGroupManagerI::GetFeedDataByType. gid:" << gid << " mid:" << member_id << " type size:" << types.size()<< " begin:" <<begin << " limit:" << limit);
  return GetFeedDataImpl(gid, member_id, types, false, begin, limit);
}

FeedDataSeq FeedGroupManagerI::GetFeedDataImpl(int gid, int member_id,
    const IntSeq& types, bool by_stype, int begin, int limit) {
  TimeStat ttotal;
  TimeRec tr;
  int oldest_time = time(NULL);
  vector<vector<vector<GroupFeedItem> > > live_feeds;
  TypeCheckerPtr checker = TypeCheckerFactory::Create(types, by_stype);

  GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(gid);
  if (group_feeds) {
    TimeStat tm;
    live_feeds = group_feeds->GetFeed(checker, begin, limit);
    oldest_time = group_feeds->GetOldestTime();
    tr.merge = tm.getTime();
  }

  if ((unsigned) limit > live_feeds.size() && oldest_time > 0 && !(group_feeds
      && group_feeds->all_loaded())) {
    MCE_INFO("GetFeedDataImpl need load gid " << gid << " mergesize=" << live_feeds.size());
    int absent = limit - live_feeds.size();
    int load_limit = (absent / kLoadFeedCount + 1) * kLoadFeedCount; // 按kLoadFeedCount向上取模
    TimeStat t1;
    GroupFeedCache::instance().Load(gid, oldest_time, load_limit); // 加载比这个时间更老的 live feed
    tr.load = t1.getTime();
    t1.reset();
    group_feeds = GroupFeedCache::instance().Find(gid);
    if (group_feeds) {
      live_feeds = group_feeds->GetFeed(checker, begin, limit);
    }
    tr.merge += t1.getTime();
  }

  TimeStat tread;
  if (group_feeds) {
    MCE_INFO("GetFeedDataImpl read all feed. gid=" << gid << " member=" << member_id);
    //    UnreadInfoPool::instance().ReadAllFeed(gid, member_id, group_feeds->max_feed());
    TaskManager::instance().execute(new ReadAllFeedTask(gid, member_id,
        group_feeds->max_feed()));
  } else {
    MCE_INFO("GetFeedDataImpl no read all feed. gid=" << gid << " member=" << member_id);
  }
  tr.readall = tread.getTime();

  FeedDataSeq feeds;
  feeds = BuildFeedContents(gid, tr, live_feeds);
  tr.total = ttotal.getTime();
  MCE_INFO("GetFeedDataImpl gid:" << gid << " begin:" << begin << " limit:" << limit << " mergesize:" << live_feeds.size()
      << " size:" << live_feeds.size() << " res:" << feeds.size() << " types:" << types.size() << " by_stype:" << by_stype << " #cost# " << tr.DumpToString());
  return feeds;
}

FeedDataSeq FeedGroupManagerI::BuildFeedContents(int gid, TimeRec& tr, vector<
    vector<vector<GroupFeedItem> > >& feeds) {
  TimeStat t1;
  LongSeq feed_ids;
  for (size_t i = 0; i < feeds.size(); ++i) {
    for (size_t j = 0; j < feeds.at(i).size(); ++j) {
      for (size_t k = 0; k < feeds.at(i).at(j).size(); ++k) {
        feed_ids.push_back(feeds.at(i).at(j).at(k).id_);
      }
    }
  }

  FeedContentDict fdict;
  try {
    fdict = FeedContentAdapter::instance().getFeedDict(feed_ids);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedContent.getFeedDict err:" << e);
    return FeedDataSeq();
  }
  MCE_INFO("FeedContent.getFeedDict gid:" << gid << " id size:" << feed_ids.size() << " res size:" << fdict.size());
  tr.memc = t1.getTime();
  t1.reset();
  if (fdict.size() < feed_ids.size()) {
    CheckLoad(gid, feed_ids, fdict);
    tr.memcloader = t1.getTime();
    t1.reset();
  }

  FeedDataSeq seq;
  for (size_t i = 0; i < feeds.size(); ++i) {
    FeedDataPtr feed = BuildAFeed(gid, feeds.at(i), fdict);
    if (feed) {
      seq.push_back(feed);
    }
  }
  tr.build = t1.getTime();
  return seq;
}

FeedDataPtr FeedGroupManagerI::BuildAFeed(int gid, const vector<vector<
    GroupFeedItem> >& group, const FeedContentDict& fdict) {
  FeedDataPtr data;
  FeedReplyPtr reply;
  ostringstream xml;
  vector<Ice::Long> valid_feedids;
  int stype = 0;
  int gather_count = 0;
  for (size_t i = 0; i < group.size(); ++i) {
    bool getid = false;
    int merge_count = 0;
    int merge_type = 0;
    for (size_t k = 0; k < group.at(i).size(); ++k) {
      const GroupFeedItem & item = group.at(i).at(k);
      int stype = item.SmallType();
      int actor = item.actor_;
      Ice::Long merge = item.merge_;
      FeedContentDict::const_iterator dict_it = fdict.find(item.id_);
      merge_type = item.NewsMergeType();
      if (dict_it != fdict.end()) {
        const FeedContentPtr content = dict_it->second;
        if (content) {
          stype = content->data->type & 0xFFFF;
          if (gather_count == 0 && !data) {
            data = content->data;
            reply = content->reply;
          }
          if (gather_count == 0 && data && ((merge_count == 0 && merge_type
              == FeedMergeReplace) || (merge_count < 5 && merge_type
              == FeedMergeAppend))) {
            xml << content->data->xml;
            ++merge_count;
          }
          if (gather_count > 0 && !getid) {
            valid_feedids.push_back(item.id_);
            getid = true;
          }
        } else {
          // 如果 feed id 对应的内容已经为空，则删除相应的索引
          GroupFeedsPtr feeds = GroupFeedCache::instance().Find(gid);
          if (feeds) {
            //feeds->RemoveById(item.id_, false);
            MCE_INFO("remove gid:" << gid << " feedid:" << item.id_ << " without merge. stype:" << stype << " actor:" << actor << " merge:" << merge);
          }
        }
      }
      if (merge_type == FeedMergeReplace) {
        break;
      } else if (merge_type == FeedMergeAppend) {
        if (k >= 5) {
          break;
        }
      }
    }
    if (data) {
      ++gather_count;
    }
  }
  if (reply) {
    xml << reply->oldReply << reply->newReply << "<reply_count>"
        << reply->count << "</reply_count>";
  }

  if (valid_feedids.size() > 0) {
    string name = "merge_feed";
    xml << "<" << name << "><count>" << valid_feedids.size() << "</count>";
    for (size_t j = 0; j < valid_feedids.size(); j++) {
      xml << "<fid>" << valid_feedids.at(j) << "</fid>";
    }
    xml << "</" << name << ">";
  }
  if (data) {
    data->xml = xml.str();
  }
  return data;
}

void FeedGroupManagerI::Test(int gid, int mid, const Ice::Current&) {
  GetFeedData(gid, mid, 10, 10);
}

void FeedGroupManagerI::ReadAllFeed(int gid, int uid, const Ice::Current&) {
  GroupFeedsPtr group_feeds = GroupFeedCache::instance().Find(gid);
  if (group_feeds) {
    MCE_INFO("FeedGroupManagerI::SetReadAll. gid=" << gid << " uid=" << uid);
    UnreadInfoPool::instance().ReadAllFeed(gid, uid, group_feeds->max_feed());
  } else {
    MCE_INFO("FeedGroupManagerI::SetReadAll. gid=" << gid << " uid=" << uid);
  }
}

string TimeRec::DumpToString() {
  ostringstream oss;
  oss << " load:" << load << " merge:" << merge << " readall:" << readall
      << " memc:" << memc << " memcloader:" << memcloader << " build:" << build
      << " total:" << total;
  return oss.str();
}
;
}
}

