/*
 * FeedQueueI.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "FeedAsyncI.h"
#include "LogWrapper.h"
#include "QueryRunner.h"
#include "ServiceI.h"
#include "SqlQueue.h"
//#include "Restore.h"
#include "Common.h"

using namespace xce::feed;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;

const string DB_INSTANCE = "feed_db";
const string GROUP_DB_INSTANCE = "feed_minigroup";
const string TAB_ID_SEQ = "feed_id_seq";
#define COL_FEED "feed"
#define COL_SOURCE "source"
#define COL_SMALLTYPE "small_type"
#define COL_PSOURCE "psource"
#define COL_PTYPE "ptype"
#define COL_ACTOR "actor"
#define COL_NEWS_MERGE "news_merge"
#define COL_MINI_MERGE "mini_merge"
#define COL_TIME "time"

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedAsyncI::instance(), service.createIdentity(
      "M", ""));
  FeedAsyncI::instance().Init();
  IdGenerator::instance().Init();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
//  int index = props->getPropertyAsIntWithDefault("Service.ServerIndex", 0);
//  Restorer::instance().SetServiceIndex(index);
  //  TaskManager::instance().execute(new RestoreTask("", "",1));//启动后回复上个小时和这个小时的
//  TaskManager::instance().scheduleRepeated(&QueueStatTimer::instance());
  SqlConverter::instance().Init();
}
void FeedAsyncI::Init() {
  qm_ = new QueueManager();
  qm_->Init(5, 10000000);
}

bool FeedAsyncI::AddFeedIndex(const FeedIndexPtr & idx,
    const Ice::Current&) {
  if (!qm_->Check()) {
    return false;
  }
  if (!idx) {
    return true;
  }
//  TimeStat ts;
  Statement sql;
  sql << "INSERT INTO " << getIndexTab(idx->feed)
      << " (feed,source,psource,small_type,big_type,ptype,actor,time,mini_merge,news_merge)"
      << " VALUES (" << idx->feed << "," << idx->source << "," << idx->psource
      << ", " << idx->stype << "," << int(idx->stype / 100) * 100 << ","
      << idx->ptype << "," << idx->actor << ",from_unixtime(" << idx->time
      << ")," << idx->miniMerge << "," << idx->newsMerge << ")";

  //  QueryRunner(DB_INSTANCE, CDbWServer, getIndexTab(idx->feed)).store(sql);
  string str = SqlConverter::instance().Convert(sql);
  qm_->Push(-1,0,DB_INSTANCE, CDbWServer, getIndexTab(idx->feed), sql, true);
  return true;
}

bool FeedAsyncI::RemoveFeedIndex(const MyUtil::LongSeq& feeds,
    const Ice::Current&) {
  if (!qm_->Check()) {
    return false;
  }
  MCE_INFO("FeedAsyncI::RemoveFeedIndex --> feeds size = " << feeds.size());
  map<int, MyUtil::LongSeq> tmp;
  for (size_t i = 0; i < feeds.size(); ++i) {
    tmp[feeds.at(i) % 10].push_back(feeds.at(i));
  }
//  IndexLoaderPtr loader = new IndexLoader();//TODO ???
  for (map<int, MyUtil::LongSeq>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    Statement sql;
    sql << "delete from " << getIndexTab(it->first) << " where " << COL_FEED
        << " in (";
    for (size_t i = 0; i < it->second.size(); ++i) {
      if (i) {
        sql << ",";
      }
      sql << it->second.at(i);
    }
    sql << ")";
    //      loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(it->first), false);
    qm_->Push(-1,0,DB_INSTANCE, CDbWServer, getIndexTab(it->first), sql, true);
  }
  return true;
}

 bool FeedAsyncI::AddFeedContent(const FeedSeedPtr & seed,
    const Ice::Current&) {
  if (!seed) {
    return false;
  }
  ostringstream os;
  for (MyUtil::Str2StrMap::iterator it = seed->extraProps.begin(); it
      != seed->extraProps.end(); ++it) {
    os << it->first << '=' << it->second << ',';
  }
  Statement sql;
  sql << "insert into " << FeedAsyncI::instance().getContentTab(seed->feed)
      << " (feed,source,type,big_type,small_type,actor,time,prop,xml,psource,ptype,mini_merge,news_merge) values ("
      << seed->feed << "," << seed->source << "," << seed->type << ","
      << int((seed->type & 0xffff) / 100) * 100 << "," << (seed->type
      & 0xffff) << "," << seed->actor << "," << "from_unixtime("
      << seed->time << ")," << mysqlpp::quote << os.str() << ","
      << mysqlpp::quote << seed->xml << "," << seed->psource << ","
      << seed->ptype << "," << seed->miniMerge << "," << seed->newsMerge << ")";
  //  QueryRunner(DB_INSTANCE, CDbWServer, getContentTab(seed->feed)).store(sql);

  string str = SqlConverter::instance().Convert(sql);
  qm_->Push(-1,0,DB_INSTANCE, CDbWServer, getContentTab(seed->feed), sql);
  return true;
}

bool FeedAsyncI::RemoveFeedContent(const MyUtil::LongSeq& feeds,
    const Ice::Current&) {
  if (!qm_->Check()) {
    return false;
  }
  if (feeds.empty()) {
    return true;
  }

  map<int, MyUtil::LongSeq> ids;
  for (size_t i = 0; i < feeds.size(); ++i) {
    ids[feeds.at(i) % 100].push_back(feeds.at(i));
  }
  for (map<int, MyUtil::LongSeq>::iterator it = ids.begin(); it != ids.end(); ++it) {
    Statement sql;
    sql << "delete from " << getContentTab(it->first) << " where feed in (";
    for (size_t i = 0; i < it->second.size(); ++i) {
      if (i) {
        sql << ",";
      }
      sql << it->second.at(i);
    }
    sql << ")";
    //    QueryRunner(DB_INSTANCE, CDbWServer, getContentTab(it->first)).store(sql);

    qm_->Push(-1,0,DB_INSTANCE, CDbWServer, getContentTab(it->first), sql,false);
  }
  return true;
}

bool FeedAsyncI::AddFeedMini(const FeedSeedPtr& seed,
    const Ice::Current&) {
  if (!qm_->Check()) {
    return false;
  }
  if (!seed) {
    return true;
  }

  if ((seed->type & 0x30000000) >> 28 == FeedMergeReplace) {
    Statement sql;
    sql << " delete from " << getMiniTab(seed->actor) << " where mini_merge="
        << seed->miniMerge << " AND  small_type=" << int(seed->type
        & 0xffff) << " AND user=" << seed->actor;
//    QueryRunner(-1,0,DB_INSTANCE, CDbWServer, getMiniTab(seed->actor)).store(sql);
    qm_->Push(-1,0,DB_INSTANCE, CDbWServer, getMiniTab(seed->actor), sql);
  }

  Statement sql;
  sql << "insert into " << getMiniTab(seed->actor)
      << " (user, feed, big_type,small_type,source,time,unread,mini_merge,type,actor) values ("
      << seed->actor << "," << seed->feed << "," << int((seed->type
      & 0xffff) / 100) * 100 << "," << int(seed->type & 0xffff) << ","
      << seed->source << ",from_unixtime(" << seed->time << "), 1 ,"
      << seed->miniMerge << " , " << seed->type << "," << seed->actor << " ) ";
  string str = SqlConverter::instance().Convert(sql);
  qm_->Push(-1,0,DB_INSTANCE, CDbWServer, getMiniTab(seed->actor), sql);
//  MCE_INFO("FeedAsyncI::addFeedMini --> cost:" << ts.getTime());
  return true;
}

bool FeedAsyncI::AddFeedSchool(int schoolid, const FeedSeedPtr& seed,
    const Ice::Current&) {
  if (!seed) {
    return true;
  }
  if (!qm_->Check()) {
    return false;
  }

  ostringstream os;
  os << "feed_mini_" << schoolid % 10;
  if (schoolid % 10 != 0) {
    os << "0";
  }
  if ((seed->type & 0x30000000) >> 28 == FeedMergeReplace) {
    Statement sql;
    sql << " delete from " << getSchoolTab(schoolid) << " where mini_merge="
        << seed->miniMerge << " AND  small_type=" << int(seed->type
        & 0xffff) << " AND user=" << schoolid;
    //    QueryRunner(DB_INSTANCE, CDbWServer, os.str()).store(sql);
    qm_->Push(-1,0,DB_INSTANCE, CDbWServer, os.str(), sql);
  }

  {
    Statement sql;
    sql << "insert into " << getSchoolTab(schoolid)
        << " (user, feed, big_type,small_type,source,time,unread,mini_merge,type,actor) values ("
        << schoolid << "," << seed->feed << ","
        << int((seed->type & 0xffff) / 100) * 100 << "," << int(seed->type
        & 0xffff) << "," << seed->source << ",from_unixtime(" << seed->time
        << "), 1 ," << seed->miniMerge << " , " << seed->type << ","
        << seed->actor << " ) ";
    //    QueryRunner(DB_INSTANCE, CDbWServer, os.str()).store(sql);
    qm_->Push(-1,0,DB_INSTANCE, CDbWServer, os.str(), sql);
  }
  return true;
}

bool FeedAsyncI::AddFeedGroup(int groupid, const FeedSeedPtr& seed,
    const Ice::Current&) {
  if (!seed) {
    return true;
  }
  if (!qm_->Check()) {
    return false;
  }
  if ((seed->type & 0x30000000) >> 28 == FeedMergeReplace) {
    Statement sql;
    sql << " delete from " << getGroupTab(groupid) << " where mini_merge="
        << seed->miniMerge << " AND  small_type=" << int(seed->type
        & 0xffff) << " AND group_id=" << groupid;
    //    QueryRunner(GROUP_DB_INSTANCE, CDbWServer, getGroupTab(groupid)).store(sql);
    qm_->Push(-1,0,GROUP_DB_INSTANCE, CDbWServer, getGroupTab(groupid), sql);
  }

  {
    Statement sql;
    sql << "insert into " << getGroupTab(groupid)
        << " (group_id, feed, big_type,small_type,source,mini_merge,type,actor) values ("
        << groupid << "," << seed->feed << "," << int((seed->type & 0xffff)
        / 100) * 100 << "," << int(seed->type & 0xffff) << ","
        << seed->source << "," << seed->miniMerge << " , " << seed->type << ","
        << seed->actor << " ) ";
    //    QueryRunner(GROUP_DB_INSTANCE, CDbWServer, getGroupTab(groupid)).store(sql);
    qm_->Push(-1,0,GROUP_DB_INSTANCE, CDbWServer, getGroupTab(groupid), sql);
  }
  return true;
}

bool FeedAsyncI::RemoveFeedMini(int user, int stype, Ice::Long miniMerge,
    const Ice::Current&) {
  if (!qm_->Check()) {
    return false;
  }

  Statement sql;
  sql << "delete from " << getMiniTab(user) << " where user=" << user
      << " and small_type=" << stype << " and mini_merge=" << miniMerge;
  //  QueryRunner(DB_INSTANCE, CDbWServer, getMiniTab(user)).store(sql);
  //  MCE_INFO("FeedAsyncI::removeFeedMini --> cost:" << ts.getTime());
  qm_->Push(-1,0,DB_INSTANCE, CDbWServer, getMiniTab(user), sql);
  return true;
}

bool FeedAsyncI::RemoveAllFeedMini(int user,
    const Ice::Current&) {
  if (!qm_->Check()) {
    return false;
  }
  Statement sql;
  sql << "delete from " << getMiniTab(user) << " where user=" << user;
  //  QueryRunner(DB_INSTANCE, CDbWServer, getMiniTab(user)).store(sql);
  qm_->Push(-1,0,DB_INSTANCE, CDbWServer, getMiniTab(user), sql);
  return true;
}
//
//int FeedQueueI::Push(const SqlInfoPtr& sql, const Ice::Current&) {
//  //  MCE_INFO("FeedQueueI::Push. " << sql->instance << " " << sql->wrflag << " " << sql->pattern << " " << sql->statement);
//  return qm_->Push(sql);
//  //  for(int i = 0; i != 100; ++i){
//  //    if(!qm_->Push(sql)){
//  //      return 0;
//  //    }
//  //  }//TODO 一定要删除
//  //  return 1;
//}
//
//void FeedQueueI::TestPush(const SqlInfoPtr& sql, const Ice::Current&) {
//  FeedQueueI::Push(sql);
//}
//
//void FeedQueueI::Restore(const string & begintime, const string & endtime,
//    int thishour, const Ice::Current&) {
//  MCE_INFO("FeedQueueI::Restore");
//  //   Restorer::instance().Restore(begintime,endtime);
//  TaskManager::instance().execute(new RestoreTask(begintime, endtime, thishour));
//}
//
//string FeedQueueI::DumpToString() {
//  return qm_->DumpToString();
//}
//int FeedQueueI::Size() {
//  return qm_->Size();
//}

void QueueStatTimer::handle() {
//  MCE_INFO("##QUEUESTAT##  FeedQueue " << FeedQueueI::instance().DumpToString()
//      << "  Restorer " << Restorer::instance().DumpToString());
}

//void FeedQueueI::Restore(){
//  vector<string> inlogs,outlogs;
//  MakeLogsName(inlogs,outlogs);
//  if(!Restorer::instance().AddLogs(inlogs,outlogs)){
//
//    return;
//  }
//  TaskManager::instance().execute(&Restorer::instance());
//}


