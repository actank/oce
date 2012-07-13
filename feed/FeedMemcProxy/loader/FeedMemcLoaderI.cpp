#include "FeedMemcLoaderI.h"

#include <boost/lexical_cast.hpp>

#include "QueryRunner.h"
#include "ServiceI.h"
#include "FeedMemcProxy/client/feed_memc_client.h"

using namespace xce::feed;

using namespace com::xiaonei::xce;
using namespace MyUtil;

const int DEFAULT_TIMEOUT = 200;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedMemcLoaderI::instance(),
      service.createIdentity("M", ""));

  ThreadPoolConfig cfg(8, 64);
  TaskManager::instance().config(0, cfg);
}

static FeedContentPtr LoadFeedFromDB(Ice::Long id, bool * exist) {
  Statement sql;
  sql
      << "SELECT feed, source, actor, type, xml, time, older_reply_id, older_reply, "
        " newer_reply_id, newer_reply, comment_count from "
      << FeedContentDB::GetTableName(id) << " WHERE feed = " << id;

  FeedContentPtr content = NULL;
  *exist = true;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner(FeedContentDB::GetInstance(),
        CDbRServer, FeedContentDB::GetTableName(id)).store(sql);

    if (!res) {
      MCE_WARN("LoadFeedFromDB.load content " << id << " res is NULL");
      return 0;
    }
    if (res.num_rows() < 1) {
      *exist = false;
      MCE_WARN("LoadFeedFromDB.load content " << id << " not exist");
      return 0;
    }

    mysqlpp::Row row = res.at(0);

    content = new FeedContent();
    content->data = new FeedData();

    content->data->feed = id;
    content->data->source = row["source"];
    content->data->actor = row["actor"];
    content->data->type = row["type"];
    content->data->xml = row["xml"].data();
    content->data->time = (time_t) mysqlpp::DateTime(row["time"]);

    content->reply = new FeedReply();
    content->reply->oldReplyId = Ice::Long(row["older_reply_id"]);
    content->reply->oldReply = row["older_reply"].data();
    content->reply->newReplyId = Ice::Long(row["newer_reply_id"]);
    content->reply->newReply = row["newer_reply"].data();
    content->reply->count = row["comment_count"];
  } catch (std::exception& e) {
    MCE_WARN("getCount() err : " << e.what());
  } catch (...) {
    MCE_WARN("getCount() unkown exception at " << __FILE__ << ":" << __LINE__);
  }

  return content;
}

static void LoadFeedFromDBParal(int uid,MyUtil::LongSeq ids, FeedContentDict & dict,
    int & not_exist,int timeout) {
  ParaLoaderPtr loader = new ParaLoader(uid,timeout);
  map<int, vector<Ice::Long> > id_map = FeedContentDB::GetIdMap(ids);
  MCE_INFO("LoadFeedFromDBParal. id_map size:" << id_map.size());
  for (map<int, vector<Ice::Long> >::iterator i = id_map.begin(); i
      != id_map.end(); ++i) {
    ostringstream sql;
    sql
        << "SELECT feed, source, actor, type, xml, time, older_reply_id, older_reply, "
          " newer_reply_id, newer_reply, comment_count from "
        << FeedContentDB::GetTableName(i->first) << " WHERE feed IN (";
    for (unsigned j = 0; j != i->second.size(); ++j) {
      sql << i->second.at(j);
      if (j != i->second.size() - 1) {
        sql << ",";
      }
    }
    sql << ")";
    loader->addQuery(FeedContentDB::GetInstance(), sql.str(),
        FeedContentDB::GetTableName(i->first), i->second);
  }
  dict = loader->query(not_exist);
}

