
#include <stdio.h>
#ifdef WIN32
#define snprintf _snprintf
#endif
#include <boost/algorithm/string/predicate.hpp>       // for starts_with
#include <boost/smart_ptr/scoped_ptr.hpp>

#include "base/common.h"
#include "base/logging.h"
#include "base/ptime.h"
#include "base/globalinit.h"
#include "base/startuplist.h"

#include "cwf/action.h"
#include "cwf/frame.h"
#include "arch/site_xiaonei.h"

#include "feed/rfeed_adapter.h"
#include "feed/feedview/expandfeed.h"
#include "feed/ilikeit_adapter.h"
#include "feed/feedview/headurl.h"

namespace xce {

static const std::string kDefaultContentType("text/html; charset=utf-8");

struct MiniGroupAction : public cwf::BaseAction {
  bool Match(const std::string& url) const;
  cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
};

bool MiniGroupAction::Match(const std::string& url) const {
  return boost::starts_with(url, "/qun.do"); 
}

cwf::HttpStatusCode MiniGroupAction::Process(cwf::Request * req, cwf::Response * res) {
  boost::scoped_ptr<cwf::User> u(Authorize(req));
  if (!u) {
    return cwf::HC_UNAUTHORIZED;
  }

  const int type = req->form("f", -1);
  const int page = req->get("p", 0);
  const long gid = req->get("gid", 0);
  const int isAdmin = req->get("isAdmin", 0);
  const int count = 20;

  // header
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
  res->OutputHeader();

  //feed::FeedDataResPtr feed_res;
  feed::FeedDataSeq feed_seq;
  try {
    ptime pt("getfeed", false, 30);

    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);

    LOG_F(WARNING) << "minigroup feed get. type=" << type;

    if (type <= 0) {
      //feed_res = fprx->GetHotFeedData(u->id(), page * count, count);
      
      LOG(INFO) << "GetFeedData begin. gid:" << gid << " uid:" << u->id() << " begin:" << page*count << " count:" << count;
      feed_seq = fprx->GetFeedData(gid, u->id(), page * count, count);
      LOG(INFO) << "GetFeedData end.";
    } else {

      LOG(INFO) << "GetFeedDataByStype begin.";
      std::vector<int> types;
      if (5 == type) {
        types.push_back(502);
        //feed_res = fprx->GetHotFeedDataByStype(u->id(), types, page * count, count);
        feed_seq = fprx->GetFeedDataByStype(gid, u->id(), types, page * count, count);
      } else {
        types.push_back(type * 100);
        //feed_res = fprx->GetHotFeedDataByType(u->id(), types, page * count, count);
        feed_seq = fprx->GetFeedDataByType(gid, u->id(), types, page * count, count);
      }
      LOG(INFO) << "GetFeedDataByStype end.";
    }

  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "FeedGroupAdapter error : " << e;
  }


  LOG(INFO) << "feed_seq size:" << feed_seq.size();

  /*
  if (feed_seq.empty()) {
    return cwf::HC_SERVICE_UNAVAILABLE;
  }
  */

  int feed_count = 0; // 实际输出的 Feed条目数

  // 得到“赞” ilike 信息
  ::xce::ilikeit::LikeInfoSeq likeseq;
  std::vector<char> vbool(0, feed_seq.size()); // 有些类型并不需要 zan
  bool likeinfo_avaible = false;
#if 1//POSIX
  {
    ptime pt("buildzan", false, 30);
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
      likeinfo_avaible = BuildZan(u->id(), globalseq, &likeseq);
    }
  }
#endif

  {
    LOG(INFO) << "expand.";
    ptime pt("expand", false, 30);
    std::string stat;
    for (size_t i=0,likeidx = 0; i<feed_seq.size(); ++i) {
      std::string output;

      const ilikeit::LikeInfo* likeinfo = 0;
      if (likeinfo_avaible && vbool[i])
        likeinfo = likeseq[likeidx++].get();
      else
        likeinfo = 0;

      std::string stat_log;
      bool f = Expand(feed_seq[i].get(), u.get(), 0, WeiredSource(WS_MINIFEED | WS_MINIGROUP), likeinfo
        , page * count + i, &output, stat_log, isAdmin );
      if (f) {
        ++ feed_count;
        res->WriteRaw(output);
        stat += stat_log;
      }
    }

    try {
      xce::feed::FeedStatLogPrx prx = CachedFeedStatLogAdapter(u->id());
      LOG(INFO) << "prx:" << prx;
      if (!prx) {
        LOG(INFO) << "mini_group_action.cc::Process --> FeedStatLog prx is NULL !";
      } else {
        std::vector<std::string> cate;
        cate.push_back("feed");
        //cate.push_back("stat");
        cate.push_back("minigroup");
        prx->AddLog(cate, stat);
      }
    } catch (Ice::Exception& e) {
      LOG_F(WARNING) << e;
    }

  }

  LOG(VERBOSE) << "p: " << page << " seq: " << feed_seq.size()
    << " expand: " << feed_count;

  // 最后添加一个如下字符串，为js服务，否则不会添加进列表
  // 第一页 ##@L#1##@L#20##@L#20
  // 第二页 ##@L#21##@L#40##@L#20
  char buf[100];
  snprintf(buf, sizeof(buf), "##@L#%d##@L#%d##@L#%d"
    , 1 + page * count, page * count + feed_count, feed_count);
  res->WriteRaw(buf);
  return cwf::HC_OK;
}

static void InitMiniGroupAction() {
  // LOG(INFO) << "Register MiniGroupAction";
  cwf::FrameWork::RegisterAction(new MiniGroupAction);

#ifdef WIN32
  return;
#endif

  try {
    xce::Adapter::feed::ResetCached<feed::FeedGroupManagerPrx>("M@FeedGroup", 10);
  } catch(std::exception & e) {
    LOG(INFO) << "feed group manager prx reset error " << e.what();
  }

  HeadUrl::FromPart("");
}

GLOBAL_INIT(CWF_TYPE_ACTION, {
  AddStartup(&InitMiniGroupAction);
});

}
