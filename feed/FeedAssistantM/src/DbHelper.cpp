#include "DbHelper.h"
#include "QueryRunner.h"
#include "IceLogger.h"
#include "FeedQueueAdapter.h"
#include "mysql++/query.h"
#include "RFeed.h"
#include <vector>
#include "Common.h"
using namespace std;
using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace MyUtil;

void DbHelper::Init(int async_property) {
  query_ready_ = false;
  is_async_ = 0;
  vector<pair<string, string> > v;
  v.push_back(make_pair(DB_INSTANCE, "feed_content_0"));
  v.push_back(make_pair(DB_INSTANCE, "feed_index_0"));
  v.push_back(make_pair(DB_INSTANCE, "feed_mini_0"));
  for (unsigned i = 0; i != v.size(); ++i) {
    try {
      ConnectionHolder conn(v.at(i).first, 3, v.at(i).second);//TODO 有么有更好的办法
      query_ = new mysqlpp::Query(conn.query());
      query_ready_ = true;
      break;
    } catch (Ice::Exception& e) {
      MCE_WARN("DbHelper::Init --> err:" << e);
    } catch (std::exception& e) {
      MCE_WARN("DbHelper::Init --> err:" << e.what());
    }
  }
  if(async_property && query_ready_){
    is_async_ = 1;
  }
  MCE_INFO("DbHelper::Init. async_property:"<< async_property
      <<" query_ready_" << query_ready_
      <<" is_async_" << is_async_);
}

void DbHelper::SetServiceIndex(int i) {
  index_ = i;
  MCE_INFO("DbHelper::SetServiceIndex. index:" << i)
}
void DbHelper::SetAsync(int async) {
  if (query_ready_) {
    is_async_ = async;
    MCE_INFO("DbHelper::SetAsync. is_async_:" << is_async_);
  } else {
    MCE_WARN("DbHelper::SetAsync. query_ready_:" << query_ready_);
  }
}

int DbHelper::WriteDb(const string & instance, int wrflag,
    const string & pattern, const Statement & statement, int actor,bool restore) {
  //正常
  if (is_async_) {
    return WriteDbAsync(instance, wrflag, pattern, statement, actor,restore);
  } else {
    return WriteDbSync(instance, wrflag, pattern, statement, actor);
  }


//  int res = WriteDbSync(instance, wrflag, pattern, statement, actor,restore);
//  if(is_async_){
//    WriteDbAsync(instance, wrflag, pattern, statement, actor,restore);
//  }
//  return res;
}

int DbHelper::WriteDbSync(const string & instance, int wrflag,
    const string & pattern, const Statement & statement, int actor) {
  TimeStat t1;
  int res = 0;
  try {
    QueryRunner(instance, wrflag, pattern).store(statement);
    res = 1;
  } catch (Ice::Exception& e) {
    MCE_WARN("DbHelper::WriteDbSync --> error:" << e);
    res = 0;
  } catch (std::exception& e) {
    MCE_WARN("DbHelper::WriteDbSync --> error:" << e.what());
    res = 0;
  }
  float t = t1.getTime();
  DbStatTimer::instance().Update(t, "db");
  return res;
}

