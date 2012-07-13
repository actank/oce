#include "FeedLoaderI.h"

#include <boost/lexical_cast.hpp>

#include "QueryRunner.h"
#include "ServiceI.h"
#include "FeedMemcProxy/client/feed_memc_client.h"


using namespace xce::feed;

using namespace com::xiaonei::xce;
using namespace MyUtil;

class FeedContentDB {
public:
  static const string GetTableName(Ice::Long id) {
    ostringstream table;
    table<< "feed_content_" << id % 100;
    return table.str();
  }

  static const string GetInstance() {
    return "feed_db";
  }
};

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&MemcFeedLoaderI::instance(), service.createIdentity("M", ""));

  ThreadPoolConfig cfg(8, 64);
  TaskManager::instance().config(0, cfg);
}

static FeedContentPtr LoadFeedFromDB(Ice::Long id, bool * exist) {
  Statement sql;
  sql << "SELECT feed, source, actor, type, xml, time, older_reply_id, older_reply, "
    " newer_reply_id, newer_reply, comment_count from " << FeedContentDB::GetTableName(id) << " WHERE feed = " << id;

  FeedContentPtr content = NULL;
  *exist = true;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner(
        FeedContentDB::GetInstance(), CDbRServer,
        FeedContentDB::GetTableName(id)).store(sql);

    if (!res || res.num_rows() < 1) {
      *exist = false;
      MCE_WARN("load content " << id << " fail");
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
    content->data->time = (time_t)mysqlpp::DateTime(row["time"]);

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

  MCE_INFO("LoadFeedTask::handle - feed loaded from database. id=" << id_);

  if (!FeedMemcClient::instance().SetFeed(content)) {
    MCE_WARN("set content " << id_ << " to memcached fail!");
  }
}

MemcFeedLoaderI::MemcFeedLoaderI() : MyUtil::Timer(1000 * 60 * 3) {
  MyUtil::TaskManager::instance().scheduleRepeated(this);
}

void MemcFeedLoaderI::handle() {
  MCE_DEBUG("clear loaded set.");
  set<Ice::Long> empty;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    recent_loaded_ids_.swap(empty);
  }
}

void MemcFeedLoaderI::LoadFeedSeqByKey(const MyUtil::StrSeq & keys, const Ice::Current&) {
  for(size_t i = 0; i < keys.size(); ++ i) {
    if (strncmp("FEED#", keys[i].c_str(), 5) == 0) {
      Ice::Long id = boost::lexical_cast<Ice::Long>(keys[i].c_str() + 5);
      LoadFeed(id);
    }
  }
}

Str2StrMap MemcFeedLoaderI::GetFeedSeqByKey(const MyUtil::StrSeq & keys, const Ice::Current&) {
  Str2StrMap res;
  Ice::Long id = 0;
  
  timeval tv_start, tv_end;
  gettimeofday(&tv_start, NULL);


  for(size_t i = 0; i < keys.size(); ++ i) {
    if (strncmp("FEED#", keys[i].c_str(), 5) != 0) {
      continue;
    }

    try {
      id = boost::lexical_cast<Ice::Long>(keys[i].c_str() + 5);
    } catch(boost::bad_lexical_cast &) {
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

    float timeuse = 1000000 * (tv_end.tv_sec - tv_start.tv_sec) + tv_end.tv_usec - tv_start.tv_usec;
    timeuse /= 1000;
    if(timeuse >= 20) {
      MCE_WARN("11111111111 time : " << timeuse <<" ms. key count=" << keys.size());
    }

    string value;
    bool b = FeedMemcClient::instance().SerializeFeed(content, &value);
    FeedMemcClient::instance().SetMemcached(keys[i].c_str(), value, b ? 0x01 : 0);
    MCE_DEBUG("key add to memcached " << keys[i]);
    
    value.push_back(b ? 'c' : 'u'); // 山寨做法, 标识是否压缩
    res.insert(make_pair(keys[i], value));
  }

  gettimeofday(&tv_end, NULL);

  float timeuse = 1000000 * (tv_end.tv_sec - tv_start.tv_sec) + tv_end.tv_usec - tv_start.tv_usec;
  timeuse /= 1000;
  if(timeuse >= 20) {
    MCE_WARN("time : " << timeuse <<" ms. key count=" << keys.size());
  }

  return res;
}


void MemcFeedLoaderI::LoadFeedSeq(const Ice::LongSeq & ids, const Ice::Current&) {
  for(size_t i = 0; i < ids.size(); ++ i) {
    LoadFeed(ids[i]);
  }
}

void MemcFeedLoaderI::LoadFeedByKey(const string & key, const Ice::Current&) {
  if (strncmp("FEED#", key.c_str(), 5) == 0) {
    Ice::Long id = boost::lexical_cast<Ice::Long>(key.c_str() + 5);
    LoadFeed(id);
  }
}

void MemcFeedLoaderI::LoadFeed(Ice::Long id, const Ice::Current&) {
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
};

