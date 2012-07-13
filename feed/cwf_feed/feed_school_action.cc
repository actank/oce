
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
  int cwf_pid = getpid();
  MCE_INFO("FeedSchoolAction::Process --> pid:" << cwf_pid);

  boost::scoped_ptr<cwf::User> u(Authorize(req));
  if (!u) {
    return cwf::HC_UNAUTHORIZED;
  }

  const int type = req->form("f", -1);
  const int page = req->get("p", 0);
  const int school_id = req->get("s", 0);
  const int count = 20;

  // header
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
  res->OutputHeader();

  feed::FeedDataSeq feed_seq;

  try {
    MCE_INFO("FeedSchoolAction::Process --> pid:" << cwf_pid << " type:" << type << " school_id:" << school_id);
    ptime pt("getfeed", false, 30);
    feed::FeedItemManagerPrx fprx = CachedFeedSchoolAdapter(school_id);
    if (!fprx) {
      MCE_WARN("FeedSchoolAction::Process --> pid:" << cwf_pid << " type:" << type << " school_id:" << school_id << "prox is NULL!");
      return cwf::HC_SERVICE_UNAVAILABLE;
    }
    

    if (type <= 0) {
      feed_seq = fprx->getFeedData(school_id, page * count, count);
    } else {
      std::vector<int> types;
      types.push_back(type * 100);
      feed_seq = fprx->getFeedDataByType(school_id, types, page * count, count);
    }
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedSchoolAction::Process --> pid:" << cwf_pid << " type:" << type << " school_id:" << school_id << " exception:" << e);
    // return cwf::HC_SERVICE_UNAVAILABLE;
  }

  MCE_INFO("FeedSchoolAction::Process --> pid:" << cwf_pid << " type:" << type << " school_id:" << school_id << " seq:" << feed_seq.size());
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
  MCE_INFO("FeedSchoolAction::Process --> pid:" << cwf_pid << " type:" << type << " school_id:" << school_id << " expand start.");

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
      MCE_WARN("FeedSchoolAction::Process --> pid:" << cwf_pid <<" getVipFeedData exception:"<< e);
    } catch (std::exception& e) {
      MCE_WARN("FeedSchoolAction::Process --> pid:" << cwf_pid <<" getVipFeedData exception:"<< e.what());
    }
    
    for (size_t i=0,likeidx = 0; i<feed_seq.size(); ++i) {
      std::string output;

      const ilikeit::LikeInfo* likeinfo = 0;
      if (likeinfo_avaible && vbool[i])
        likeinfo = likeseq[likeidx++].get();
      else
        likeinfo = 0;

      std::string stat_log;
      xce::vipinfo::VipIconDataSeq emptyVipIconDataSeq;
      bool f = false;

      vipinfo::VipFeedDataSeqMap::const_iterator it = vipInfoMap.find(feed_seq[i].get()->actor);
      if(it == vipInfoMap.end() || (it->second).empty()) {
        f = ExpandFeedData(feed_seq[i].get(), u.get(), 0, (WeiredSource)(WS_MINIFEED), likeinfo
          , page * count + i, emptyVipData, &output, stat_log);
      }
      else {
        f = ExpandFeedData(feed_seq[i].get(), u.get(), 0, (WeiredSource)(WS_MINIFEED), likeinfo
          , page * count + i, it->second, &output, stat_log);
      }

      //f = ExpandFeedData(feed_seq[i].get(), u.get(), 0, WeiredSource(WS_MINIFEED), likeinfo
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
        //MCE_WARN("feed_school_action.cc::Process --> FeedStatLog prx is NULL !");
      //} else {
        std::vector<std::string> cate;
        cate.push_back("feed");
        //cate.push_back("stat");
        cate.push_back("school");
        //prx->AddLog(cate, stat);
        feed::FeedStatLogAdapter::instance().AddLog(cate, stat);
      //}
    } catch (Ice::Exception& e) {
      MCE_WARN(e);
    }

  }

  MCE_INFO("FeedSchoolAction::Process --> pid:" << cwf_pid  << " p: " << page << " school_id:" << school_id  << " seq: " << feed_seq.size() 
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

static void InitSchoolAction() {
  // LOG(INFO) << "Register FeedSchoolAction";
  cwf::FrameWork::RegisterAction(new FeedSchoolAction);

#ifdef WIN32
  return;
#endif

  try {
    xce::Adapter::feed::ResetCached<feed::FeedItemManagerPrx>("FIM@FeedSchool", 2);
  } catch(std::exception & e) {
    MCE_INFO("feednews prx reset error " << e.what());
  }

  HeadUrl::FromPart("");
}

GLOBAL_INIT(CWF_TYPE_ACTION, {
  AddStartup(&InitSchoolAction);
});

}