int DbHelper::WriteDbAsync(const string & instance, int wrflag,
    const string & pattern, const Statement & statement, int actor,bool restore) {
  string sqlstr;
  {
    //把statement转成string
    IceUtil::Mutex::Lock lock(query_mutex_);
    statement.query(*query_);
    sqlstr = query_->str();
    query_->reset();
  }
  SqlInfoPtr info = new SqlInfo;
  info->instance = instance;
  info->wrflag = wrflag;
  info->pattern = pattern;
  info->statement = sqlstr;
  info->flag = restore?1:0;

  TimeStat t1;
  int res = 0;
  try {
    //TODO 按照什么来散服务
    res = FeedQueueAdapter::instance().Push(index_, info);
//    FeedQueueAdapter::instance().TestPush(index_, info);
  } catch (Ice::Exception& e) {
    MCE_WARN("DbHelper::WriteDbAsync --> call feedqueue err:" << e);
    res = 0;
  } catch (std::exception& e) {
    MCE_WARN("DbHelper::WriteDbAsync --> call feedqueue err:" << e.what());
    res = 0;
  }
  float t = t1.getTime();
  DbStatTimer::instance().Update(t, "q");
  return res;
}
int DbHelper::addFeedIndex(const FeedIndexPtr& itemPtr) {
  if (!itemPtr) {
    return 1;//TODO
  }
  TimeStat ts;
  Statement sql;
  sql << "INSERT INTO " << getIndexTab(itemPtr->feed)
      << " (feed,source,psource,small_type,big_type,ptype,actor,time,mini_merge,news_merge)"
      << " VALUES (" << itemPtr->feed << "," << itemPtr->source << ","
      << itemPtr->psource << ", " << itemPtr->stype << ","
      << int(itemPtr->stype / 100) * 100 << "," << itemPtr->ptype << ","
      << itemPtr->actor << ",from_unixtime(" << itemPtr->time << "),"
      << itemPtr->miniMerge << "," << itemPtr->newsMerge << ")";

  return WriteDb(DB_INSTANCE, CDbWServer, getIndexTab(itemPtr->feed), sql,
      itemPtr->actor,true);//TODO

  MCE_INFO("DbHelper::addFeedIndex --> cost:" << ts.getTime());
}
FeedIndexSeq DbHelper::getFeedIndex(int stype, Ice::Long source, int actor) {

  IndexLoaderPtr loader = new IndexLoader();
  for (size_t i = 0; i < 10; ++i) {
    ostringstream sql;
    sql << "select * from " << getIndexTab(i) << " where " << COL_SMALLTYPE
        << " = " << stype << " and " << COL_SOURCE << " = " << source
        << " and " << COL_ACTOR << " = " << actor << " limit 5";
    loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(i), true);
    //MCE_INFO("DbHelper::getFeedIndex --> addQuery, i = " << i << ",sql = " << sql.str());
  }
  return loader->query();
}
FeedIndexSeq DbHelper::getFeedIndexBySource(int stype, Ice::Long source) {
  IndexLoaderPtr loader = new IndexLoader();
  for (size_t i = 0; i < 10; ++i) {
    ostringstream sql;
    sql << "select * from " << getIndexTab(i) << " where " << COL_SMALLTYPE
        << " = " << stype << " and " << COL_SOURCE << " = " << source
        << " order by time desc limit 10";
    loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(i), true);
  }
  return loader->query();
}

FeedIndexSeq DbHelper::getFeedIndexByPsource(int ptype, Ice::Long psource) {
  IndexLoaderPtr loader = new IndexLoader();
  for (size_t i = 0; i < 10; ++i) {
    ostringstream sql;
    sql << "select * from " << getIndexTab(i) << " where " << COL_PTYPE
        << " = " << ptype << " and " << COL_PSOURCE << " = " << psource
        << " order by time desc limit 30";
    loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(i), true);
  }
  return loader->query();
}
FeedIndexPtr DbHelper::getFeedIndex(Ice::Long feed) {
  TimeStat ts;
  Statement sql;
  sql << "SELECT * FROM " << getIndexTab(feed) << " WHERE " << COL_FEED
      << " = " << feed;
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer,
      getIndexTab(feed)).store(sql);
  if (!res) {
    MCE_WARN("DbHelper::getFeedIndex --> query db fail ");
    return 0;
  }
  if (res.empty()) {
    MCE_INFO("DbHelper::getFeedIndex --> res is empty");
    return 0;
  }
  FeedIndexPtr index = new FeedIndex;
  for (size_t i = 0; i < res.num_rows(); ++i) {
    mysqlpp::Row& row = res.at(i);
    index->feed = row[COL_FEED];
    index->newsMerge = row[COL_NEWS_MERGE];
    index->miniMerge = row[COL_MINI_MERGE];
    index->source = row[COL_SOURCE];
    index->psource = row[COL_PSOURCE];
    index->stype = row[COL_SMALLTYPE];
    index->ptype = row[COL_PTYPE];
    index->actor = row[COL_ACTOR];
    index->time = (time_t) mysqlpp::DateTime(row[COL_TIME]);
  }
  MCE_INFO("DbHelper::getFeedIndex --> cost:" << ts.getTime());
  return index;
}
void DbHelper::removeFeedIndex(const MyUtil::LongSeq& feeds, int actor) {
  MCE_INFO("DbHelper::removeAllFeedMini --> feeds size = " << feeds.size());
  map<int, MyUtil::LongSeq> tmp;
  for (size_t i = 0; i < feeds.size(); ++i) {
    tmp[feeds.at(i) % 10].push_back(feeds.at(i));
  }
  IndexLoaderPtr loader = new IndexLoader();
  for (map<int, MyUtil::LongSeq>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
    ostringstream sql;
    sql << "delete from " << getIndexTab(it->first) << " where " << COL_FEED
        << " in (";
    for (size_t i = 0; i < it->second.size(); ++i) {
      if (i) {
        sql << ",";
      }
      sql << it->second.at(i);
    }
    sql << ")";
    loader->addQuery(DB_INSTANCE, sql.str(), getIndexTab(it->first), actor,
        false);
  }
  loader->query();
}

