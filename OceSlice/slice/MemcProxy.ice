#ifndef __MEMC_PROXY_ICE__
#define __MEMC_PROXY_ICE__

#include <Util.ice>
#include <RFeed.ice>

module xce
{
module feed
{

class MemcProxy {
  // 指定 upstream 服务器列表
  bool SetUpstreamList(string cfg);

  // 获取proxy的 memcache方式 监听端口
  string GetEndpoint();
};

class MemcFeedLoader {
  void LoadFeed(long id);
  void LoadFeedSeq(MyUtil::LongSeq id);

  void LoadFeedByKey(string key);
  void LoadFeedSeqByKey(MyUtil::StrSeq keys);

  MyUtil::Str2StrMap GetFeedSeqByKey(MyUtil::StrSeq keys);
};

class MemcFeedTest {
  FeedContent GetFeed(long id);
  FeedContentDict GetFeedDict(MyUtil::LongSeq ids);
};

};
};

#endif // __RFEED_ICE__
