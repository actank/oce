#include "socialgraph/socialgraphd/video/worker/content_rcd.h"

#include <boost/foreach.hpp>
#include <boost/random.hpp>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "OceCxxAdapter/src/ActiveTrackAdapter.h"

void MyUtil::initialize() {
  using namespace MyUtil;
  using namespace com::xiaonei::xce;
  using namespace xce::serverstate;
  using namespace xce::socialgraph;
  using namespace xce::log;
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&ContentRcdWorkerI::instance(), service.createIdentity("CRWI", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("ContentRcdWorker.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ContentRcdWorker.Interval", 5);

  static const std::string kControllerName = "ControllerContentRcdWorker";
  // register to Controller Service
  ServerStateSubscriber::instance().initialize(kControllerName,  &ContentRcdWorkerI::instance(), mod, interval, new MyUtil::SocialGraphChannel());
  ServerStateSubscriber::instance().setStatus(1);
  ActiveTrack::instance().initialize();
}


namespace xce {
namespace socialgraph {


ContentRcdWorkerI::ContentRcdWorkerI() : thread_poll_(8, 16) {
  MyUtil::TimeCost tc = MyUtil::TimeCost::create("ContentRcdWorkerI::ContentRcdWorkerI()", MyUtil::TimeCost::LOG_LEVEL_INFO);
  tree_ = new Tree("/data/xce/Project/videoshare/video_share_result", "/data/xce/Project/videoshare/tag_index");
  thread_poll_.Post(new InitVideoTreeMessage(tree_));
}

ContentRcdSeq ContentRcdWorkerI::GetVideoRcd(int host, const ContentRcd& rcd, const Ice::Current& current) {
  std::ostringstream oss;
  oss << "ContentRcdWorkerI::GetVideoRcd(" << host << ")" 
      << " rcd.md5:" << rcd.md5 
      << " rcd.desc:" << rcd.desc 
      << " rcd.uid:" << rcd.uid
      << " rcd.sid:" << rcd.sid
      << " rcd.lisc:" << rcd.lisc;
  MyUtil::TimeCost tc = MyUtil::TimeCost::create(oss.str(), MyUtil::TimeCost::LOG_LEVEL_INFO);   

  ContentRcdSeq content_rcd_seq;
  VideoShare* vs = tree_->SearchVideoShare(rcd.md5);
  if (!vs) {
    return content_rcd_seq;   
  }
  Node* node = vs->node();
  int size = node->video_share_list()->size();

  if (size == 0) { // return immediately

  }
  else if (size <= 2) {
    BOOST_FOREACH(VideoShare* v, *node->video_share_list()) {
      if (v->md5() != rcd.md5) {
        ContentRcd cr;
        cr.md5 = v->md5(); 
        cr.sid = v->share_id();
        cr.uid = v->user_id();
        cr.uname = "";
        cr.thumburl = v->thumb_url();
        cr.shareurl = v->ShareUrl();
        cr.title = v->title();
        cr.lisc = v->score();
        cr.desc = "";
        content_rcd_seq.push_back(cr);
      } 
    }
  }
  else {
    static boost::mt19937 gen; 
    boost::uniform_int<> dist(0, size - 1);
    boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(gen, dist);
    int step = die(); 
    std::list<VideoShare*>::const_iterator it = node->video_share_list()->begin();
    std::advance(it, step);
    for (int i = 0; i < 2 && it != node->video_share_list()->end(); ++i, ++it) {
      if ((*it)->md5() != rcd.md5) {
        ContentRcd cr;
        cr.md5 = (*it)->md5(); 
        cr.sid = (*it)->share_id();
        cr.uid = (*it)->user_id();
        cr.uname = "";
        cr.thumburl = (*it)->thumb_url();
        cr.shareurl = (*it)->ShareUrl();
        cr.title = (*it)->title();
        cr.lisc = (*it)->score();
        cr.desc = "";
        content_rcd_seq.push_back(cr);
      } 
    }
  }
  MCE_INFO("ContentRcdWorkerI::GetVideoRcd(" << host << ") rcd.md5:" << rcd.md5 << " content_rcd_seq.size():" << content_rcd_seq.size());
  xce::log::ActiveTrack::instance().action(host, "video_rcd", "show", "non-object");
  return content_rcd_seq;
}


}} // end xce::socialgraph



