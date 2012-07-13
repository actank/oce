#include "feed/cwf_action/ugc_action.h"

#include <stdio.h>
#ifdef WIN32
#define snprintf _snprintf
#endif
#include <boost/algorithm/string/predicate.hpp>       // for starts_with
// #include <boost/smart_ptr/scoped_ptr.hpp>

#include "base/common.h"
//#include "base/logging.h"
#include "base/ptime.h"
#include "base/globalinit.h"
#include "base/startuplist.h"

#include "cwf/frame.h"
#include "arch/site_xiaonei.h"

#include "LogWrapper.h"
#include "unistd.h"

#include "FeedNewsReplicaAdapter.h"
#include "FeedMiniReplicaAdapter.h"


#include "feed/rfeed_adapter.h"
#include "feed/feedview/expandfeed.h"
#include "feed/ilikeit_adapter.h"
#include "feed/feedview/headurl.h"

//XAR_IMPL(exGT30); // expand > 30 ms
//XAR_IMPL(gfGT30);
//XAR_IMPL(likeGT30);

namespace xce {
static const std::string kDefaultContentType("text/html; charset=utf-8");

bool UgcFeedAction::Match(const std::string& url) const {
  // http://www.renren.com/feedretrieve.do
  return boost::starts_with(url, "/getugcfeed.do");
}

const int ugc_type_arr[] = {209,211,501,502,601,701,702,708,709,901,902,903,904,1101,1102,1103,1104,1501,1502,2901,2902,2906,8210};
const std::vector<int> types(ugc_type_arr, ugc_type_arr + sizeof(ugc_type_arr) / sizeof(int) );
//const std::vector<int> types;

cwf::HttpStatusCode UgcFeedAction::Process(cwf::Request * req, cwf::Response * res) {
  int cwf_pid = getpid();
  MCE_INFO("UgcFeedAction::Process --> pid:" << cwf_pid << " types:" << types.size());

  boost::scoped_ptr<cwf::User> u(Authorize(req));
  if (!u) {
    return cwf::HC_UNAUTHORIZED;
  }

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
      MCE_INFO("UgcFeedAction::Process --> pid:" << cwf_pid << " uid:" << u->id());
      feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(u->id());
      if (!fprx) {
        MCE_INFO("UgcFeedAction::Process --> pid:" << cwf_pid << " fprx is NULL!");
        return cwf::HC_SERVICE_UNAVAILABLE;
      }
      //feed_seq = FeedNewsAdapter::instance().GetHotFeedDataByType(uid, types, from, count);

      feed_res = fprx->GetLiveFeedDataByStype(u->id(), types, page * count, count);

    } catch (Ice::Exception & e) {
      //LOG_F(WARNING) << "feed retrive as hot error. uid=" << u->id() << " : " << e;
      MCE_WARN("UgcFeedAction::Process --> pid:" << cwf_pid << " uid:" << u->id() << " exception:" << e);
      // return cwf::HC_SERVICE_UNAVAILABLE;
    }
    if (feed_res) {
      feed_seq = feed_res->data;
    }
  }
  MCE_INFO("UgcFeedAction::Process --> pid:" << cwf_pid << " uid:" << u->id()  << " types size:" << types.size() << " seq: " << feed_seq.size());
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

    //if (pt.wall_clock() > 30)
     // XAR_INC(likeGT30);
  }
#endif
  MCE_INFO("UgcFeedAction::Process --> pid:" << cwf_pid << " uid:" << u->id() << " expand start.");

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
      bool f = ExpandFeedData(feed_seq[i].get(), u.get(), 0, (WeiredSource)render_type, likeinfo
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
      MCE_WARN("feedtypeaction.cc::Process --> exception:" << e);
    }


    //if (pt.wall_clock() > 30)
     // XAR_INC(exGT30);
  }

  MCE_INFO("UgcFeedAction::Process --> pid:" << cwf_pid << " uid:" << u->id() 
      << "p: " << page << " seq: " << feed_seq.size()
      << " expand: " << feed_count);

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
  // LOG(INFO) << "Register UgcFeedAction";
  cwf::FrameWork::RegisterAction(new UgcFeedAction);

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
