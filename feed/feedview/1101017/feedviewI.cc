#include "feed/feedviewI.h"

#include "base/logging.h"
#include "base/ptime.h"
#include "base/pcount.h"
#include "base/startuplist.h"

#include "feed/rfeed_adapter.h"
#include "feed/expandfeed.h"
#include "feed/ilikeit_adapter.h"
#include "cwf/site.h"
#include "cwf/frame.h"

#include "arch/service.h"

namespace xce {

XAR_IMPL(News);
XAR_IMPL(NewsT);
XAR_IMPL(NewsS);

XAR_IMPL(Mini);
XAR_IMPL(MiniT);
XAR_IMPL(MiniS);

XAR_IMPL(FeedErr);      // exception from FeedItem
XAR_IMPL(ExpandGT10);   // 模板expand超过 10ms

XAR_IMPL(FeedGT30);     // feednews 调用超过 30 ms
XAR_IMPL(LikeGT3);      // ilike 调用超过 3ms

XAR_IMPL(FocusGT30);      // focus/favorite 调用超过 3ms

static feed::HtmlResult kEmptyResult;

int Expand(Ice::Int owner, Ice::Int uid, const std::string& name, Ice::Int render
    , feed::FeedDataSeq feed_seq, Ice::Int from, feed::HtmlResult * html, bool isAdmin = false) {
  ASSERT(!feed_seq.empty() && html);

  html->reserve(feed_seq.size());

  int feed_count = 0; // 实际输出的 Feed条目数

  // 得到“赞” ilike 信息
  ::xce::ilikeit::LikeInfoSeq likeseq;
  std::vector<char> vbool(0, feed_seq.size()); // 有些类型并不需要 zan
  bool likeinfo_avaible = false;
  {
    ptime pt("buildzan", false);
    ::xce::ilikeit::GlobalIdSeq globalseq;
    for (size_t i=0; i<feed_seq.size(); ++i) {
      feed::FeedData* feed = feed_seq[i].get();
      if (BuildGlobalId(feed->source, feed->type, &globalseq))
        vbool.push_back(true);
      else
        vbool.push_back(false);
    }

    if (!globalseq.empty()) {
      // try catch inside
      likeinfo_avaible = BuildZan(uid, globalseq, &likeseq);
    }

    if (pt.wall_clock() > 3)
      XAR_INC(LikeGT3);
  }

  cwf::User u(name, uid);
  {
    ptime pt("expand", false);
    for (size_t i=0,likeidx = 0; i<feed_seq.size(); ++i) {
      const ilikeit::LikeInfo* likeinfo = 0;
      if (vbool[i] && likeinfo_avaible)
        likeinfo = likeseq[likeidx++].get();
      else
        likeinfo = 0;

      // 这儿render被强转了
      std::string output;
      bool f = Expand(feed_seq[i].get(), &u, owner, (WeiredSource)render, likeinfo, from, &output, isAdmin);
      if (f) {
        ++ feed_count;
        ++ from;
        html->push_back(output);
      }
    }

    // ASSERT(feed_count == feed_seq.size());

    if (pt.wall_clock() > 10)
      XAR_INC(ExpandGT10);
  }
  return feed_count;
}

feed::HtmlResult FeedViewI::Get(Ice::Int uid, const std::string& name, Ice::Int render
      , Ice::Int from, Ice::Int count, const Ice::Current& current) {
  XAR_INC(News);

  //if (uid % 10 == 2 || uid % 10 == 1 || uid % 10 == 9 || uid % 10 == 3 || uid % 10 == 4 || uid % 10 == 0) {
    return GetHotByType(uid, name , render, feed::TypeSeq(), from, count, current);
  //}

  //return GetByType(uid, name, render, feed::TypeSeq(), from, count, current);
}

feed::HtmlResult FeedViewI::GetByType(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types
    , Ice::Int from, Ice::Int count, const Ice::Current& cur) {

  XAR_INC(NewsT); 
  feed::FeedDataSeq feed_seq;

  //LOG(INFO) << "connection: " << cur.con->toString();  
  try {
    ptime pt("getFeed", false);
    feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(uid);

    LOG(VERBOSE) << "GetByType: uid: " << uid << " proxy: " << fprx;

    if (types.empty()) {
      feed_seq = fprx->getFeedData(uid, from, count);
    }
    else {
      feed_seq = fprx->getFeedDataByType(uid, types, from, count);
    }
    // ASSERT(feed_seq.size() == count);

    if (pt.wall_clock() > 30)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << e;
  }

  LOG(INFO) << "GetByType() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq.size() << '/' << count;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render, feed_seq, from, &ret);

  LOG(LS_INFO) << "Expand : uid=" << uid << " len=" << ret.size();
  return ret;
}

feed::HtmlResult FeedViewI::GetByStype(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types
    , Ice::Int from, Ice::Int count, const Ice::Current& cur) {
  XAR_INC(NewsS);

  feed::FeedDataSeq feed_seq;
  //LOG(INFO) << "connection: " << cur.con->toString(); 
  try {
    ptime pt("getFeedByStype", false);
    feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(uid);
    feed_seq = fprx->getFeedDataByStype(uid, types, from, count);

    if (pt.wall_clock() > 3)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << e;
  }

  LOG(INFO) << "GetByStype() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq.size() << '/' << count;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render, feed_seq, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetByIds(Ice::Int uid, const std::string& name
    , const feed::FeedIdSeq & fids
    , const Ice::Current&) {
  return GetByIds2(uid, name, 0, fids);
}

feed::HtmlResult FeedViewI::GetByIds2(Ice::Int uid, const std::string& name, Ice::Int render
    , const feed::FeedIdSeq & fids
    , const Ice::Current& cur) {
  //////////////
  LOG(INFO) << "GetByIds2: " << uid << " " << name << " " << render;
////////////////
  render = 0;
  //LOG(INFO) << "connection: " ;
  feed::FeedDataSeq feed_seq;

  try {
    ptime pt("getFeedById", false);
    //LOG(INFO) << "before fprx.";
   // feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(uid);
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    //LOG(INFO) << "fprx: " << fprx;
    feed_seq = fprx->GetFeedDataByIds(fids);
    //LOG(INFO) << "feed_seq.size:" << feed_seq.size();

    if (pt.wall_clock() > 3)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << e;
  }

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  if (!(render & WS_MINIFEED)) {
    render |= WS_HOME;
  }
  //LOG(INFO) << "before expand.";
  Expand(0, uid, name, render, feed_seq, 0, &ret);
  //LOG(INFO) << "ret size:" << ret.size();
  return ret;
}


feed::HtmlResult FeedViewI::GetByIdsAndStype(Ice::Int uid, const std::string& name
    , const feed::FeedIdSeq & fids, int stype, long firstID
    , const Ice::Current&) {
  return GetByIdsAndStype2(uid, name, 0, fids, stype, firstID);
}

feed::HtmlResult FeedViewI::GetByIdsAndStype2(Ice::Int uid, const std::string& name, Ice::Int render
    , const feed::FeedIdSeq & fids, int stype, long firstID
    , const Ice::Current& cur) {
  //////////////
  LOG(INFO) << "GetByIdsAndStype2: " << uid << " " << name << " " << render << " " << stype << " " << firstID;
////////////////
  render = 0;
  //LOG(INFO) << "connection: " ;
  feed::FeedDataSeq feed_seq;

  try {
    ptime pt("getFeedById", false);
    //LOG(INFO) << "before fprx.";
   // feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(uid);
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    //feed::FeedNewsManagerPrx fprx = CachedFeedNews3Adapter(uid);
    LOG(INFO) << "fprx: " << fprx;
    feed_seq = fprx->GetFeedDataByIdsWithUid(uid, fids, firstID, stype);
 
    LOG(INFO) << "feed_seq.size:" << feed_seq.size() << " firstID:" << firstID << " stype:" << stype;

    if (pt.wall_clock() > 3)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << e;
  }

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  if (!(render & WS_MINIFEED)) {
    render |= WS_HOME;
  }
  //LOG(INFO) << "before expand.";
  Expand(0, uid, name, render, feed_seq, 0, &ret);
  //LOG(INFO) << "ret size:" << ret.size();
  return ret;
}


feed::HtmlResult FeedViewI::GetMini(Ice::Int owner, Ice::Int uid, const std::string& name, Ice::Int render
      , Ice::Int from, Ice::Int count, const Ice::Current& current) {
  XAR_INC(Mini);
  return GetMiniByType(owner, uid, name, render|WS_MINIFEED, feed::TypeSeq()
    , from, count, current);
}

feed::HtmlResult FeedViewI::GetMiniByType(Ice::Int owner, Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types
    , Ice::Int from, Ice::Int count, const Ice::Current&) {
  XAR_INC(MiniT);

  feed::FeedDataSeq feed_seq;
  
  try {
    ptime pt("getFeed", false);
    feed::FeedItemManagerPrx fprx = CachedFeedMiniAdapter(owner);
    //feed::FeedItemManagerPrx fprx = CachedFeedMiniAdapter2(owner);

    LOG(VERBOSE) << "owner: " << owner << " uid: " << uid << " proxy: " << fprx;

    std::vector<int> types;
    if (types.empty()) {
      feed_seq = fprx->getFeedData(owner, from, count);
    }
    else {
      feed_seq = fprx->getFeedDataByType(owner, types, from, count);
    }

    if (pt.wall_clock() > 3)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << e;
  }

  LOG(INFO) << "GetMiniByType() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq.size() << '/' << count;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(owner, uid, name, render|WS_MINIFEED, feed_seq, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniByStype(Ice::Int owner, Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types
    , Ice::Int from, Ice::Int count, const Ice::Current&) {
  XAR_INC(MiniS);

  feed::FeedDataSeq feed_seq;
  
  try {
    ptime pt("getFeedByStype", false);
    feed::FeedItemManagerPrx fprx = CachedFeedMiniAdapter(owner);
    feed_seq = fprx->getFeedDataByStype(owner, types, from, count);

    if (pt.wall_clock() > 3)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << "GetMiniByStype() uid=" << uid << " error:" << e;
  }

  LOG(INFO) << "GetMiniByStype() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq.size() << '/' << count;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(owner, uid, name, render|WS_MINIFEED, feed_seq, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetFavorite(Ice::Int uid, const std::string& name
    , Ice::Int render, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataSeq feed_seq;
  
  try {
    ptime pt("getFeed", false);
    feed::FocusManagerPrx fprx = CachedFeedFocusAdapter(uid);

    feed_seq = fprx->GetFocusFeedData(uid, from, count);

    if (pt.wall_clock() > 30)
      XAR_INC(FocusGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << "GetFavorite() uid=" << uid << " error:" << e;
  }

  LOG(INFO) << "GetFavorite() uid=" << uid << " count=" << feed_seq.size() << '/' << count;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render | WS_FAVORITE, feed_seq, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetHot(Ice::Int uid, const std::string& name
    , Ice::Int render, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataResPtr feed_seq;
  
  try {
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
     //feed::FeedNewsManagerPrx fprx = CachedFeedNews3Adapter(uid);
    feed_seq = fprx->GetHotFeedData(uid, from, count);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetHot() uid=" << uid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetHot() uid=" << uid << " count=" << feed_seq->data.size() << '/' << count;

  if (feed_seq->data.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render | WS_HOT, feed_seq->data, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetHotByType(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types, Ice::Int from, Ice::Int count, const Ice::Current& cur) {

  feed::FeedDataResPtr feed_seq;
  
  try {

    //feed::FeedNewsManagerPrx fprx = CachedFeedNews3Adapter(uid);
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    feed_seq = fprx->GetHotFeedDataByType(uid, types, from, count);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetHotByType() uid=" << uid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetHotByType() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq->data.size() << '/' << count;

  if (feed_seq->data.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render | WS_HOT, feed_seq->data, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetHotByStype(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types, Ice::Int from, Ice::Int count, const Ice::Current&) {

  feed::FeedDataResPtr feed_seq;
  
  try {
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    feed_seq = fprx->GetHotFeedDataByStype(uid, types, from, count);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetHotByStype() uid=" << uid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetHotByStype() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq->data.size() << '/' << count;

  if (feed_seq->data.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render | WS_HOT, feed_seq->data, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetLive(Ice::Int uid, const std::string& name
    , Ice::Int render, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataResPtr feed_seq;
  
  try {
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    feed_seq = fprx->GetLiveFeedData(uid, from, count);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetLive() uid=" << uid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetLive() uid=" << uid << " count=" << feed_seq->data.size() << '/' << count;

  if (feed_seq->data.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render | WS_LIVE, feed_seq->data, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetLiveByType(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataResPtr feed_seq;
  
  try {
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    feed_seq = fprx->GetLiveFeedDataByType(uid, types, from, count);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetLiveByType() uid=" << uid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetLiveByType() uid=" << uid << " count=" << feed_seq->data.size() << '/' << count;

  if (feed_seq->data.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render | WS_LIVE, feed_seq->data, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetLiveByStype(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& stypes, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataResPtr feed_seq;
  
  try {
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    feed_seq = fprx->GetLiveFeedDataByStype(uid, stypes, from, count);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetLiveByStype() uid=" << uid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetLiveByStype() uid=" << uid << " count=" << feed_seq->data.size() << '/' << count;

  if (feed_seq->data.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(0, uid, name, render | WS_LIVE, feed_seq->data, from, &ret);
  return ret;
}
feed::HtmlResult FeedViewI::GetMiniGroupFeedData(Ice::Int gid, const std::string& name,  Ice::Int render, Ice::Int memberid, Ice::Int begin, Ice::Int limit, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedData(gid, memberid, begin, limit);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetMiniGroupFeedData() gid=" << gid << " memberid:" << memberid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetMiniGroupFeedData() gid=" << gid << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(gid, memberid, name, render | WS_MINIFEED, feed_seq, begin, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniGroupFeedDataByType(Ice::Int gid, const std::string& name,  Ice::Int render,  Ice::Int memberid, const ::xce::feed::IntSeq& stypes, Ice::Int begin, Ice::Int limit, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedDataByType(gid, memberid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetMiniGroupFeedDataByType() gid=" << gid  << " memberid:" << memberid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetMiniGroupFeedDataByType() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(gid, memberid, name, render | WS_MINIFEED, feed_seq, begin, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniGroupFeedDataByStype(Ice::Int gid, const std::string& name, Ice::Int render, Ice::Int memberid, const ::xce::feed::IntSeq& stypes, Ice::Int begin, Ice::Int limit, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedDataByStype(gid, memberid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetMiniGroupFeedDataBySType() gid=" << gid  << " memberid:" << memberid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetMiniGroupFeedDataBySType() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(gid, memberid, name, render | WS_MINIFEED, feed_seq, begin, &ret);
  return ret;
}





feed::HtmlResult FeedViewI::GetMiniGroupFeedData2(Ice::Int gid, const std::string& name,  Ice::Int render, Ice::Int memberid, Ice::Int begin, Ice::Int limit, bool isAdmin, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedData(gid, memberid, begin, limit);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetMiniGroupFeedData2() gid=" << gid << " memberid:" << memberid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetMiniGroupFeedData2() gid=" << gid << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(gid, memberid, name, render | WS_MINIFEED| WS_MINIGROUP, feed_seq, begin, &ret, isAdmin);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniGroupFeedDataByType2(Ice::Int gid, const std::string& name,  Ice::Int render,  Ice::Int memberid, const ::xce::feed::IntSeq& stypes, Ice::Int begin, Ice::Int limit, bool isAdmin, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedDataByType(gid, memberid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetMiniGroupFeedDataByType2() gid=" << gid  << " memberid:" << memberid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetMiniGroupFeedDataByType2() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(gid, memberid, name, render | WS_MINIFEED| WS_MINIGROUP, feed_seq, begin, &ret, isAdmin);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniGroupFeedDataByStype2(Ice::Int gid, const std::string& name, Ice::Int render, Ice::Int memberid, const ::xce::feed::IntSeq& stypes, Ice::Int begin, Ice::Int limit, bool isAdmin, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedDataByStype(gid, memberid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetMiniGroupFeedDataBySType2() gid=" << gid  << " memberid:" << memberid << " error:" << e;
    return kEmptyResult;
  }

  LOG(INFO) << "GetMiniGroupFeedDataBySType2() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin;

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  Expand(gid, memberid, name, render | WS_MINIFEED | WS_MINIGROUP, feed_seq, begin, &ret, isAdmin);
  return ret;
}




}

BEGIN_SERVANT_MAP("feedview")
  SERVANT_ENTRY(feedview, &xce::ServantFactory< xce::FeedViewI >::CreateServant, &xce::ServantFactory< xce::FeedViewI >::Destroy)
END_SERVANT_MAP()

// 静态函数可能添加不上，需要main内部再显示添加一次
static void Init() {
  cwf::FrameWork::InitTemplate();
}

GLOBAL_INIT(FEEDVIEWI, {
  xce::AddStartup(&Init, true);
});