int DbHelper::addFeedContent(const FeedSeedPtr& seed) {
  if (!seed) {
    return 1;
  }
  TimeStat ts;
  ostringstream os;
  for (MyUtil::Str2StrMap::iterator it = seed->extraProps.begin(); it
      != seed->extraProps.end(); ++it) {
    os << it->first << '=' << it->second << ',';
  }
  Statement sql;
//  ostringstream sql;
  sql << "insert into " << DbHelper::instance().getContentTab(seed->feed)
      << " (feed,source,type,big_type,small_type,actor,time,prop,xml,psource,ptype,mini_merge,news_merge) values ("
      << seed->feed << "," << seed->source << "," << seed->type << ","
      << int((seed->type & 0xffff) / 100) * 100 << "," << (seed->type
      & 0xffff) << "," << seed->actor << "," << "from_unixtime("
      << seed->time << ")," << mysqlpp::quote << os.str() << ","
      << mysqlpp::quote << seed->xml << "," << seed->psource << ","
      << seed->ptype << "," << seed->miniMerge << "," << seed->newsMerge << ")";
//  Statement state;
//  state << sql.str();
//  MCE_INFO("DbHelper::addFeedContent. sql: " << sql.str());
  //  QueryRunner(DB_INSTANCE, CDbWServer, getContentTab(seed->feed)).store(sql);
  int flag = WriteDb(DB_INSTANCE, CDbWServer, getContentTab(seed->feed), sql,
      seed->actor,true);
  MCE_INFO("DbHelper::addFeedContent --> cost:" << ts.getTime());
  return flag;
}

//�滻XML
void DbHelper::ReplaceXML(const MyUtil::LongSeq & feeds, const string & xml,
    int actor) {
  MCE_DEBUG("DbHelper::ReplaceXML --> feeds size = " << feeds.size());
  if (feeds.empty()) {
    return;
  }
  TimeStat ts;
  map<int, MyUtil::LongSeq> ids;
  for (size_t i = 0; i < feeds.size(); ++i) {
    ids[feeds.at(i) % 100].push_back(feeds.at(i));
  }

  for (map<int, MyUtil::LongSeq>::iterator it = ids.begin(); it != ids.end(); ++it) {
    Statement sql;
    sql << "update " << getContentTab(it->first) << " set xml = "
        << mysqlpp::quote << xml << " where feed in (";
    for (size_t i = 0; i < it->second.size(); ++i) {
      if (i) {
        sql << ",";
      }
      sql << it->second.at(i);
    }
    sql << ")";

    WriteDb(DB_INSTANCE, CDbWServer, getContentTab(it->first), sql, actor,true);
  }
  MCE_INFO("DbHelper::ReplaceXML --> cost:" << ts.getTime());
}
void DbHelper::removeFeedContent(const MyUtil::LongSeq& feeds, int actor) {
  if (feeds.empty()) {
    return;
  }
  TimeStat ts;
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
    WriteDb(DB_INSTANCE, CDbWServer, getContentTab(it->first), sql, actor,false);

  }
  MCE_INFO("DbHelper::removeFeedContent --> cost:" << ts.getTime());
}

map<string, string> DbHelper::getFeedConfigProp(Ice::Long fid) {
  TimeStat ts;
  map<string, string> prop;
  Statement sql;
  sql << "SELECT prop FROM " << DbHelper::instance().getContentTab(fid)
      << " WHERE feed=" << fid;
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer,
      getContentTab(fid)).store(sql);
  string propStr;
  if (!res || res.num_rows() == 0) {
    MCE_WARN("DbHelper::getFeedConfigProp --> load from db null, fid:" << fid);
    return prop;
  }
  for (size_t i = 0; i < res.num_rows(); ++i) {
    mysqlpp::Row row = res.at(i);
    propStr = row["prop"].data();
    size_t pos = 0;
    string::size_type idx;
    string key;
    string val;

    while (pos < propStr.size()) {

      idx = propStr.find('=', pos);
      if (idx == string::npos) {
        return prop;
      }

      key = propStr.substr(pos, idx - pos);
      pos = idx + 1;
      idx = propStr.find(',', pos);
      if (idx == string::npos) {
        return prop;
      }

      val = propStr.substr(pos, idx - pos);
      pos = idx + 1;

      prop[key] = val;

    }

    return prop;
  }
  MCE_INFO("DbHelper::getFeedConfigProp --> cost:" << ts.getTime());
  return prop;

}