void LoadFeedTask::handle() {
  bool exist;
  FeedContentPtr content = LoadFeedFromDB(id_, &exist);
  if (!content) {
    MCE_INFO("load " << id_ << " from database fail!");
    if (!exist) {
      FeedMemcClient::instance().SetEmptyFeed(id_);
    }
    return;
  }

//  MCE_INFO("LoadFeedTask::handle - feed loaded from database. id=" << id_);

  if (!FeedMemcClient::instance().SetFeed(content)) {
    MCE_WARN("set content " << id_ << " to memcached fail!");
  }
}

FeedMemcLoaderI::FeedMemcLoaderI() :
  MyUtil::Timer(1000 * 60 * 3) {
  MyUtil::TaskManager::instance().scheduleRepeated(this);
}

void FeedMemcLoaderI::handle() {
  MCE_DEBUG("clear loaded set.");
  set<Ice::Long> empty;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    recent_loaded_ids_.swap(empty);
  }
}

void FeedMemcLoaderI::LoadFeedSeqByKey(const MyUtil::StrSeq & keys,
    const Ice::Current&) {
  for (size_t i = 0; i < keys.size(); ++i) {
    if (strncmp("FEED#", keys[i].c_str(), 5) == 0) {
      Ice::Long id = boost::lexical_cast<Ice::Long>(keys[i].c_str() + 5);
      LoadFeed(id);
    }
  }
}

Str2StrMap FeedMemcLoaderI::GetFeedSeqByKey(const MyUtil::StrSeq & keys,
    const Ice::Current&) {
  Str2StrMap res;
  Ice::Long id = 0;

  timeval tv_start, tv_end;
  gettimeofday(&tv_start, NULL);

  for (size_t i = 0; i < keys.size(); ++i) {
    if (strncmp("FEED#", keys[i].c_str(), 5) != 0) {
      continue;
    }

    try {
      id = boost::lexical_cast<Ice::Long>(keys[i].c_str() + 5);
    } catch (boost::bad_lexical_cast &) {
      continue;
    }
    bool exist;
    FeedContentPtr content = LoadFeedFromDB(id, &exist);
    if (!content) {
      if (!exist) {
        MCE_INFO("feed not found in database. id=" << id);
        FeedMemcClient::instance().SetEmptyFeed(id);
      }
      continue;
    }

    MCE_INFO("GetFeedSeqByKey : feed loaded from database. id=" << id);

    gettimeofday(&tv_end, NULL);

    float timeuse = 1000000 * (tv_end.tv_sec - tv_start.tv_sec)
        + tv_end.tv_usec - tv_start.tv_usec;
    timeuse /= 1000;
    if (timeuse >= 20) {
      MCE_WARN("11111111111 time : " << timeuse <<" ms. key count=" << keys.size());
    }

    FeedMemcClient::instance().SetFeed(content);

    string value;
    bool b = FeedMemcClient::instance().SerializeFeed(content, &value);
//    FeedMemcClient::instance().SetMemcached(keys[i].c_str(), value, b ? 0x01
//        : 0);
    MCE_DEBUG("key add to memcached " << keys[i]);

    value.push_back(b ? 'c' : 'u'); // 山寨做法, 标识是否压缩
    res.insert(make_pair(keys[i], value));
  }

  gettimeofday(&tv_end, NULL);

  float timeuse = 1000000 * (tv_end.tv_sec - tv_start.tv_sec) + tv_end.tv_usec
      - tv_start.tv_usec;
  timeuse /= 1000;
  //if(timeuse >= 20) {
  MCE_WARN("time : " << timeuse <<" ms. key count=" << keys.size());
  //}

  return res;
}

