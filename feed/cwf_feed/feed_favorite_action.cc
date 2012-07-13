#include <stdio.h>
#ifdef WIN32
#define snprintf _snprintf
#endif
#include <boost/algorithm/string/predicate.hpp>       // for starts_with
// #include <boost/smart_ptr/scoped_ptr.hpp>

#include "feed/base_feed/common.h"
#include "feed/base_feed/logging.h"
#include "feed/base_feed/ptime.h"
#include "feed/base_feed/globalinit.h"
#include "feed/base_feed/startuplist.h"

#include "feed/cwf_feed/action.h"
#include "feed/cwf_feed/frame.h"
#include "feed/arch_feed/site_xiaonei.h"

#include "IceLogger.h"
#include "feed/rfeed_adapter.h"
#include "feed/feedview/expandfeed.h"
#include "feed/ilikeit_adapter.h"
#include "feed/feedview/headurl.h"
#include "OceCxxAdapter/src/VipIconDataReaderAdapter.h"
#include "FeedStatLogAdapter.h"
#include "FeedFocusAdapter.h"

XAR_IMPL(expand_GT30); // expand > 30 ms
XAR_IMPL(focus_get_GT30);
XAR_IMPL(like_GT30);

namespace xce {

struct FeedFavoriteAction : public cwf::BaseAction {
  bool Match(const std::string& url) const;
  cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
};

bool FeedFavoriteAction::Match(const std::string& url) const {
  // http://www.renren.com/feedretrieve.do
  return boost::starts_with(url, "/feedfavorite.do") || boost::starts_with(url, "/feedfavorite2.do");
}

cwf::HttpStatusCode FeedFavoriteAction::Process(cwf::Request * req, cwf::Response * res) {
  int cwf_pid = getpid();
  MCE_INFO("FeedFavoriteAction::Process --> pid:" << cwf_pid);

#if 0
  int uidarr[] = {
    200024619,    200301672,    200431163,    20035454,    25500412,    2143,    2267625,    
    224610169,    224623024,    224926741,    22420274,    22766242,    22804028,    22820426,
    230716453,    231106894,    234800486,    25639587,    23617453,    23617462,    23669539,
    244149202,    247135184,    248982587,    49402770,    25054928,    25294674,    25694512,
    353044164,    367344916,    367347221,    58742,    600002261,    64932
  };
  int i = rand() % ARRAY_SIZE(uidarr);
  boost::scoped_ptr<cwf::User> u(new cwf::User("Ken", uidarr[i]));
#else
  boost::scoped_ptr<cwf::User> u(Authorize(req));
  if (!u) {
    return cwf::HC_UNAUTHORIZED;
  }
#endif

  const int page = req->get("p", 0);
  const int count = 20;
  // std::string host = req->query("host");

  // header
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, "text/html; charset=utf-8");
// Cache-Control	no-cache
// Pragma	no-cache
// Expires	Thu, 01 Jan 1970 00:00:00 GMT
  // res->header().Add(cwf::HH_CACHE_CONTROL, "no-cache");
  // config in nginx?
  res->OutputHeader();

  feed::FeedDataSeq feed_seq;

  try {
    MCE_INFO("FeedFavoriteAction::Process --> pid:" << cwf_pid <<  "uid:" << u->id());
    ptime pt("getfeed", false, 30);
    //feed::FocusManagerPrx fprx = CachedFeedFocusAdapter(u->id());
    //if (!fprx) {
    //  MCE_INFO("FeedFavoriteAction::Process --> pid:" << cwf_pid << " fprx is NULL!");
    //  return cwf::HC_SERVICE_UNAVAILABLE;
    //}


    //feed_seq = fprx->GetFocusFeedData(u->id(), page * count, count);
    feed_seq = feed::FeedFocusAdapter::instance().GetFocusFeedData(u->id(), page * count, count);

    if (pt.wall_clock() > 30)
      XAR_INC(focus_get_GT30);
  } catch (Ice::Exception & e) {
    // return cwf::HC_SERVICE_UNAVAILABLE;
    MCE_WARN("FeedFavoriteAction::Process --> pid:" << cwf_pid <<  " uid:" << u->id() << " exception:" << e);
  }
  MCE_INFO("FeedFavoriteAction::Process --> pid:" << cwf_pid <<  " uid:" << u->id() << " seq:" << feed_seq.size());
  int feed_count = 0; // 实际输出的 Feed条目数

