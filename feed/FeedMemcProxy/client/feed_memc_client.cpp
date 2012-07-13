
#include "feed_memc_client.h"

#include <boost/lexical_cast.hpp>

#include "compress_wrap.h"
// #include "feedcontent.pb.h"
#include "FeedContent.pb.h"

#include "ServiceI.h"
#include "LogWrapper.h"

namespace xce {
namespace feed {

static char * kEmptyFeed = "EMPTY_FEED";
Ice::Long GetFeedId(const char * key, size_t len) {
  if (len < 5 || strncmp("FEED#", key, 5) != 0) {
    return -1;
  }

  Ice::Long id = -1;
  try {
    id = boost::lexical_cast<Ice::Long>(string(key + 5, len - 5));
  } catch(boost::bad_lexical_cast &) {
    return -1;
  }
  return id;
}

static void FeedContentPbToIce(const PbFeedContent & pb, FeedContentPtr content) {
  if (!content->data) {
    content->data = new FeedData();
  }

  content->data->feed = pb.data().feed();
  content->data->source = pb.data().source();
  content->data->actor = pb.data().actor();
  content->data->type = pb.data().type();
  content->data->time = pb.data().time();
  content->data->weight = pb.data().weight();
  content->data->xml = pb.data().xml();

  if (pb.has_reply()) {
    content->reply = new FeedReply();

    content->reply->oldReplyId = pb.reply().oldreplyid();
    content->reply->oldReply = pb.reply().oldreply();
    content->reply->newReplyId = pb.reply().newreplyid();
    content->reply->newReply = pb.reply().newreply();
    content->reply->count = pb.reply().count();
  }
}

FeedMemcClient::FeedMemcClient() {
  pool_.Init(250);
}

FeedMemcClient::~FeedMemcClient() {
}

FeedContentDict FeedMemcClient::GetFeedDict(const MyUtil::LongSeq& ids) {
  float cost_mem;
  float cost_decom;
  vector<string> feedids;
  for(size_t i=0; i<ids.size(); i++){
    char data[64] = {0};
    snprintf(data, 64, "FEED#%ld", ids.at(i));
    feedids.push_back(data);
  }
  FeedContentDict dict;
  pair<int, MemcachedClient*> cli = pool_.GetClient(ids.at(0)); 
  if(!cli.second){
    return FeedContentDict();
  }
  map<string, string> res;
  map<string, int> flags;
  TimeStat1 ts;
  bool succ = cli.second->Get(feedids, res, flags);
  //MCE_INFO("FeedMemcClient::GetFeedDict --> get end, " << cli.first << " " << res.size() << " " << flags.size());
  cost_mem = ts.getTime();
  //if(!succ){
  //  return FeedContentDict();
  //}
  ts.reset();
  map<string, string>::iterator it = res.begin();
  for(; it!=res.end(); ++it){
    Ice::Long fid = GetFeedId(it->first.c_str(), it->first.size());
    if(it->second == kEmptyFeed){
      dict.insert(make_pair(fid, FeedContentPtr(NULL)));
      continue;
    } 
    int flag = flags[it->first.c_str()];
    PbFeedContent pb;
    if(flag & 0x01) {
      string decompressed;
      CompressWrap::instance().Decompress(it->second.c_str(), &decompressed);
      pb.ParseFromArray(decompressed.data(), decompressed.size());
    } else {
       pb.ParseFromArray(it->second.c_str(), it->second.size());
    }

    FeedContentPtr content = new FeedContent();
    FeedContentPbToIce(pb, content);
    content->data->weight &= 0xFFFF0000;
    if(content->reply){
      content->data->weight |= (content->reply->count&0xFFFF);
    }
    dict.insert(make_pair(content->data->feed, content));
  }
  cost_decom = ts.getTime();
  string ip = cli.second->GetIp();
  pool_.ReleaseClient(cli.first, cli.second);
  MCE_INFO("FeedMemcClient::GetFeedDict --> ids:" << ids.size() << " res:" << dict.size() 
          << " cost_mem:" << cost_mem << " cost_par:" << cost_decom << " cost_total:" << (cost_mem+cost_decom)
          << " ip:" << ip);
  return dict;
}

FeedContentPtr FeedMemcClient::GetFeed(Ice::Long feed_id) {
  MyUtil::LongSeq ids;
  ids.push_back(feed_id);
  FeedContentDict dict = GetFeedDict(ids);
  if(dict.empty()){
    return 0;
  }
  return dict[feed_id];
}

// 返回值, 标识是否压缩了
bool FeedMemcClient::SerializeFeed(const FeedContentPtr& content, string * serialized) {
  if (!content) {
    return false;
  }
  PbFeedContent pb;
  // TODO : 哪些字段是optional的? 可以优化
  pb.mutable_data()->set_feed(content->data->feed);
  pb.mutable_data()->set_source(content->data->source);
  pb.mutable_data()->set_actor(content->data->actor);
  pb.mutable_data()->set_type(content->data->type);
  pb.mutable_data()->set_time(content->data->time);
  pb.mutable_data()->set_weight(content->data->weight);
  pb.mutable_data()->set_xml(content->data->xml);
  if(content->data->xml.empty()){
    return false;
  }
  if (content->reply) {
    pb.mutable_reply()->set_oldreplyid(content->reply->oldReplyId);
    pb.mutable_reply()->set_newreplyid(content->reply->newReplyId);
    pb.mutable_reply()->set_count(content->reply->count);
    pb.mutable_reply()->set_oldreply(content->reply->oldReply);
    pb.mutable_reply()->set_newreply(content->reply->newReply);
  }

  pb.SerializeToString(serialized);
  return true;
}

bool FeedMemcClient::SetEmptyFeed(Ice::Long id) {
  char key[64] = {0};
  snprintf(key, 64, "FEED#%ld", id);
  pair<int, MemcachedClient*> cli = pool_.GetClient(id); 
  bool res = cli.second->Set(key, kEmptyFeed, 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

bool FeedMemcClient::SetFeed(const FeedContentPtr& content) {
  char key[64];
  snprintf(key, 64, "FEED#%ld", content->data->feed);
  string value;
  bool b = SerializeFeed(content, &value);
  if(!b){
    return false;
  }
  pair<int, MemcachedClient*> cli = pool_.GetClient(content->data->feed); 
  bool res = cli.second->Set(key, value, 0);
  pool_.ReleaseClient(cli.first, cli.second);
  return res;
}

}
}

