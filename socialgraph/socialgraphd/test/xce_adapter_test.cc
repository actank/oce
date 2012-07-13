#include "gtest/gtest.h"
#include <fstream>

#include "boost/foreach.hpp"
#include "base/stringdigest.h"
#include "base/logging.h"
#include "base/threadpool.h"
#include "OceCxxAdapter/src/ContentRcdAdapter.h"
#include "OceCxxAdapter/src/SocialGraphRelationAdapter.h"
#include "OceCxxAdapter/src/SocialGraphReportServiceAdapter.h"
#include "OceCxxAdapter/src/SocialGraphRcdMessageServiceAdapter.h"

namespace xce {
namespace socialgraph {

TEST(ContentRcdAdapter, GetVideoRcd) {
  ContentRcdSeq rcd_seq;
  std::string s_url = "http://v.youku.com/v_show/id_XMjU0NjQyMjc2.html";
  try {
    ContentRcd cr;
    cr.md5 = MD5(s_url);
    cr.desc = s_url;
    cr.lisc = 2;
    rcd_seq = ContentRcdAdapter::instance().GetVideoRcd(286162347, cr);
    LOG(INFO) << "rcd_seq.size():" << rcd_seq.size();
    BOOST_FOREACH(ContentRcd& rcd, rcd_seq) {
      std::ostringstream oss;
      oss << "md5:" << rcd.md5 << " rcd.shareurl:" << rcd.shareurl;
      LOG(INFO) << oss.str();
    }
  } catch (Ice::Exception& e) {
    LOG(LERROR) << e.what();
  }
}

TEST(SocialGraphRelationAdapter, GetEmotionState) {
  
  int user_id = 286162347;
  EmotionState es;
  es.et.userId = user_id;
  es.et.lt.first = 0;
  es.et.lt.second = 0;
  try {
    SgRelationAdapter::instance().setEmotionState(user_id, es);
    //es = SgRelationAdapter::instance().getEmotionState(user_id);
   // LOG(INFO) << "es.et.userId:" << es.et.userId << " first:" << es.et.lt.first << " second:" << es.et.lt.second << " court:" << es.court;
  } catch (Ice::Exception& e) {
    LOG(LERROR) << e.what();
  } catch (...) {
    LOG(LERROR) << "... exception";
  }
}


TEST(SocialGraphReportServiceAdatper, function) {
//  std::string job_id = ReportServiceAdapter::instance().applyJobId("foo");
  std::string job_id = "foo_1311750346932";
  ReportServiceAdapter::instance().startAndRunning(job_id, "fucker foo");
  LOG(INFO) << "job_id:" << job_id;
}

TEST(SocialGraphRcdMessageServiceAdapter, addblock) {
  Ice::Long source = 1000;
  Ice::Long target = 2000;
  RcdMessageServiceAdapter::instance().AddSgBlock(source, target, "FFW");
}

TEST(SocialGraphRcdMessageServiceAdapter, removeRcd) {
  Ice::Long source = 666666;
  Ice::Long target = 8;
  RcdMessageServiceAdapter::instance().RemoveRcdCache(source, target, "FFW");
}



}}


