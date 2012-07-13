#include "feed/cwf_action/feedtypeaction.h"

#include <stdio.h>
#ifdef WIN32
#define snprintf _snprintf
#endif
#include <boost/algorithm/string/predicate.hpp>       // for starts_with
#include <boost/smart_ptr/scoped_ptr.hpp>

#include "base/common.h"
//#include "base/logging.h"
#include "base/ptime.h"
#include "base/globalinit.h"
#include "base/startuplist.h"

#include "cwf/frame.h"
#include "arch/site_xiaonei.h"

#include "LogWrapper.h"
#include "unistd.h"

#include "feed/rfeed_adapter.h"
#include "feed/feedview/expandfeed.h"
#include "feed/ilikeit_adapter.h"
#include "feed/feedview/headurl.h"

XAR_IMPL(exGT30); // expand > 30 ms
XAR_IMPL(gfGT30);
XAR_IMPL(likeGT30);

namespace xce {
static const std::string kDefaultContentType("text/html; charset=utf-8");

bool FeedTypeAction::Match(const std::string& url) const {
  // http://www.renren.com/feedretrieve.do
  return boost::starts_with(url, "/feedretrieve.do") ||  boost::starts_with(url, "/feedretrieve2.do");
}

cwf::HttpStatusCode FeedTypeAction::Process(cwf::Request * req, cwf::Response * res) {
  MCE_INFO("feed retr pid:" << getpid());
  boost::scoped_ptr<cwf::User> u(Authorize(req));
  if (!u) {
    return cwf::HC_UNAUTHORIZED;
  }

  const int type = req->form("f", -1); //type == -1 是“全部”的访问
  int render_type = WS_HOME;

  const int page = req->get("p", 0);
  const int count = 20;
  // std::string host = req->query("host");

  // header
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
// Cache-Control	no-cache
// Pragma	no-cache
// Expires	Thu, 01 Jan 1970 00:00:00 GMT
  // res->header().Add(cwf::HH_CACHE_CONTROL, "no-cache");
  // config in nginx?
  res->OutputHeader();

  feed::FeedDataSeq feed_seq;
  feed::FeedDataResPtr feed_res;

  //if (u->id() % 10 == 2 || u->id() % 10 == 1 || u->id() % 10 == 9 || u->id() % 10 == 3|| u->id() % 10 == 4|| u->id() % 10 == 0) 
  {
    render_type = WS_HOME|WS_HOT;
    try {
      //feed::FeedNewsManagerPrx fprx = CachedFeedNews3Adapter(u->id());
      feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(u->id());

      MCE_INFO("feed retrive as hot. type=" << type << " uid=" << u->id());

      if (type <= 0) {
        feed_res = fprx->GetHotFeedData(u->id(), page * count, count);
      } else {
        std::vector<int> types;
        if (5 == type) {
          types.push_back(502);
          feed_res = fprx->GetHotFeedDataByStype(u->id(), types, page * count, count);
        } else {
          types.push_back(type * 100);
          feed_res = fprx->GetHotFeedDataByType(u->id(), types, page * count, count);
        }
      }

    } catch (Ice::Exception & e) {
      MCE_WARN("feed retrive as hot error. uid=" << u->id() << " : " << e);
      // return cwf::HC_SERVICE_UNAVAILABLE;
    }
    if (feed_res) {
      feed_seq = feed_res->data;
    }
  }
 /* else {
    try {
      ptime pt("getfeed", false, 30);
      feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(u->id());

      LOG_F(INFO) << "feed retrive. type=" << type << " uid=" << u->id();

      std::vector<int> types;
      if (-1 == type) {
        feed_seq = fprx->getFeedData(u->id(), page * count, count);
      } else {
        // 应康伟要求: 如果是取状态，就只取 502 类型的新鲜事，去掉头像变更
        if (5 == type) {
          types.push_back(502);
          feed_seq = fprx->getFeedDataByStype(u->id(), types, page * count, count);
        } else {
          types.push_back(type * 100);
          feed_seq = fprx->getFeedDataByType(u->id(), types, page * count, count);
        }
      }

      if (pt.wall_clock() > 30)
        XAR_INC(gfGT30);
    } catch (Ice::Exception & e) {
      LOG_F(WARNING) << "FeedItemAdapter error : " << e;
      // return cwf::HC_SERVICE_UNAVAILABLE;
    }
  } */

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

    if (pt.wall_clock() > 30)
      XAR_INC(likeGT30);
  }
#endif

  {
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
      bool f = Expand(feed_seq[i].get(), u.get(), 0, (WeiredSource)render_type, likeinfo
        , page * count + i, &output, stat_log);
      if (f) {
        ++ feed_count;
        res->WriteRaw(output);
        stat += stat_log;
      }
    }

    try {
      xce::feed::FeedStatLogPrx prx = CachedFeedStatLogAdapter(u->id());
      //LOG(INFO) << "prx:" << prx;
      if (!prx) {
        MCE_WARN("feedtypeaction.cc::Process --> FeedStatLog prx is NULL !");
      } else {
        std::vector<std::string> cate;
        cate.push_back("feed");
        //cate.push_back("stat");
        cate.push_back("retrieve");
        prx->AddLog(cate, stat);
      }
    } catch (Ice::Exception& e) {
      LOG_F(WARNING) << e;
    }


    if (pt.wall_clock() > 30)
      XAR_INC(exGT30);
  }

  //LOG(VERBOSE) << "p: " << page << " seq: " << feed_seq.size() << " expand: " << feed_count;
  MCE_INFO("p: " << page << " seq: " << feed_seq.size() << " expand: " << feed_count);

  // 最后添加一个如下字符串，为js服务，否则不会添加进列表
  // 第一页 ##@L#1##@L#20##@L#20
  // 第二页 ##@L#21##@L#40##@L#20
  char buf[100];
  snprintf(buf, sizeof(buf), "##@L#%d##@L#%d##@L#%d"
    , 1 + page * count, page * count + feed_count, feed_count);
  res->WriteRaw(buf);
  return cwf::HC_OK;
}

static void Init2() {
  // LOG(INFO) << "Register FeedTypeAction";
  cwf::FrameWork::RegisterAction(new FeedTypeAction);

#ifdef WIN32
  return;
#endif

  try {
    xce::Adapter::feed::ResetCached<feed::FeedItemManagerPrx>("FIM@FeedItem", 20);
  } catch(std::exception & e) {
    LOG(INFO) << "feednews prx reset error " << e.what();
  }
  try {
    xce::Adapter::minifeed::ResetCached<feed::FeedItemManagerPrx>("FIM@FeedMini", 20);
  } catch(std::exception & e) {
    LOG(INFO) << "minifeed prx reset error " << e.what();
  }

  HeadUrl::FromPart("");
}

GLOBAL_INIT(CWF_TYPE_ACTION, {
  AddStartup(&Init2);
});

}