int DbHelper::addFeedMini(const FeedSeedPtr& seed) {
  if (!seed) {
    return 1;
  }
  TimeStat ts;
  if ((seed->type & 0x30000000) >> 28 == FeedMergeReplace) {
    Statement sql;
    sql << " delete from " << getMiniTab(seed->actor) << " where mini_merge="
        << seed->miniMerge << " AND  small_type=" << int(seed->type
        & 0xffff) << " AND user=" << seed->actor;
    QueryRunner(DB_INSTANCE, CDbWServer, getMiniTab(seed->actor)).store(sql);
  }
  {
    Statement sql;
    sql << "insert into " << getMiniTab(seed->actor)
        << " (user, feed, big_type,small_type,source,time,unread,mini_merge,type,actor) values ("
        << seed->actor << "," << seed->feed << "," << int((seed->type
        & 0xffff) / 100) * 100 << "," << int(seed->type & 0xffff)
        << "," << seed->source << ",from_unixtime(" << seed->time << "), 1 ,"
        << seed->miniMerge << " , " << seed->type << "," << seed->actor
        << " ) ";
    return WriteDb(DB_INSTANCE, CDbWServer, getMiniTab(seed->actor), sql,
        seed->actor,true);

  }
  MCE_INFO("DbHelper::addFeedMini --> cost:" << ts.getTime());
}

void DbHelper::addFeedSchool(int schoolid, const FeedSeedPtr& seed) {
  if (!seed) {
    return;
  }
  TimeStat ts;
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
    WriteDb(DB_INSTANCE, CDbWServer, os.str(), sql, seed->actor,false);

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
    WriteDb(DB_INSTANCE, CDbWServer, os.str(), sql, seed->actor,true);

  }
  MCE_INFO("DbHelper::addFeedSchool --> cost:" << ts.getTime());
}

void DbHelper::addFeedGroup(int groupid, const FeedSeedPtr& seed) {
  if (!seed) {
    return;
  }
  if ((seed->type & 0x30000000) >> 28 == FeedMergeReplace) {
    Statement sql;
    sql << " delete from " << getGroupTab(groupid) << " where mini_merge="
        << seed->miniMerge << " AND  small_type=" << int(seed->type
        & 0xffff) << " AND group_id=" << groupid;
    WriteDb(DB_INSTANCE, CDbWServer, getGroupTab(groupid), sql, seed->actor,true);

  }

  {
    Statement sql;
    sql << "insert into " << getGroupTab(groupid)
        << " (group_id, feed, big_type,small_type,source,mini_merge,type,actor) values ("
        << groupid << "," << seed->feed << "," << int((seed->type & 0xffff)
        / 100) * 100 << "," << int(seed->type & 0xffff) << ","
        << seed->source << "," << seed->miniMerge << " , " << seed->type << ","
        << seed->actor << " ) ";
    WriteDb(DB_INSTANCE, CDbWServer, getGroupTab(groupid), sql, seed->actor,true);

  }
}

void DbHelper::removeFeedMini(int user, int stype, Ice::Long miniMerge) {
  TimeStat ts;
  Statement sql;
  sql << "delete from " << getMiniTab(user) << " where user=" << user
      << " and small_type=" << stype << " and mini_merge=" << miniMerge;
  WriteDb(DB_INSTANCE, CDbWServer, getMiniTab(user), sql, user,false);

  MCE_INFO("DbHelper::removeFeedMini --> cost:" << ts.getTime());
}

void DbHelper::removeAllFeedMini(int user) {
  Statement sql;
  sql << "delete from " << getMiniTab(user) << " where user=" << user;
  WriteDb(DB_INSTANCE, CDbWServer, getMiniTab(user), sql, user,false);

}

