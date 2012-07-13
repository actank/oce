
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

struct FeedSchoolAction : public cwf::BaseAction {
  bool Match(const std::string& url) const;
  cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
};

bool FeedSchoolAction::Match(const std::string& url) const {
  return boost::starts_with(url, "/feedschool.do") || boost::starts_with(url, "/feedschool2.do");
}

cwf::HttpStatusCode FeedSchoolAction::Process(cwf::Request * req, cwf::Response * res) {
  boost::scoped_ptr<cwf::User> u(Authorize(req));
  if (!u) {
    return cwf::HC_UNAUTHORIZED;
  }

  const int type = req->form("f", -1);
  const int page = req->get("p", 0);
  const int school_id = req->get("s", 0);
  const int count = 20;

  LOG_F(INFO) << "feed school get. type=" << type << " school_id=" << school_id << " type=" << type;

  // header
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
  res->OutputHeader();

  feed::FeedDataSeq feed_seq;

  try {
    ptime pt("getfeed", false, 30);
    feed::FeedItemManagerPrx fprx = CachedFeedSchoolAdapter(school_id);
    
    LOG_F(WARNING) << "feed school proxy : " << fprx->ice_toString();

    if (type <= 0) {
      feed_seq = fprx->getFeedData(school_id, page * count, count);
    } else {
      std::vector<int> types;
      types.push_back(type * 100);
      feed_seq = fprx->getFeedDataByType(school_id, types, page * count, count);
    }
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "feed school getFeedData error : " << e;
    // return cwf::HC_SERVICE_UNAVAILABLE;
  }

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
      bool f = Expand(feed_seq[i].get(), u.get(), 0, WeiredSource(WS_MINIFEED), likeinfo
        , page * count + i, &output, stat_log);
      if (f) {
        ++ feed_count;
        res->WriteRaw(output);
        stat += stat_log;
        //stat += "\n";
      }
    }
    try {
      xce::feed::FeedStatLogPrx prx = CachedFeedStatLogAdapter(u->id());
      //LOG(INFO) << "prx:" << prx;
      if (!prx) {
        LOG(INFO) << "feed_school_action.cc::Process --> FeedStatLog prx is NULL !";
      } else {
        std::vector<std::string> cate;
        cate.push_back("feed");
        //cate.push_back("stat");
        cate.push_back("school");
        prx->AddLog(cate, stat);
      }
    } catch (Ice::Exception& e) {
      LOG_F(WARNING) << e;
    }

  }

  LOG(INFO) << "feed school p=" << page << " seq=" << feed_seq.size()
    << " expand=" << feed_count;

  // 最后添加一个如下字符串，为js服务，否则不会添加进列表
  // 第一页 ##@L#1##@L#20##@L#20
  // 第二页 ##@L#21##@L#40##@L#20
  char buf[100];
  snprintf(buf, sizeof(buf), "##@L#%d##@L#%d##@L#%d"
    , 1 + page * count, page * count + feed_count, feed_count);
  res->WriteRaw(buf);
  return cwf::HC_OK;
}

static void InitSchoolAction() {
  // LOG(INFO) << "Register FeedSchoolAction";
  cwf::FrameWork::RegisterAction(new FeedSchoolAction);

#ifdef WIN32
  return;
#endif

  try {
    xce::Adapter::feed::ResetCached<feed::FeedItemManagerPrx>("FIM@FeedSchool", 2);
  } catch(std::exception & e) {
    LOG(INFO) << "feednews prx reset error " << e.what();
  }

  HeadUrl::FromPart("");
}

GLOBAL_INIT(CWF_TYPE_ACTION, {
  AddStartup(&InitSchoolAction);
});

}