void FeedMemcLoaderI::LoadFeedSeq(const Ice::LongSeq & ids, const Ice::Current&) {
  TimeStat ts;
  FeedContentDict dict;
  int not_exist = 0;
  int timeout = DEFAULT_TIMEOUT;
  LoadFeedFromDBParal(0 ,ids, dict, not_exist,timeout);//默认200ms

  float t = ts.getTime();
//  TaskManager::instance().execute(new MemcTask(dict));
  MCE_INFO("FeedMemcLoaderI::LoadFeedSeq. input fids:" << ids.size()
  		<< " output size:" << dict.size()<< " not exist:" << not_exist <<" cost:" << t
  		<< " timeout:" << timeout
  );
  return ;

//  TimeStat ts;
//  for (size_t i = 0; i < ids.size(); ++i) {
//    LoadFeed(ids[i]);
//  }
//  MCE_INFO("FeedMemcLoaderI::LoadFeedSeq.ids size:" << ids);
}

void FeedMemcLoaderI::LoadFeedByKey(const string & key, const Ice::Current&) {
	MCE_INFO("FeedMemcLoaderI::LoadFeedByKey. key:" << key);
  if (strncmp("FEED#", key.c_str(), 5) == 0) {
    Ice::Long id = boost::lexical_cast<Ice::Long>(key.c_str() + 5);
    LoadFeed(id);
  }
}

void FeedMemcLoaderI::LoadFeed(Ice::Long id, const Ice::Current&) {
	MCE_INFO("FeedMemcLoaderI::LoadFeed. fid:" << id);
  {
    IceUtil::Mutex::Lock lock(mutex_);
    if (recent_loaded_ids_.find(id) != recent_loaded_ids_.end()) {
      MCE_DEBUG("recently loaded id : " << id);
      return;
    }
    MCE_DEBUG("to load id : " << id);
    recent_loaded_ids_.insert(id);
  }

  TaskManager::instance().execute(new LoadFeedTask(id));
}
;

map<Ice::Long, FeedContentPtr> FeedMemcLoaderI::GetFeedDictSingleThread(const vector<
    Ice::Long> & ids) {
  Ice::Long id = 0;

  timeval tv_start, tv_end;
  gettimeofday(&tv_start, NULL);
  map<Ice::Long, FeedContentPtr> dict;
  for (size_t i = 0; i < ids.size(); ++i) {
    id = ids.at(i);

    bool exist;
    FeedContentPtr content = LoadFeedFromDB(id, &exist);
    if (!content) {
      if (!exist) {
        MCE_INFO("FeedMemcLoaderI::GetFeedDictSingleThread. feed not found in database. id=" << id);
        FeedMemcClient::instance().SetEmptyFeed(id);
        dict.insert(make_pair(id, FeedContentPtr()));
      }
      continue;
    }

    MCE_INFO("FeedMemcLoaderI::GetFeedDictSingleThread : feed loaded from database. id:" << id);

    gettimeofday(&tv_end, NULL);

    float timeuse = 1000000 * (tv_end.tv_sec - tv_start.tv_sec)
        + tv_end.tv_usec - tv_start.tv_usec;
    timeuse /= 1000;
    if (timeuse >= 20) {
      MCE_WARN("FeedMemcLoaderI::GetFeedDictSingleThread. time : " << timeuse <<" ms. ids count=" << ids.size());
    }

    if (!FeedMemcClient::instance().SetFeed(content)) {
      MCE_WARN("FeedMemcLoaderI::GetFeedDictSingleThread. set content " << id << " to memcached fail!");
    }
    //    string value;
    //    bool b = FeedMemcClient::instance().SerializeFeed(content, &value);
    //    string key("FEED#");
    //    key += boost::lexical_cast<string>(id);
    //    FeedMemcClient::instance().SetMemcached(key.c_str(), value, b ? 0x01: 0);

    //    value.push_back(b ? 'c' : 'u'); // 山寨做法, 标识是否压缩

    MCE_DEBUG("FeedMemcLoaderI::GetFeedDictSingleThread. add to memcached id:" << id );

    //    res.insert(make_pair(keys[i], value));
    dict.insert(make_pair(id, content));
  }

  gettimeofday(&tv_end, NULL);

  float timeuse = 1000000 * (tv_end.tv_sec - tv_start.tv_sec) + tv_end.tv_usec
      - tv_start.tv_usec;
  timeuse /= 1000;
  //if(timeuse >= 20) {
  MCE_WARN("FeedMemcLoaderI::GetFeedDictSingleThread.time : " << timeuse <<" ms. ids count=" << ids.size());
  //}

  return dict;
}