map<string, string> DbHelper::getFeedContent(Ice::Long feed_id) {
  TimeStat ts;
  Statement sql;
  sql << "select * from " << getContentTab(feed_id) << " where feed = "
      << feed_id;

  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer,
      getContentTab(feed_id)).store(sql);

  map<string, string> data;
  if (!res) {
    return data;
  }

  for (size_t i = 0; i < res.num_rows(); ++i) {
    mysqlpp::Row row = res.at(i);

    data["feed"] = row["feed"] .data();
    data["source"] = row["source"] .data();
    data["type"] = row["type"] .data();
    data["big_type"] = row["big_type"] .data();
    data["small_type"] = row["small_type"] .data();
    data["actor"] = row["actor"] .data();
    data["time"] = row["time"] .data();
    data["prop"] = row["prop"] .data();
    data["xml"] = row["xml"] .data();
    data["count_expr"] = row["count_expr"] .data();
    data["older_reply_id"] = row["older_reply_id"].data();
    data["older_reply"] = row["older_reply"] .data();
    data["newer_reply_id"] = row["newer_reply_id"].data();
    data["newer_reply"] = row["newer_reply"] .data();
    data["psource"] = row["psource"] .data();
    data["ptype"] = row["ptype"] .data();
    data["mini_merge"] = row["mini_merge"] .data();
    data["news_merge"] = row["news_merge"] .data();
    data["comment_count"] = row["comment_count"] .data();
  }
  /*

   for (map<string, string>::iterator it = data.begin(); it != data.end(); ++it) {
   MCE_INFO("key:" << it->first << ", value:"<< it->second);
   }*/
  MCE_INFO("DbHelper::getFeedContent --> cost:" << ts.getTime());
  return data;
}

bool DbHelper::copyFeedContent(Ice::Long old_feed_id, Ice::Long new_feed_id,
    int new_type, const string& new_props) {

  TimeStat ts;
  map<string, string> data = getFeedContent(old_feed_id);
  if (data.empty()) {
    return false;
  }
  data["feed"] = boost::lexical_cast<string>(new_feed_id);
  data["type"] = boost::lexical_cast<string>(new_type);
  data["big_type"] = boost::lexical_cast<string>(
      ((new_type & 0xffff) / 100) * 100);
  data["small_type"] = boost::lexical_cast<string>(new_type & 0xffff);
  data["prop"] = new_props;

  data["older_reply_id"] = "0";
  data["older_reply"] = "";
  data["newer_reply_id"] = "0";
  data["newer_reply"] = "";
  data["comment_count"] = "0";
  data.erase("time");

  Statement sql;
  sql << "insert into " << getContentTab(new_feed_id) << " (";
  for (map<string, string>::iterator it = data.begin(); it != data.end(); ++it) {
    if (it != data.begin()) {
      sql << ",";
    }
    sql << it->first;
  }
  sql << ") values (";
  for (map<string, string>::iterator it = data.begin(); it != data.end(); ++it) {
    if (it != data.begin()) {
      sql << ",";
    }
    sql << mysqlpp::quote << it->second;
  }
  sql << ")";
  WriteDb(DB_INSTANCE, CDbWServer, getContentTab(new_feed_id), sql, 0,false);//TODO

  MCE_INFO("DbHelper::copyFeedContent --> cost:" << ts.getTime());

  return true;
}

