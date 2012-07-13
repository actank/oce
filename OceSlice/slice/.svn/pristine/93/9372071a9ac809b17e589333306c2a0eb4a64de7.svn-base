#ifndef __MEMC_PROXY_ICE__
#define __MEMC_PROXY_ICE__

#include <Util.ice>
#include <RFeed.ice>

module xce
{
module feed
{

class ProxyInfo{
  string  ip;
  int     port;
  int     connections;
};

class FeedMemcProxy {
  // 指定 upstream 服务器列表
  bool SetUpstreamList(string cfg);

  // 获取proxy的 memcache方式 监听端口
  string GetEndpoint();
  ProxyInfo GetProxyInfo();
};

class FeedMemcLoader {
  void LoadFeed(long id);
  void LoadFeedSeq(MyUtil::LongSeq id);

  void LoadFeedByKey(string key);
  void LoadFeedSeqByKey(MyUtil::StrSeq keys);

  MyUtil::Str2StrMap GetFeedSeqByKey(MyUtil::StrSeq keys);
  
  FeedContentDict GetFeedDict(MyUtil::LongSeq ids);
  FeedContentDict GetFeedDictTimeout(int uid,MyUtil::LongSeq fids,int timeout);
};

class FeedMemcContent {
  FeedContent GetFeed(long id);
  FeedContentDict GetFeedDict(MyUtil::LongSeq ids);
  FeedDataSeq GetFeedDataByIds(int uid,MyUtil::LongSeq ids);
  //FeedContentDict LocateFeedDict(int uid,MyUtil::LongSeq fids);
  void  Test(MyUtil::LongSeq ids);
  void  Test2(MyUtil::IntSeq ids);
};

};
};

#endif // __RFEED_ICE__