map<Ice::Long, FeedContentPtr> FeedMemcLoaderI::GetFeedDict(const vector<
    Ice::Long> & ids, const Ice::Current&) {
  TimeStat ts;
  FeedContentDict dict;
  int not_exist = 0;
  int timeout = DEFAULT_TIMEOUT;
  LoadFeedFromDBParal(0, ids, dict, not_exist,timeout);

  float t = ts.getTime();
//  TaskManager::instance().execute(new MemcTask(dict));
  MCE_INFO("FeedMemcLoaderI::GetFeedDict. input fids:" << ids.size()
  		<< " output size:" << dict.size()<< " not exist:" << not_exist <<" cost:" << t
  		<< " timeout:" << timeout
  );
  return dict;
}

map<Ice::Long, FeedContentPtr> FeedMemcLoaderI::GetFeedDictTimeout(int uid,const vector<
    Ice::Long> & ids, int timeout, const Ice::Current&) {
  TimeStat ts;
  FeedContentDict dict;
  int not_exist = 0;
  LoadFeedFromDBParal(uid,ids, dict, not_exist,timeout);

  float t = ts.getTime();
//  TaskManager::instance().execute(new MemcTask(dict));
  MCE_INFO("FeedMemcLoaderI::GetFeedDictTimeout. uid:" << uid << " input fids:" << ids.size() << " output size:" << dict.size()<< " not exist:" << not_exist <<" cost:" << t << " timeout:" << timeout);
  return dict;
}

FeedContentDict ParaLoader::query(int & not_exist) {
  int total = _count;
  for (size_t i = 0; i < _taskSeq.size(); ++i) {
    //    MCE_INFO("ParaLoader::query --> execute task i = " << i);
    MyUtil::TaskManager::instance().execute(_taskSeq.at(i));
  }
  _taskSeq.clear();

  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
  if (!timedWait(IceUtil::Time::milliSeconds(timeout_))) {
    MCE_WARN("ParaLoader::query -> uid:" << uid_ << " total:" << total << " miss:" << _count << "  fail!");
  } else {
    MCE_INFO("ParaLoader::query -> uid:" << uid_ << " total:" << total << " miss:" << _count << "  success");
  }
  not_exist = not_exist_;
  return fdict_;
}
void ParaLoader::addQuery(const string& dbInstance, const string& sql,
    const string& pattern, const vector<Ice::Long>& fids) {
  SqlExecuteTask* t = new SqlExecuteTask(uid_,this);
  t->init(dbInstance, sql, pattern, fids);
  _taskSeq.push_back(t);
  ++_count;
}