void DbHelper::UpdateReply(const map<Ice::Long, FeedReplyPtr>& buffer) {
  typedef map<Ice::Long, FeedReplyPtr> BufferMap;
  typedef BufferMap::const_iterator BufferMapIter;
  map<int, vector<BufferMapIter> > tmp;
  for (BufferMapIter iter = buffer.begin(); iter != buffer.end(); ++iter) {
    tmp[iter->first % 100].push_back(iter);
  }
  for (map<int, vector<BufferMapIter> >::iterator iter = tmp.begin(); iter
      != tmp.end(); ++iter) {
    Statement sql;
    //MCE_INFO("DbHelper::UpdateReply --> index:" << iter->first << " size:" << iter->second.size());
    sql << " insert into " << getContentTab(iter->first)
        << " (feed,older_reply_id,older_reply,newer_reply_id,newer_reply, comment_count) values ";
    for (vector<BufferMapIter>::iterator h = iter->second.begin(); h
        != iter->second.end(); ++h) {
      if (h != iter->second.begin()) {
        sql << " , ";
      }
      sql << " (" << (*h)->first << "," << (*h)->second->oldReplyId << ","
          << mysqlpp::quote << (*h)->second->oldReply << ","
          << (*h)->second->newReplyId << "," << mysqlpp::quote
          << (*h)->second->newReply << "," << (*h)->second->count << ") ";
    }
    sql
        << " on duplicate key update older_reply_id=values(older_reply_id),older_reply=values(older_reply),newer_reply_id=values(newer_reply_id),newer_reply=values(newer_reply),comment_count=values(comment_count)";
    QueryRunner(DB_INSTANCE, CDbWServer, getContentTab(iter->first)).schedule(
        sql,false);//TODO
  }
}
/*
 Ice::Long DbHelper::createId() {
 IceUtil::Mutex::Lock lock(_mutex);
 while (1) {
 MCE_DEBUG("_curIndex:"<<_curIndex << ", _topIndex:"<<_topIndex);
 if (_curIndex != -1 && _curIndex < _topIndex) {
 return _curIndex++;
 }
 {
 Statement sql;
 sql << "update " << TAB_ID_SEQ << " SET id = id +"
 << ID_SEQ_STEP;
 QueryRunner(DB_INSTANCE, CDbWServer, TAB_ID_SEQ).store(sql);
 }
 {
 Statement sql;
 sql << "SELECT * FROM " << TAB_ID_SEQ;
 mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE,
 CDbRServer, TAB_ID_SEQ).store(sql);
 if (!res) {
 return -1;
 }
 for (size_t i = 0; i < res.num_rows(); ++i) {
 mysqlpp::Row row = res.at(i);
 _topIndex = (int) row["ID"];
 _curIndex = _topIndex - ID_SEQ_STEP;
 MCE_DEBUG("topIndex:"<<_topIndex);
 break;
 }
 }

 }
 }
 */
//---------------------------------------------------------------------------

FeedIndexSeq IndexLoader::query() {
  for (size_t i = 0; i < _taskSeq.size(); ++i) {
    //MCE_INFO("IndexLoader::query --> execute task i = " << i);
    MyUtil::TaskManager::instance().execute(_taskSeq.at(i));
  }
  _taskSeq.clear();

  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
  if (!timedWait(IceUtil::Time::milliSeconds(LOAD_TIMEOUT))) {
    MCE_WARN("IndexLoader::query -> count: " << _count << " missing");
  } else {
    //MCE_INFO("IndexLoader::query -> success");
  }
  return _indexSeq;
}
void IndexLoader::addQuery(const string& dbInstance, const string& sql,
    const string& pattern, int actor, bool isSelect) {
  SqlExecuteTask* t = new SqlExecuteTask(this, isSelect);
  t->init(dbInstance, sql, pattern, actor);
  _taskSeq.push_back(t);
  ++_count;
}

void IndexLoader::SqlExecuteTask::handle() {
  //	MCE_DEBUG("IndexLoader --> SqlExecuteTask  seq:"<<_sql);
  TimeStat ts;
  if (_isSelect) {
    mysqlpp::StoreQueryResult res;
    try {
      res = QueryRunner(_dbInstance, CDbRServer, _pattern).store(_sql);
    } catch (std::exception& e) {
      MCE_WARN("IndexLoader::SqlExecuteTask --> err, " << e.what());
    }
    if (!res) {
      MCE_WARN("IndexLoader::SqlExecuteTask::handle --> query return null");
      return;
    }
    //MCE_INFO("IndexLoader::SqlExecuteTask::handle -->1 cost:" << ts.getTime() << " rows:" << res.num_rows() << " expr:" << _pattern);
    FeedIndexSeq seq;
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);

      FeedIndexPtr index = new FeedIndex;
      index->feed = row[COL_FEED];
      index->newsMerge = row[COL_NEWS_MERGE];
      index->miniMerge = row[COL_MINI_MERGE];
      index->source = row[COL_SOURCE];
      index->psource = row[COL_PSOURCE];
      index->stype = row[COL_SMALLTYPE];
      index->ptype = row[COL_PTYPE];
      index->actor = row[COL_ACTOR];
      index->time = (time_t) mysqlpp::DateTime(row[COL_TIME]);
      seq.push_back(index);

    }
    _loader->finish(seq);

  } else {
    try {
      DbHelper::instance().WriteDb(_dbInstance, CDbWServer, _pattern, _sql,
          actor,false);
    } catch (std::exception& e) {
      MCE_WARN("IndexLoader::SqlExecuteTask --> err, " << e.what());
    }
    _loader->finish();
  }
  //MCE_INFO("IndexLoader::SqlExecuteTask::handle  --> cost:" << ts.getTime());
}

