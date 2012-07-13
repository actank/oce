/*
 * FeedQueueI.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "FeedClassI.h"
#include "LogWrapper.h"
#include "QueryRunner.h"
#include "ServiceI.h"
#include "GroupCache.h"
#include "Util.h"
#include "FeedMiniAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
#include "FeedGroupAdapter.h"

using namespace xce::feed;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy::adapter;
void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedClassI::instance(), service.createIdentity(
      "M", ""));
  TaskManager::instance().scheduleRepeated(new PrintTask());

}

MyUtil::IntSeq FeedClassI::GetMembers(::Ice::Int uid, const ::Ice::Current&) {
  vector<int> gids = GroupCache::instance().GetMembers(uid);
  MCE_INFO("FeedClassI::GetMembers. uid:" << uid << " memberids size:" << gids.size() );
  return gids;
}

void FeedClassI::AddMember(::Ice::Int uid, ::Ice::Int gid,
    const ::Ice::Current&) {
  MCE_INFO("FeedClassI::AddMember. uid:" << uid << " gid:" << gid );
  GroupCache::instance().AddMember(uid, gid);
}

void FeedClassI::DelMember(::Ice::Int uid, ::Ice::Int gid,
    const ::Ice::Current&) {
  MCE_INFO("FeedClassI::DelMember. uid:" << uid << " gid:" << gid);
  GroupCache::instance().DelMember(uid, gid);
}

void FeedClassI::DelGroup(int gid,const ::Ice::Current&) {
  MCE_INFO("FeedClassI::DelGroup. gid:" << gid );
  GroupCache::instance().DelGroup(gid);
}

FeedDataSeq FeedClassI::GetFeedDataOfFriendsInGroups(int uid, const vector<int>& gids,
    int begin, int limit, const ::Ice::Current&) {
  TimeStat ttotal;
  TimeCost cost;
  MCE_INFO("FeedClassI::GetFeedDataOfFriendsInGroups. uid:" << uid << " gids size:" << gids.size() << " begin:" << begin
      << " limit:" << limit);
  if (gids.empty()) {
    return FeedDataSeq();
  }

  //群中的好友
  map<int, vector<int> > bdy_map;
  bdy_map = GetBuddiesInGroups(uid, gids,cost);

  //群中好友的mini
  TimeStat t1;
  vector<int> buddies = Map2Vector(bdy_map);
  FeedItemSeqSeq itemseqseq =
      FeedMiniAdapter::instance().GetFeedItemSeqSeqByUsers(buddies);
  cost.mini = t1.getTime();

  //将item排序
  multimap<int, FeedItemSeq> time2data;
  MakeTime2Data(itemseqseq, time2data);
  itemseqseq.clear();
  vector<long> feedids;
  Rank(time2data, begin,limit,itemseqseq, feedids);

  //取FeedContent
  FeedContentDict dict;
  if (!GetFeedDict(feedids,dict)) {
    MCE_INFO("FeedClassI::GetFeedDataOfFriendsInGroups. get Content fail. uid:" << uid << " gids size:" << gids.size() << " begin:" << begin
        << " limit:" << limit);
    return FeedDataSeq();
  }
  cost.memc = t1.getTime();
  t1.reset();

  //拼XML
  FeedDataSeq result = BuildFeedData(itemseqseq, dict);
  cost.build = t1.getTime();

  cost.total = ttotal.getTime();
  MCE_INFO("FeedClassI::GetFeedDataOfFriendsInGroups. uid:" << uid << " gids size:" << gids.size() << " begin:" << begin
      << " limit:" << limit << " result size:" << result.size() << " ##cost## " << cost.DumpToString());
  return result;
}

map<int, vector<int> > FeedClassI::GetBuddiesInGroups(int uid,
    const vector<int> & gids,TimeCost & cost) {
  vector<int> buddies;
  try {
    TimeStat ts;
    buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc(uid, 5000);
    cost.buddy = ts.getTime();
  } catch (...) {
    MCE_INFO("FeedClassI::GetBuddiesInGroups. error. uid:" << uid << " gid size:" <<gids.size());
    return map<int, vector<int> > ();
  }
  if (buddies.empty()) {
    return map<int, vector<int> > ();
  }

  set<int> buddy_set(buddies.begin(),buddies.end());
  TimeStat ts;
  map<int, vector<int> > res;
  map<int,vector<int> > gid2members;
  try {
    TimeStat ts;
    gid2members = FeedGroupAdapter::instance().GetMembersInGroups(gids);
    cost.loadmembers = ts.getTime();
  } catch (...) {
    MCE_INFO("FeedClassI::GetBuddiesInGroups. error. uid:" << uid << " gid size:" <<gids.size());
    return map<int, vector<int> > ();
  }


  for(map<int,vector<int> >::iterator i =  gid2members.begin(); i != gid2members.end();++i){
    MCE_INFO("FeedClassI::GetBuddiesInGroups. gid:" << i->first << " members:" << Print(i->second));
    vector<int> v = Intersect(buddy_set, i->second);
    res[i->first] = v;
  }

//  for (unsigned i = 0; i != gids.size(); ++i) {
//    vector<int> m = GroupCache::instance().GetMembers(gids.at(i));
//    if (!m.empty()) {
//      vector<int> v = Intersect(buddy_set, m);//TODO
//      if (!v.empty()) {
//        res[gids.at(i)] = v;
//      }
//    }
//  }
//  cost.loadmembers = ts.getTime();
  return res;
}


string TimeCost::DumpToString(){
  ostringstream oss;
  oss << " buddy:" << buddy << " loadmembers:" << loadmembers
      << " mini:" << mini << " memc:" << memc << " build:" << build << " total:" << total ;
  return oss.str();
}