void ParaLoader::SqlExecuteTask::handle() {
	TimeStat ts;
  mysqlpp::StoreQueryResult res;
  set<long> origfids = fids_;
  set<long> mc_succ,mc_fail;
  try {
    Ice::Long idx = 0;
    if(!fids_.empty()){
      idx = *(fids_.begin());
    }
    TimeStat ts;
    res = QueryRunner(_dbInstance, CDbRServer, _pattern).store(_sql);
//    if(idx % 1000 == 1){
//      MCE_INFO("SqlExecuteTask::handle. sqlcost:" << ts.getTime());
//    }
  } catch (std::exception& e) {
    MCE_WARN("ParaLoader::SqlExecuteTask --> uid:" << uid_ << "err, " << e.what());
//    MCE_WARN("ParaLoader::SqlExecuteTask::handle --> query return null");
    _loader->finish(0);//读数据库失败，认为feed都存在
    return;
  }
  if (!res) {
    MCE_WARN("ParaLoader::SqlExecuteTask::handle --> query return null uid:" << uid_ );
    _loader->finish(0);//读数据库失败，认为feed都存在
    return;
  }
  //MCE_INFO("ParaLoader::SqlExecuteTask::handle -->1 cost:" << ts.getTime() << " rows:" << res.num_rows() << " expr:" << _pattern);
  FeedContentDict fdict;
  for (size_t i = 0; i < res.num_rows(); ++i) {
    mysqlpp::Row row = res.at(i);

    FeedContentPtr content = new FeedContent();
    content->data = new FeedData();

    content->data->feed = row["feed"];
    content->data->source = row["source"];
    content->data->actor = row["actor"];
    content->data->type = row["type"];
    content->data->xml = row["xml"].data();
    if(content->data->xml.empty()){
      MCE_INFO("SqlExecuteTask::handle. feed xml empty. uid:" << uid_ << " fid:" << content->data->feed);
      continue;
    }
    content->data->time = (time_t) mysqlpp::DateTime(row["time"]);

    content->reply = new FeedReply();
    content->reply->oldReplyId = Ice::Long(row["older_reply_id"]);
    content->reply->oldReply = row["older_reply"].data();
    content->reply->newReplyId = Ice::Long(row["newer_reply_id"]);
    content->reply->newReply = row["newer_reply"].data();
    content->reply->count = row["comment_count"];

    fdict[row["feed"]] = content;
    fids_.erase(content->data->feed);
//    TimeStat t;
//    ContentQueue::instance().Push(content->data->feed,content);
    if (!FeedMemcClient::instance().SetFeed(content)) {
      MCE_WARN("SqlExecuteTask::handle. set content " << content->data->feed << " to memcached fail! uid:" << uid_ );
      mc_fail.insert(content->data->feed);
    }else{
//      MCE_INFO("SqlExecuteTask::handle. set content " << content->data->feed << " to memcached success uid:" << uid_ );
      mc_succ.insert(content->data->feed);
    }

  }
  for (set<Ice::Long>::iterator i = fids_.begin(); i != fids_.end(); ++i) {
  	//不写空本体到Memcached,防止不写db的新鲜事用户过早的看不到
    TimeStat t;
    //FeedMemcClient::instance().SetEmptyFeed(*i);
    fdict[*i] = 0;//不可少!!!!!!!!!
//    MCE_INFO("SqlExecuteTask::handle. set content empty. id:" << *i << " cost:" << t.getTime() << " .not store mc");
  }

  MCE_INFO("SqlExecuteTask::handle. uid:" << uid_
  		<< " allcost:" << ts.getTime()
  		<< " request fids size:" << origfids.size() << " " << PrintHelper::Print(origfids)
			<< " null fids size:" << fids_.size() << " " << PrintHelper::Print(fids_)
			<< " mc_succ size:" << mc_succ.size() << " " <<  PrintHelper::Print(mc_succ)
			<< " mc_fail size:" << mc_fail.size() << " " << PrintHelper::Print(mc_fail)
  );
  _loader->finish(fdict, fids_.size());
}


MemcTask::MemcTask(const FeedContentDict & fdict){
  fdict_ = fdict;
}
void MemcTask::handle(){
  int exist = 0,not_exist = 0 ;
  for(FeedContentDict::iterator i = fdict_.begin(); i != fdict_.end(); ++i){
    if (i->second == 0) {

      if(!FeedMemcClient::instance().SetEmptyFeed(i->first)){
        MCE_WARN("MemcTask::handle. SetEmptyFeed fail. id:" << i->first);
      }
      ++not_exist;
    } else {
      if(!FeedMemcClient::instance().SetFeed(i->second)){
        MCE_WARN("MemcTask::handle. SetFeed fail. id:" << i->first);
      }
      ++exist;
    }
  }
//  MCE_INFO("MemcTask::run. exist:" << exist << " not exist:" << not_exist);
}