  // 得到“赞” ilike 信息
  ::xce::ilikeit::LikeInfoSeq likeseq;
  std::vector<char> vbool(0, feed_seq.size()); // 有些类型并不需要 zan
  bool likeinfo_avaible = false;
#if POSIX
  {
    ptime pt("buildzan", false, 30);
    ::xce::ilikeit::GlobalIdSeq globalseq;
    for (size_t i=0; i<feed_seq.size(); ++i) {
      feed::FeedData* feed = feed_seq[i].get();
      if (BuildGlobalId(feed->source, feed->type, &globalseq)) {
        vbool.push_back(true);
      } else {
        vbool.push_back(false);
      }
    }
    
    if (!globalseq.empty()) {
      // try catch inside
      likeinfo_avaible = BuildZan(u->id(), globalseq, &likeseq);
    }

    if (pt.wall_clock() > 30)
      XAR_INC(like_GT30);
  }
#endif
  MCE_INFO("FeedFavoriteAction::Process --> pid:" << cwf_pid << " expand start.");


  {
    ptime pt("expand", false, 30);
    std::string stat;
    
    MyUtil::IntSeq uids, vipicontypes;
    vipicontypes.push_back(0); //目前只传0，具体有哪些类型，见OceCxxAdapter/src/VipIconDataReaderAdapter.h中的VIP_TYPE枚举类型
    for (size_t i = 0; i<feed_seq.size(); ++i) {
      uids.push_back(feed_seq[i].get()->actor);
    }
    vipinfo::VipFeedDataSeqMap vipInfoMap;
    const vipinfo::VipFeedDataSeq emptyVipData;
    try {
      vipInfoMap = adapter::vipicondata::VipIconDataReaderAdapter::instance().getVipFeedDataSeqMap(uids, vipicontypes);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedFavoriteAction::Process --> pid:" << cwf_pid <<" getVipFeedData exception:"<< e);
    } catch (std::exception& e) {
      MCE_WARN("FeedFavoriteAction::Process --> pid:" << cwf_pid <<" getVipFeedData exception:"<< e.what());
   }

    for (size_t i=0,likeidx = 0; i<feed_seq.size(); ++i) {
      std::string output;

      const ilikeit::LikeInfo* likeinfo = 0;
      if (likeinfo_avaible && vbool[i]) {
        likeinfo = likeseq[likeidx++].get();
      } else {
        likeinfo = 0;
      }
     
      std::string stat_log; 
      bool f = false; 

     vipinfo::VipFeedDataSeqMap::const_iterator it = vipInfoMap.find(feed_seq[i].get()->actor); 
     if(it == vipInfoMap.end() || (it->second).empty()) {
        f = ExpandFeedData(feed_seq[i].get(), u.get(), 0, (WeiredSource)(WS_MINIFEED | WS_FAVORITE), likeinfo
          , page * count + i, emptyVipData, &output, stat_log);
      }
      else {
        f = ExpandFeedData(feed_seq[i].get(), u.get(), 0, (WeiredSource)(WS_MINIFEED | WS_FAVORITE), likeinfo
          , page * count + i, it->second, &output, stat_log);
      }

      //f = ExpandFeedData(feed_seq[i].get(), u.get(), 0, WeiredSource(WS_MINIFEED | WS_FAVORITE), likeinfo
      //  , page * count + i, emptyVipIconDataSeq, &output, stat_log);
      if (f) {
        ++ feed_count;
        res->WriteRaw(output);
        stat += stat_log;
        //stat += "\n";
      }
    }

    try {
      //xce::feed::FeedStatLogPrx prx = CachedFeedStatLogAdapter(u->id());
      //LOG(INFO) << "prx:" << prx;
      //if (!prx) {
        //LOG(INFO) << "feed_favorite_action.cc::Process --> FeedStatLog prx is NULL !";
        //MCE_WARN("FeedFavoriteAction::Process --> pid:" << cwf_pid << " FeedStatLog prx is NULL !");
      //} else {
        std::vector<std::string> cate;
        cate.push_back("feed");
        //cate.push_back("stat");
        cate.push_back("favorite");
        //prx->AddLog(cate, stat);
        feed::FeedStatLogAdapter::instance().AddLog(cate, stat);
      //}
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedFavoriteAction::Process --> pid:" << cwf_pid << " " << e);
    }


    if (pt.wall_clock() > 30)
      XAR_INC(expand_GT30);
  }

  MCE_INFO("FeedFavoriteAction::Process --> pid:" << cwf_pid << " p: " << page << " seq: " << feed_seq.size()
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

static void InitFavoriteAction() {
  MCE_INFO("Register FeedFavoriteAction");
  cwf::FrameWork::RegisterAction(new FeedFavoriteAction);

#ifdef WIN32
  return;
#endif

  try {
    xce::Adapter::feed::ResetCached<feed::FeedItemManagerPrx>("M@FeedFocus", 5);
  } catch(std::exception & e) {
    MCE_WARN("feednews prx reset error " << e.what());
  }

  HeadUrl::FromPart("");
}

GLOBAL_INIT(CWF_TYPE_ACTION, {
  AddStartup(&InitFavoriteAction);
});

}
