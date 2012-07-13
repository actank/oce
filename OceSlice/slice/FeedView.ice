#ifndef XCE_FEED_FEEDVIEW_SLICE__
#define XCE_FEED_FEEDVIEW_SLICE__
#include <RFeed.ice>
module xce {
module feed {

// 业务大类型
const int TypeSHARE   = 100;
const int TypeSTATUS  = 500;
const int TypeBLOG    = 600;
const int TypePHOTO   = 700;
const int TypePAGE    = 2000;

sequence<int> TypeSeq;
sequence<int> IntSeq;
// 渲染类型 = 模板类型 = 数据库中的 template_index
const int RenderHOME     = 0;
const int RenderWAP      = 1;
const int RenderUNKNOWN2 = 2;
const int RenderUNKNOWN3 = 3;
const int RenderUNKNOWN4 = 4;
const int RenderOUT      = 5;
const int RenderPAGETINY = 96;
const int RenderPAGE     = 97;
const int RenderWEBSLICE = 99;

sequence<long> FeedIdSeq;

// 1 对于返回了多少条数据，还是很有价值的
// 2 对于 out 参数，铁安担心Java程序员接受起来有些困难
sequence<string> HtmlResult;

struct CSTplCheckRes {
  int code;       // 0 表示合法
	string reason;	// 错误描述信息
};

interface FeedView {
  // uid, name 始终为访问者的
  // 由调用者传入name，可以减少一次远程调用
  HtmlResult Get(int uid, string name, int render, int from, int count);
  HtmlResult GetByType(int uid, string name, int render, TypeSeq types, int from, int count);
  HtmlResult GetByStype(int uid, string name, int render, TypeSeq stypes, int from, int count);

  HtmlResult GetByIds(int uid, string name, FeedIdSeq fids);
  HtmlResult GetByIds2(int uid, string name, int render, FeedIdSeq fids);
  FeedDataSeq GetByIds2View(int uid, string name, int render, FeedIdSeq fids, int view);
  
  HtmlResult GetByIdsAndStype(int uid, string name, FeedIdSeq fids, int stype, long firstID);
  HtmlResult GetByIdsAndStype2(int uid, string name, int render, FeedIdSeq fids, int stype, long firstID);

  // owner 为被访问者 id
  HtmlResult GetMini(int owner, int uid, string name, int render, int from, int count);
  HtmlResult GetMiniByType(int owner, int uid, string name, int render, TypeSeq types, int from, int count);
  HtmlResult GetMiniByStype(int owner, int uid, string name, int render, TypeSeq stypes, int from, int count);

  HtmlResult GetFavorite(int uid, string name, int render, int from, int count);

  HtmlResult GetHot(int uid, string name, int render, int from, int count);
  HtmlResult GetHotByType(int uid, string name, int render, TypeSeq types, int begin, int limit);
  HtmlResult GetHotByStype(int uid, string name, int render, TypeSeq stypes, int begin, int limit);

  HtmlResult GetLive(int uid, string name, int render, int from, int count);
  HtmlResult GetLiveByType(int uid, string name, int render, TypeSeq types, int begin, int limit);
  HtmlResult GetLiveByStype(int uid, string name, int render, TypeSeq stypes, int begin, int limit);

  HtmlResult GetMiniGroupFeedData(int gid, string name, int render, int memberid, int begin, int limit);
  HtmlResult GetMiniGroupFeedDataByType(int gid, string name, int render, int memberid, IntSeq stypes, int begin, int limit);
  HtmlResult GetMiniGroupFeedDataByStype(int gid, string name, int render, int memberid, IntSeq stypes, int begin, int limit);

  HtmlResult GetMiniGroupFeedData2(int gid, string name, int render, int memberid, int begin, int limit, bool isAdmin);
  HtmlResult GetMiniGroupFeedData2CS(int gid, string name, int render, int memberid, int begin, int limit, bool isAdmin, int view);
  HtmlResult GetMiniGroupFeedDataByType2(int gid, string name, int render, int memberid, IntSeq stypes, int begin, int limit, bool isAdmin);
  HtmlResult GetMiniGroupFeedDataByStype2(int gid, string name, int render, int memberid, IntSeq stypes, int begin, int limit, bool isAdmin);

  void Test(int uid, string name, int render, int begin, int limit);

  bool ReloadCSTemplate();
  bool ReloadDataSchema();
  CSTplCheckRes CheckCSTemplate(string tpl);

  FeedDataRes GetHotDataByStype(int uid, string name, int render, TypeSeq stypes, int begin, int limit);
  FeedDataRes GetLiveDataByStype(int uid, string name, int render, TypeSeq stypes, int begin, int limit);
	FeedDataSeq GetMiniDataByStype(int uid, string name, int render, TypeSeq stypes, int begin, int limit);

  FeedDataRes GetHotDataByStype2(int uid, string name, int render, TypeSeq stypes, int begin, int limit, int view);
  FeedDataRes GetLiveDataByStype2(int uid, string name, int render, TypeSeq stypes, int begin, int limit, int view);
  FeedDataSeq GetMiniDataByStype2(int uid, string name, int render, TypeSeq stypes, int begin, int limit, int view);
  FeedDataSeq GetFavorite2(int uid, string name, int render, int begin, int limit, int view);
  FeedDataSeq GetFavorite2ByStype(int uid, string name, int render, TypeSeq stypes, int begin, int limit, int view);

  FeedDataSeq GetGreaterThanFeedidWithUseridByType(int appid, long feedid, int limit, int uid, TypeSeq types, int render, int view);
};

};
};
#endif // XCE_FEED_FEEDVIEW_SLICE__
