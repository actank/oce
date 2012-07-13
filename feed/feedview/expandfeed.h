#ifndef XCE_FEED_EXPAND_FEED_H__
#define XCE_FEED_EXPAND_FEED_H__

#include <string>
#include "feed/base_feed/common.h"
#include "feed/base_feed/basictypes.h"

#include "RFeed.h"
#include "IceLogger.h"
#include "ILikeIt.h"
#include "Notify/util/clear_silver_wrap.h"
#include "OceCxxAdapter/src/VipIconDataReaderAdapter.h"

namespace cwf {
  class User;
}

namespace xce {

struct FeedXml;

// README.txt
struct WeiredType {
  unsigned type       : 16;
  unsigned version    : 8;
  unsigned reversed0  : 2;
  unsigned real_type  : 2;  
  unsigned mini_merge : 2;
  unsigned news_merge : 2;

  WeiredType(unsigned int v) {
    BOOST_STATIC_ASSERT(sizeof(WeiredType) == sizeof(int));
    //MCE_INFO("wt:" << v);
    //*(unsigned int *)this = v;
     type = v&0xffff;
     v >>= 16;
     version = v & 0xff;
     v >>= 8;
     reversed0 = v & 0x3;
     v >>= 2;
     real_type = v & 0x3;
     v >>= 2;
     mini_merge = v & 0x3;
     v >>= 2;
     news_merge = v & 0x3;
    // MCE_INFO("type:" << type << "  v:" << v << " this:" << *(unsigned int *)this);
 //   MCE_INFO("type=" << type << " this=" << *(unsigned int *)this << " version=" << version);
  }
};

// 业务类别不同，模板不同 = template_index
// 使用枚举能防止错误,不要强转
enum WeiredSource {
  WS_HOME     = 0,
  WS_IM       = 1,
  WS_WAP      = 2,
  WS_UNKNOWN2 = 2,
  WS_PAGER    = 4,
  WS_UNKNOWN3 = 4,
  WS_APP      = 8,
  WS_MINIGROUP = 16,

  WS_OUT      = 32,
  WS_PAGETINY = 64,
  WS_PAGE     = 128,
  WS_WEBSLICE = 256,

  WS_FLAG = 0xFFFF,

  WS_MINIFEED = 0x10000,
  WS_HOT = 0x20000, 
  WS_LIVE = 0x40000, 
  WS_FAVORITE = 0x80000, 
};
/*
// 业务类别不同，模板不同 = template_index
// 使用枚举能防止错误,不要强转
enum WeiredSource {
  WS_HOME     = 0,
  WS_IM       = 1,
  WS_WAP      = 2,
  WS_UNKNOWN2 = 2,
  WS_PAGER    = 3,
  WS_UNKNOWN3 = 3,
  WS_APP      = 4,
  WS_MINIGROUP = 5,

  WS_OUT      = 95,
  WS_PAGETINY = 96,
  WS_PAGE     = 97,
  WS_WEBSLICE = 99,

  WS_FLAG = 0xFFFF,

  WS_MINIFEED = 0x10000,
  WS_HOT = 0x20000, 
  WS_LIVE = 0x40000, 
  WS_FAVORITE = 0x80000, 
};
*/
struct FeedTypeEntry {
  int type; // feed type
  WeiredSource source; // 对应于数据库中的template index
  const char* filename;
};

extern FeedTypeEntry FeedTypeTable[];

std::string GetTemplateFilename(int type, WeiredSource source, int version);

class FeedTplCache : public xce::notify::CSTemplateCache {
 public:
  static FeedTplCache & Instance() {
     static FeedTplCache instance;
     return instance;
  }

  virtual bool Reload(int is_test);
  virtual ~FeedTplCache() {}
 private:
  FeedTplCache(){}
  FeedTplCache(const FeedTplCache &);
};

// index 为本条目的序号，统计用途

// 可以设定模板文件
/*
bool ExpandFeedXml(const feed::FeedData * feed, FeedXml * fx
            , cwf::User * u, int owner
            , WeiredSource source
            , const ilikeit::LikeInfo* likeinfo
            , const std::string & template_file
            , int index
            , std::string * output
            , bool isXML
            , std::string logic_str
            , bool isAdmin = false);
*/

// 使用 source = WS_HOME 的模板文件
bool ExpandFeedData(const feed::FeedData * feed, cwf::User * u
            , int owner
            , WeiredSource source
            , const ilikeit::LikeInfo* likeinfo
            , int index
            , const vipinfo::VipFeedDataSeq & vipInfo
            , std::string * output
            , std::string & stat_log 
            , bool isAdmin = false
            , int view = 0);

}
#endif //XCE_FEED_EXPAND_FEED_H__
