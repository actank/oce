
#include <stdio.h>
#ifdef WIN32
#define snprintf _snprintf
#endif
#include <boost/algorithm/string/predicate.hpp>       // for starts_with
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

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

struct MiniGroupPushAction : public cwf::BaseAction {
  bool Match(const std::string& url) const;
  cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
};

bool MiniGroupPushAction::Match(const std::string& url) const {
  return boost::starts_with(url, "/qunpush.do"); 
}

cwf::HttpStatusCode MiniGroupPushAction::Process(cwf::Request * req, cwf::Response * res) {
  boost::scoped_ptr<cwf::User> u(Authorize(req));
  if (!u) {
    return cwf::HC_UNAUTHORIZED;
  }

  //const int type = req->form("f", -1);
  //const int page = req->get("p", 0);
  const long gid = req->get("gid", 0);
  const int isAdmin = req->get("isAdmin", 0);
  const std::string fid_str = req->get("feedid");
  //const int count = 20;
  const int push_max_count = 20;

  std::vector<std::string> feed_str_vec;
  boost::split(feed_str_vec, fid_str, boost::is_any_of(","), boost::token_compress_on);

  if (feed_str_vec.size() > push_max_count) {
    LOG_F(WARNING) << "mini_group_push_action.cc Process --> feed_str_vec size:" << feed_str_vec.size(); 
    feed_str_vec.resize(push_max_count);
  }

  std::vector<long> feed_id_vec;
  for (std::vector<std::string>::iterator it = feed_str_vec.begin(); it != feed_str_vec.end(); it++) {
    std::string id_str = *it;
    long fid = -1;
    try {
      fid = boost::lexical_cast<long>(id_str);
    } catch (boost::bad_lexical_cast& e) {
      LOG_F(WARNING) << "mini_group_push_action.cc Process --> " << e.what();
      continue;
    }
    feed_id_vec.push_back(fid);
  } 
 

  // header
  res->header().set_status_code(cwf::HC_OK, "OK");
  res->header().Add(cwf::HH_CONTENT_TYPE, kDefaultContentType);
  res->OutputHeader();

  //feed::FeedDataResPtr feed_res;
  feed::FeedDataSeq feed_seq;
  try {
    ptime pt("getfeed", false, 30);

    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(u->id());
    LOG(INFO) << "mini_group_push_action.cc Process --> fprx:" << fprx; 

    feed_seq = fprx->GetFeedDataByIds(feed_id_vec);
    LOG(INFO) << "mini_group_push_action.cc Process --> uid:" << u->id() << " gid:" << gid << " count:" << feed_seq.size() << "/" << feed_id_vec.size() ;

  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "mini_group_push_action.cc Process --> " << e;
  }

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
        , i, &output, stat_log, isAdmin );
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

  LOG(VERBOSE)  << " seq: " << feed_seq.size()
    << " expand: " << feed_count;

/*
  // 最后添加一个如下字符串，为js服务，否则不会添加进列表
  // 第一页 ##@L#1##@L#20##@L#20
  // 第二页 ##@L#21##@L#40##@L#20
  char buf[100];
  snprintf(buf, sizeof(buf), "##@L#%d##@L#%d##@L#%d"
    , 1 + page * count, page * count + feed_count, feed_count);
  res->WriteRaw(buf);
*/
  return cwf::HC_OK;
}

static void InitMiniGroupPushAction() {
  // LOG(INFO) << "Register MiniGroupPushAction";
  cwf::FrameWork::RegisterAction(new MiniGroupPushAction);

#ifdef WIN32
  return;
#endif

  try {
    xce::Adapter::feed::ResetCached<feed::FeedNewsManagerPrx>("FIM@FeedNews", 20);
  } catch(std::exception & e) {
    LOG(INFO) << "FeedNewsManager prx reset error " << e.what();
  }

  HeadUrl::FromPart("");
}

GLOBAL_INIT(CWF_TYPE_ACTION, {
  AddStartup(&InitMiniGroupPushAction);
});

}
