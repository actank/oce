#ifndef XCE_FEED_EXPAND_FEED_H__
#define XCE_FEED_EXPAND_FEED_H__

#include <string>
#include "base/common.h"
#include "base/basictypes.h"

#include "feed/rfeed.h"
#include "oldslice/ILikeIt.h"

namespace cwf {
  class User;
}

namespace xce {

struct FeedXml;

// README.txt
struct WeiredType {
  unsigned type       : 24;
  unsigned reversed0  : 2;
  unsigned real_type  : 2;  
  unsigned mini_merge : 2;
  unsigned news_merge : 2;

  WeiredType(unsigned int v) {
    BOOST_STATIC_ASSERT(sizeof(WeiredType) == sizeof(int));
    *(unsigned int *)this = v;
  }
};

// 业务类别不同，模板不同 = template_index
// 使用枚举能防止错误,不要强转
enum WeiredSource {
  WS_HOME     = 0,
  WS_WAP      = 1,
  WS_UNKNOWN2 = 2,
  WS_UNKNOWN3 = 3,
  WS_MINIGROUP = 4,
  WS_OUT      = 5,
  WS_PAGETINY = 96,
  WS_PAGE     = 97,
  WS_WEBSLICE = 99,

  WS_FLAG = 0xFFFF,

  WS_MINIFEED = 0x10000,
  WS_HOT = 0x20000, 
  WS_LIVE = 0x40000, 
  WS_FAVORITE = 0x80000, 
};

struct FeedTypeEntry {
  int type; // feed type
  WeiredSource source; // 对应于数据库中的template index
  const char* filename;
};

extern FeedTypeEntry FeedTypeTable[];

// 
std::string GetTemplateFilename(int type, WeiredSource source, int version);

// index 为本条目的序号，统计用途

// 可以设定模板文件
bool Expand(const feed::FeedData * feed, FeedXml * fx
            , cwf::User * u, int owner
            , WeiredSource source
            , const ilikeit::LikeInfo* likeinfo
            , const std::string & template_file
            , int index
            , std::string * output
            , bool isAdmin = false);

// 使用 source = WS_HOME 的模板文件
bool Expand(const feed::FeedData * feed, cwf::User * u
            , int owner
            , WeiredSource source
            , const ilikeit::LikeInfo* likeinfo
            , int index
            , std::string * output
            , bool isAdmin = false);

// 测试用
bool RawExpand(int type, const char* raw_xml
               , const std::string & template_file
               , std::string * output);

struct ExpandResult {
  std::string err;
  std::string dump_tpl, dump_dict;
  std::string output;
};

bool RawExpand(const std::string& raw_xml, const std::string& ral_tpl
               , ExpandResult * result, bool dump = false);

// TODO: write an Emitter

}
#endif //XCE_FEED_EXPAND_FEED_H__
