#include "feed/feedview/feedviewI.h"

//#include "feed/base_feed/logging.h"
#include <time.h>
#include <sys/time.h>
#include "feed/base_feed/ptime.h"
#include "feed/base_feed/pcount.h"
#include "feed/base_feed/startuplist.h"
//#include "feed/base_feed/logrotate.h"

#include "feed/feedview/modifytime.h"
#include "feed/base_feed/signals.h"
#include <signal.h>

#include "feed/rfeed_adapter.h"
#include "feed/feedview/expandfeed.h"
#include "feed/feedview/feed_hdf_parser.h"
#include "feed/ilikeit_adapter.h"
#include "feed/feedview/feedcache.h"
#include "Notify/util/clear_silver_wrap.h"
#include "feed/cwf_feed/site.h"

#include "IceLogger.h"
//#include "FeedNewsReplicaAdapter.h"
#include "FeedNewsReplicaAdapter.h"
#include "FeedMiniReplicaAdapter.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "ctemplate/template.h"
#include "OceCxxAdapter/src/VipIconDataReaderAdapter.h"
#include "FeedFocusAdapter.h"
#include "FeedStatLogAdapter.h"
#include "feed/feedview/ambcache.h"


//#include "feed/arch_feed/service.h"
using namespace xce;
using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace ::xce::serverstate;

#define PRODUCT_SIGNAL_AMB_LOAD_UID 61

static void SignalReload2(int) {
  ctemplate::Template::ReloadAllIfChanged();
}

static void SignalReload3(int) {
  TaskManager::instance().execute(new AmbInitTask());
}

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&xce::FeedViewI::instance(), service.createIdentity("feedview", ""));


  ctemplate::Template::SetTemplateRootDirectory("/data/xce/XFeed/etc/feed_view_tpl3");
  ctemplate::Template::AddAlternateTemplateRootDirectory("/data/cwf2/tpl3");
  ctemplate::AddModifier("x-time", &friendly_time);

  // TODO: move to perfect place
  InstallSignal(XCE_SIGNAL_CWF_LOAD_TEMPLATE, &SignalReload2);
  InstallSignal(PRODUCT_SIGNAL_AMB_LOAD_UID, &SignalReload3);
  
  TaskManager::instance().execute(new AmbInitTask());
  TaskManager::instance().scheduleRepeated(new AmbLoadTask());
/*
  std::string name = service.getName();
  std::string log_dir = //props->getPropertyWithDefault(
  service.getCommunicator()->getProperties()->getPropertyWithDefault(
    "Service." + name + ".LogPath"
    , "../log/" + name);


  int log_level = LS_INFO;
  {
    std::string level = //props->getProperty("Service." + name + ".LogLevel");
    service.getCommunicator()->getProperties()->getProperty("Service." + name + ".LogLevel");
    if (!level.empty()) {
      log_level = LogRotate::LogLevel(level);
    }
  }

  LOG(INFO) << "log_dir:" << log_dir << " log_level:" << log_level;

  if (log_level != -1)
    LogRotate::instance().Start(log_dir, log_level);
*/
/*
{

  // 缺省log至 ../log/box_shop.log
  std::string filename_log("../log/box_log");

  std::ofstream* stem = new std::ofstream(filename_log.c_str(), std::ios::binary);
  LogMessage::LogToStream(stem, LS_INFO);

} */

    //detail::IceThreadHook* hook = new detail::IceThreadHook;

/*
    Ice::InitializationData init; 
    init.logger = new IceLogger;
    init.threadHook = hook;
    init.properties = props;    
    init.stats = communicator->getStats(); // ??? TODO: use
    cloned = Ice::initialize(init);
 */             
    //XAR_POLL("icethr", boost::bind(&detail::IceThreadHook::size, hook));
  
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FeedViewN.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedViewN.Interval", 5);
  MCE_INFO("mod:" << mod << " interval:" << interval);

  ServerStateSubscriber::instance().initialize("ControllerFeedViewR",&FeedViewI::instance(),mod,interval, new XceFeedChannel);
#if 1
  int is_test = MyUtil::ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedViewN.IsTest", 0);
  FeedTplCache::Instance().Reload(is_test);

  if (!FeedSchemaCache::Instance().Reload(is_test)) {
    MCE_WARN("FeedHdfParser load error!");
  }
#endif
}

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


static int ExpandUserFeeds(Ice::Int owner, Ice::Int uid, const std::string& name, Ice::Int render
    , feed::FeedDataSeq feed_seq, Ice::Int from, feed::HtmlResult * html, bool isAdmin = false, Ice::Int view = 0) {
  ASSERT(!feed_seq.empty() && html);
  MCE_INFO("feed seq size : " << feed_seq.size());
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
      struct timeval start, end;
      gettimeofday(&start, NULL);
      likeinfo_avaible = BuildZan(uid, globalseq, &likeseq);
      gettimeofday(&end, NULL);
      double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
   
      MCE_INFO("FeedViewI::Expand --> uid:" << uid << " zan dur:" << timeuse / 1000000);
    }

    if (pt.wall_clock() > 3)
      XAR_INC(LikeGT3);
  }

  cwf::User u(name, uid);
  {
    ptime pt("expand", false);
    std::string stat;

    MyUtil::IntSeq uids, types;
    types.push_back(0); //目前只传0，具体有哪些类型，见OceCxxAdapter/src/VipIconDataReaderAdapter.h中的VIP_TYPE枚举类型
    for (size_t i = 0; i<feed_seq.size(); ++i) {
      uids.push_back(feed_seq[i].get()->actor);
    }
    vipinfo::VipFeedDataSeqMap vipInfoMap;
    const vipinfo::VipFeedDataSeq emptyVipData;
    try {
      vipInfoMap = adapter::vipicondata::VipIconDataReaderAdapter::instance().getVipFeedDataSeqMap(uids, types);
    } catch (Ice::Exception& e) {
      MCE_WARN("feedviewI.cc::Expand --> uid:" << uid <<" getVipFeedData exception:"<< e);
    } catch (std::exception& e) {
      MCE_WARN("feedviewI.cc::Expand --> uid:" << uid <<" getVipFeedData exception:"<< e.what());
    }

    for (size_t i = 0,likeidx = 0; i<feed_seq.size(); ++i) {
      const ilikeit::LikeInfo* likeinfo = 0;
      if (vbool[i] && likeinfo_avaible)
        likeinfo = likeseq[likeidx++].get();
      else
        likeinfo = 0;

      std::string output;
      std::string stat_log;
      bool f = false;

      vipinfo::VipFeedDataSeqMap::const_iterator it = vipInfoMap.find(feed_seq[i].get()->actor);
      if(it == vipInfoMap.end() || (it->second).empty()) {
        f = ExpandFeedData(feed_seq[i].get(), &u, owner, (WeiredSource)render, likeinfo, from, emptyVipData, &output, stat_log, isAdmin);
      }
      else {
        f = ExpandFeedData(feed_seq[i].get(), &u, owner, (WeiredSource)render, likeinfo, from, it->second, &output, stat_log, isAdmin);
      }

      //MCE_INFO("expand feed data fid:" << feed_seq[i].get()->feed << " u:" << (&u)->id() << " result:" << f);
      if (f) {
        ++ feed_count;
        ++ from;
        html->push_back(output);
        stat += stat_log;
        //stat += "\n"; 
      }
    }

#if 1
    try {
      //xce::feed::FeedStatLogPrx prx = CachedFeedStatLogAdapter(uid);
      //LOG(INFO) << "prx:" << prx;
      //if (!prx) {
        //LOG(INFO) << "feedviewI.cc::expand --> FeedStatLog prx is NULL !";
        //MCE_WARN("feedviewI.cc::expand --> FeedStatLog prx is NULL !");
      //} else {
        std::vector<std::string> cate;
        cate.push_back("feed");
        cate.push_back("stat");
        //LOG(INFO) << "stat:" << stat;
        //prx->AddLog(cate, stat);
        FeedStatLogAdapter::instance().AddLog(cate, stat);
      //}
    } catch (Ice::Exception& e) {
      //LOG_F(WARNING) << e;
      MCE_WARN("feedviewI.cc::expand -->" << e);
    }
#endif
    // ASSERT(feed_count == feed_seq.size());

    if (pt.wall_clock() > 10)
      XAR_INC(ExpandGT10);
  }
  return feed_count;
}

bool FeedViewI::ReloadCSTemplate(const Ice::Current&) {
  int is_test = MyUtil::ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedViewN.IsTest", 0);
  return FeedTplCache::Instance().Reload(is_test);
}

bool FeedViewI::ReloadDataSchema(const Ice::Current&) {
  int is_test = MyUtil::ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedViewN.IsTest", 0);
  return FeedSchemaCache::Instance().Reload(is_test);
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

  MCE_INFO("FeedViewI::GetByType --> uid:" << uid);
  //LOG(INFO) << "connection: " << cur.con->toString();  
  try {
    ptime pt("getFeed", false);

    // feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(uid);

    if (types.empty()) {
      // feed_seq = fprx->getFeedData(uid, from, count);
    } else {
      // TODO : stop TEST. feed_seq = fprx->getFeedDataByType(uid, types, from, count);
      feed::FeedDataResPtr feed_res;
      feed_res = FeedNewsReplicaAdapter::instance().GetHotFeedDataByType(uid, types, from, count);
      feed_seq = feed_res->data;
    }

    if (pt.wall_clock() > 30)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << e;
    MCE_WARN("FeedViewI::GetByType() -->" << e);
  }

  //LOG(INFO) << "GetByType() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq.size() << '/' << count;
  MCE_INFO("FeedViewI::GetByType() --> uid :" << uid << " type.size :" << types.size() << " count :" << feed_seq.size() << '/' << count);

  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(0, uid, name, render, feed_seq, from, &ret);

  return ret;
}

feed::HtmlResult FeedViewI::GetByStype(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types
    , Ice::Int from, Ice::Int count, const Ice::Current& cur) {
  XAR_INC(NewsS);

  feed::FeedDataSeq feed_seq;
  //LOG(INFO) << "connection: " << cur.con->toString(); 
  MCE_INFO("FeedViewI::GetBySType --> uid :" << uid);
  try {
    ptime pt("getFeedByStype", false);
    // feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(uid);
    // TODO : stop test. feed_seq = fprx->getFeedDataByStype(uid, types, from, count);
    // feed_seq = FeedNewsReplicaAdapter::instance().GetHotFeedDataBySType(uid, types, from, count);
    feed::FeedDataResPtr feed_res = FeedNewsReplicaAdapter::instance().GetHotFeedDataByStype(uid, types, from, count);

    feed_seq = feed_res->data;

    if (pt.wall_clock() > 3)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    LOG_F(WARNING) << e;
  }

  //LOG(INFO) << "GetByStype() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq.size() << '/' << count;
  MCE_INFO("FeedViewI::GetBySType() --> uid :" << uid << " type.size :" << types.size() << " count :" << feed_seq.size() << '/' << count);
  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(0, uid, name, render, feed_seq, from, &ret);
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
  //LOG(INFO) << "GetByIds2: " << uid << " " << name << " " << render;
  if (fids.empty()) {
    MCE_INFO("FeedViewI::GetByIds2() --> GetByIds2: uid=" << uid << " " << name << " " << render << " size=0");
  } else {
    MCE_INFO("FeedViewI::GetByIds2() --> GetByIds2: uid=" << uid << " " << name << " " << render << " id[0]=" << fids[0]);
  }

////////////////
  render = 0;
  //LOG(INFO) << "connection: " ;
  feed::FeedDataSeq feed_seq;

  try {
    //LOG(INFO) << "before fprx.";
   // feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(uid);
    //feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    //LOG(INFO) << "fprx: " << fprx;
    //feed_seq = fprx->GetFeedDataByIds(fids);
    //LOG(INFO) << "feed_seq.size:" << feed_seq.size();
    feed_seq = FeedNewsReplicaAdapter::instance().GetFeedDataByIds(fids);
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedViewI::GetByIds2() --> uid=" << uid << " "<< e); 
  }
  
  MCE_INFO("FeedViewI::GetByIds2() --> uid=" << uid  << feed_seq.size()); 

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  if (!(render & WS_MINIFEED)) {
    render |= WS_HOME;
  }
  //LOG(INFO) << "before expand.";
  ExpandUserFeeds(0, uid, name, render, feed_seq, 0, &ret);
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
  //LOG(INFO) << "GetByIdsAndStype2: " << uid << " " << name << " " << render << " " << stype << " " << firstID;
  MCE_INFO("FeedViewI::GetByIdsAndStype2 --> " << uid << " " << name << " " << render << " " << stype << " " << firstID);
////////////////
  render = 0;
  //LOG(INFO) << "connection: " ;
  feed::FeedDataSeq feed_seq;

  try {
    ptime pt("getFeedById", false);
    //LOG(INFO) << "before fprx.";
   // feed::FeedItemManagerPrx fprx = CachedFeedItemAdapter(uid);
    //feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    //feed::FeedNewsManagerPrx fprx = CachedFeedNews3Adapter(uid);
    //feed_seq = fprx->GetFeedDataByIdsWithUid(uid, fids, firstID, stype);
    feed_seq = FeedNewsReplicaAdapter::instance().GetFeedDataByIdsWithUid(uid, fids, firstID, stype);
 
    MCE_INFO("FeedViewI::GetByIdsAndStype2 --> feed_seq.size:" << feed_seq.size() << " firstID:" << firstID << " stype:" << stype);

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
  ExpandUserFeeds(0, uid, name, render, feed_seq, 0, &ret);
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

  struct timeval start, end;
  
  try {
    ptime pt("getFeed", false);
    //feed::FeedItemManagerPrx fprx = CachedFeedMiniReplicaAdapter(owner);
    //LOG(VERBOSE) << "owner: " << owner << " uid: " << uid << " proxy: " << fprx;
    MCE_INFO("FeedViewI::GetMiniByType --> owner: " << owner << " uid: " << uid);

    gettimeofday(&start, NULL);

    if (types.empty()) {
     //feed_seq = fprx->getFeedData(owner, from, count);
      feed_seq = FeedMiniReplicaAdapter::instance().getFeedData(owner, from, count);
      MCE_INFO("Get all types");
    } else {
      //feed_seq = fprx->getFeedDataByType(owner, types, from, count);
      for (size_t i = 0; i < types.size(); ++i) {
        MCE_INFO("type[" << i << '/' << types.size() << "]= " << types[i]);
      }
      feed_seq = FeedMiniReplicaAdapter::instance().getFeedDataByType(owner, types, from, count);
    }
    if (pt.wall_clock() > 3)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    MCE_WARN("FeedViewI::GetMiniByType -->  owner:" << owner << " uid:" << uid << " exception:" << e);
  }

  gettimeofday(&end, NULL);
  double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;

  MCE_INFO("FeedViewI::GetMiniByType -->  owner:" << owner << " uid:" << uid << " type.size:" << types.size() << " count:" << feed_seq.size() << '/' << count << " ice call dur:" << timeuse / 1000000);

  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  gettimeofday(&start, NULL);
  int expand_feed_suc = ExpandUserFeeds(owner, uid, name, render|WS_MINIFEED, feed_seq, from, &ret);
  gettimeofday(&end, NULL);
  timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;

  MCE_INFO("FeedViewI::GetMiniByType -->  owner:" << owner << " uid:" << uid << " expand dur:" << timeuse / 1000000 << " expand_feed_suc:" << expand_feed_suc);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniByStype(Ice::Int owner, Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types
    , Ice::Int from, Ice::Int count, const Ice::Current&) {
  XAR_INC(MiniS);

  feed::FeedDataSeq feed_seq;
  
  MCE_INFO("FeedViewI::GetMiniByStype --> uid=" << uid);
  try {
    ptime pt("getFeedByStype", false);
    //feed::FeedItemManagerPrx fprx = CachedFeedMiniReplicaAdapter(owner);
    //feed_seq = fprx->getFeedDataByStype(owner, types, from, count);
    feed_seq = FeedMiniReplicaAdapter::instance().getFeedDataByStype(owner, types, from, count);

    if (pt.wall_clock() > 3)
      XAR_INC(FeedGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    //LOG_F(WARNING) << "GetMiniByStype() uid=" << uid << " error:" << e;
    MCE_WARN("FeedViewI::GetMiniByStype --> uid=" << uid << " error:" << e);
  }

  //LOG(INFO) << "GetMiniByStype() uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq.size() << '/' << count;
  MCE_INFO("FeedViewI::GetMiniByStype --> uid=" << uid << " type.size=" << types.size() << " count=" << feed_seq.size() << '/' << count);

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(owner, uid, name, render|WS_MINIFEED, feed_seq, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetFavorite(Ice::Int uid, const std::string& name
    , Ice::Int render, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataSeq feed_seq;
  
  MCE_INFO("FeedViewI::GetFavorite --> uid:" << uid);
  try {
    ptime pt("getFeed", false);
    //feed::FocusManagerPrx fprx = CachedFeedFocusAdapter(uid);

    //feed_seq = fprx->GetFocusFeedData(uid, from, count);
    feed_seq = feed::FeedFocusAdapter::instance().GetFocusFeedData(uid, from, count);

    if (pt.wall_clock() > 30)
      XAR_INC(FocusGT30);
  } catch (Ice::Exception & e) {
    XAR_INC(FeedErr);
    //LOG_F(WARNING) << "GetFavorite() uid=" << uid << " error:" << e;
    MCE_WARN("FeedViewI::GetFavorite --> uid:" << uid << " error:" << e);
  }

  MCE_INFO("FeedViewI::GetFavorite --> uid:" << uid << " count:" << feed_seq.size() << '/' << count);

  
  if (feed_seq.empty())
   return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(0, uid, name, render | WS_FAVORITE, feed_seq, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetHot(Ice::Int uid, const std::string& name
    , Ice::Int render, Ice::Int from, Ice::Int count, const Ice::Current&) {

  feed::FeedDataResPtr feed_seq;
  FeedDataSeq exp_feed;
 /* 
  try {
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
     //feed::FeedNewsManagerPrx fprx = CachedFeedNews3Adapter(uid);
    feed_seq = fprx->GetHotFeedData(uid, from, count);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetHot() uid=" << uid << " error:" << e;
    return kEmptyResult;
  }
*/

  MCE_INFO("FeedViewI::GetHot --> uid:" << uid); 
  
  struct timeval start, end;
  gettimeofday(&start, NULL);
  try{
    feed_seq = FeedNewsReplicaAdapter::instance().getHotFeedData(uid, from, count);
  }catch(Ice::Exception& e){
    MCE_WARN("FeedViewI::GetHot --> uid:" << uid << " error:" << e);
    gettimeofday(&end, NULL);
    double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    MCE_WARN("FeedViewI::GetHot -->  uid:" << uid << " ice call error.  dur:" << timeuse / 1000000);
    //return kEmptyResult;
    if(!feed_seq || feed_seq->data.empty()) {
       if(!FeedCache::instance().Get(uid, exp_feed)) {
         MCE_WARN("FeedViewI::GetHot --> FeedCache.Get() failed. uid:" << uid );
         return kEmptyResult;
       }    
       MCE_INFO("FeedViewI::GetHot --> FeedCache.Get() success. uid:" << uid );
    }
  }catch (std::exception & e) {
    MCE_WARN("FeedViewI::GetHot --> uid:" << uid << " error:" << e.what());
    gettimeofday(&end, NULL);
    double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    MCE_WARN("FeedViewI::GetHot -->  uid:" << uid << " ice call error.  dur:" << timeuse / 1000000);
    //return kEmptyResult;
    if(!feed_seq || feed_seq->data.empty()) {
      if(!FeedCache::instance().Get(uid, exp_feed)) {
        MCE_WARN("FeedViewI::GetHot --> FeedCache.Get() failed. uid:" << uid );
        return kEmptyResult;
      }    
      MCE_INFO("FeedViewI::GetHot --> FeedCache.Get() success. uid:" << uid );
    }
  }

  if (feed_seq && !feed_seq->data.empty()){
    exp_feed = feed_seq->data;
    MCE_INFO("FeedViewI::GetHot --> FeedCache add. uid:" << uid);
    if(!FeedCache::instance().Add(uid, exp_feed)) {
      MCE_WARN("FeedViewI::GetHot --> FeedCache::instance().Add(" << uid << ", feed_seq) failed!" );
    }
  }
  MCE_INFO("FeedViewI::GetHot --> uid:" << uid << " count=" << exp_feed.size() << '/' << count); 

  if (exp_feed.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  //ExpandUserFeeds(0, uid, name, render | WS_HOT, feed_seq, from, &ret);
  gettimeofday(&start, NULL);
  int expand_feed_suc = ExpandUserFeeds(0, uid, name, render | WS_HOT, exp_feed, from, &ret);
  gettimeofday(&end, NULL);
  double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
  MCE_INFO("FeedViewI::GetHot --> uid:" << uid << " expand dur:" << timeuse / 1000000 << " expand_feed_suc:" <<  expand_feed_suc);
  return ret;
}

feed::HtmlResult FeedViewI::GetHotByType(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types
    , Ice::Int from, Ice::Int count, const Ice::Current& cur) {
  feed::FeedDataResPtr feed_seq;
  MCE_INFO("FeedViewI::GetHotByType uid=" << uid);

  FeedDataSeq exp_feed;

  struct timeval start, end;
  gettimeofday(&start, NULL);
  
  try {
     //feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
     //feed_seq = fprx->GetHotFeedDataByType(uid, types, from, count);
    feed_seq = FeedNewsReplicaAdapter::instance().GetHotFeedDataByType(uid, types, from, count);
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedViewI::GetHotByType --> uid:" << uid << " error:" << e);

    gettimeofday(&end, NULL);
    double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    MCE_INFO("FeedViewI::GetHotByType -->  uid:" << uid << " ice call error.  dur:" << timeuse / 1000000);

    //added by qun.liu 20110414
    if(!feed_seq || feed_seq->data.empty()) {
      MCE_INFO("FeedViewI::GetHotByType() --> feed_seq is empty.  uid:" << uid);
      if(!FeedCache::instance().Get(uid, exp_feed)) {
        MCE_INFO("FeedViewI::GetHotByType() --> FeedCache.Get() failed. uid:" << uid );
        return kEmptyResult;
      }
      MCE_INFO("FeedViewI::GetHotByType() --> FeedCache.Get() success. uid:" << uid );
    } 
  } catch (std::exception & e) {
    MCE_WARN("FeedViewI::GetHotByType --> uid:" << uid << " error:" << e.what());

    gettimeofday(&end, NULL);
    double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    MCE_INFO("FeedViewI::GetHotByType -->  uid:" << uid << " ice call error.  dur:" << timeuse / 1000000);

    //added by qun.liu 20110414
    if(!feed_seq || feed_seq->data.empty()) {
      MCE_INFO("FeedViewI::GetHotByType() --> feed_seq is empty.  uid:" << uid);
      if(!FeedCache::instance().Get(uid, exp_feed)) {
        MCE_INFO("FeedViewI::GetHotByType() --> FeedCache.Get() failed. uid:" << uid );
        return kEmptyResult;
      }
    } 
    if (feed_seq) {
      MCE_INFO("FeedViewI::GetHotByType() ice call error, use FeedCache. feed_seq size:" << feed_seq->data.size() << " uid:" << uid);
    }
  }

  if (feed_seq && !feed_seq->data.empty()) {
    exp_feed = feed_seq->data;
    MCE_INFO("FeedViewI::GetHotByType() --> FeedCache add. uid:" << uid);
    if(!FeedCache::instance().Add(uid, exp_feed)) {
      MCE_WARN("FeedViewI::GetHotByType() --> FeedCache::instance().Add(" << uid << ", feed_seq) failed!" );
    }
  }

  MCE_INFO("FeedViewI::GetHotByType --> uid:" << uid << " count:" << exp_feed.size() << '/' << count);

  //if (feed_seq->data.empty())
  if (exp_feed.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  //ExpandUserFeeds(0, uid, name, render | WS_HOT, feedSeq, from, &ret);
  gettimeofday(&start, NULL);

  //ExpandUserFeeds(0, uid, name, render | WS_HOT, feed_seq->data, from, &ret);
  int expand_feed_suc = ExpandUserFeeds(0, uid, name, render | WS_HOT, exp_feed, from, &ret);

  gettimeofday(&end, NULL);
  double timeuse = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
  MCE_INFO("FeedViewI::GetHotByType --> uid:" << uid << " expand dur:" << timeuse / 1000000 << " expand_feed_suc:" <<  expand_feed_suc);
  return ret;
}

feed::HtmlResult FeedViewI::GetHotByStype(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types, Ice::Int from, Ice::Int count, const Ice::Current&) {

  feed::FeedDataResPtr feed_seq;
  MCE_INFO("FeedViewI::GetHotByStype --> uid:" << uid); 
  
  try {
    feed_seq = FeedNewsReplicaAdapter::instance().GetHotFeedDataByStype(uid, types, from, count);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetHotByStype() uid=" << uid << " error:" << e;
    MCE_WARN("FeedViewI::GetHotByStype --> uid:" << uid << " error:" << e);
    feed_seq = NULL;
    //return kEmptyResult;
  }

  MCE_INFO("FeedViewI::GetHotByStype --> uid:" << uid << " type.size=" << types.size() << " count=" << feed_seq->data.size() << '/' << count);
  if (feed_seq->data.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  //ExpandUserFeeds(0, uid, name, render | WS_HOT, feedSeq, from, &ret);
  ExpandUserFeeds(0, uid, name, render | WS_HOT, feed_seq->data, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetLive(Ice::Int uid, const std::string& name
    , Ice::Int render, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataResPtr feed_seq;
  MCE_INFO("FeedViewI::GetLive --> uid:" << uid); 
  
  try {
    //feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    //feed_seq = fprx->GetLiveFeedData(uid, from, count);
    feed_seq = FeedNewsReplicaAdapter::instance().getLiveFeedData(uid, from, count);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetLive() uid=" << uid << " error:" << e;
    MCE_WARN("FeedViewI::GetLive --> uid:" << uid << " error:" << e);
    feed_seq = NULL;
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetLive() uid=" << uid << " count=" << feed_seq->data.size() << '/' << count;
  MCE_INFO("FeedViewI::GetLive --> uid:" << uid << " count=" << feed_seq->data.size() << '/' << count);

  
  //if (feed_seq->data.empty())
  //  return kEmptyResult;

  feed::HtmlResult ret;
  //ExpandUserFeeds(0, uid, name, render | WS_LIVE, feedSeq, from, &ret);
  ExpandUserFeeds(0, uid, name, render | WS_LIVE, feed_seq->data, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetLiveByType(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& types, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataResPtr feed_seq;
  MCE_INFO("FeedViewI::GetLiveByType --> uid:" << uid); 
  
  try {
    //feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    //feed_seq = fprx->GetLiveFeedDataByType(uid, types, from, count);
    feed_seq = FeedNewsReplicaAdapter::instance().getLiveFeedDataByType(uid, types, from, count);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetLiveByType() uid=" << uid << " error:" << e;
    MCE_WARN("FeedViewI::GetLiveByType --> uid:" << uid << " error:" << e);
    feed_seq = NULL;
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetLiveByType() uid=" << uid << " count=" << feed_seq->data.size() << '/' << count;
  MCE_INFO("FeedViewI::GetLiveByType --> uid:" << uid << " count=" << feed_seq->data.size() << '/' << count);

  
  //if (feed_seq->data.empty())
  //  return kEmptyResult;

  feed::HtmlResult ret;
  //ExpandUserFeeds(0, uid, name, render | WS_LIVE, feedSeq, from, &ret);
  ExpandUserFeeds(0, uid, name, render | WS_LIVE, feed_seq->data, from, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetLiveByStype(Ice::Int uid, const std::string& name
    , Ice::Int render, const ::xce::feed::TypeSeq& stypes, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataResPtr feed_seq;
 
  MCE_INFO("FeedViewI::GetLiveByStype --> uid:" << uid); 
  try {
    //feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
    //feed_seq = fprx->GetLiveFeedDataByStype(uid, stypes, from, count);
    feed_seq = FeedNewsReplicaAdapter::instance().getLiveFeedDataByStype(uid, stypes, from, count);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetLiveByStype() uid=" << uid << " error:" << e;
    MCE_WARN("FeedViewI::GetLiveByStype --> uid:" << uid << " error:" << e);
    feed_seq = NULL;
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetLiveByStype() uid=" << uid << " count=" << feed_seq->data.size() << '/' << count;
  MCE_INFO("FeedViewI::GetLiveByStype --> uid:" << uid << " count=" << feed_seq->data.size() << '/' << count);

  
  if (feed_seq->data.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  //ExpandUserFeeds(0, uid, name, render | WS_LIVE, feedSeq, from, &ret);
  ExpandUserFeeds(0, uid, name, render | WS_LIVE, feed_seq->data, from, &ret);
  return ret;
}
feed::HtmlResult FeedViewI::GetMiniGroupFeedData(Ice::Int gid, const std::string& name,  Ice::Int render, Ice::Int memberid, Ice::Int begin, Ice::Int limit, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedData(gid, memberid, begin, limit);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetMiniGroupFeedData() gid=" << gid << " memberid:" << memberid << " error:" << e;
    MCE_WARN("GetMiniGroupFeedData() gid:" << gid << " memberid:" << memberid << " error:" << e);
    feed_seq.clear();
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetMiniGroupFeedData() gid=" << gid << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit;
  MCE_INFO("GetMiniGroupFeedData() gid:" << gid << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit);
  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(gid, memberid, name, render | WS_MINIFEED, feed_seq, begin, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniGroupFeedDataByType(Ice::Int gid, const std::string& name,  Ice::Int render,  Ice::Int memberid, const ::xce::feed::IntSeq& stypes, Ice::Int begin, Ice::Int limit, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedDataByType(gid, memberid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetMiniGroupFeedDataByType() gid=" << gid  << " memberid:" << memberid << " error:" << e;
    MCE_WARN("GetMiniGroupFeedDataByType() gid=" << gid  << " memberid:" << memberid << " error:" << e);
    feed_seq.clear();
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetMiniGroupFeedDataByType() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit;
  MCE_INFO("GetMiniGroupFeedDataByType() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit);

  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(gid, memberid, name, render | WS_MINIFEED, feed_seq, begin, &ret);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniGroupFeedDataByStype(Ice::Int gid, const std::string& name, Ice::Int render, Ice::Int memberid, const ::xce::feed::IntSeq& stypes, Ice::Int begin, Ice::Int limit, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedDataByStype(gid, memberid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetMiniGroupFeedDataBySType() gid=" << gid  << " memberid:" << memberid << " error:" << e;
    MCE_WARN("GetMiniGroupFeedDataBySType() gid=" << gid  << " memberid:" << memberid << " error:" << e);
    feed_seq.clear();
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetMiniGroupFeedDataBySType() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit;
  MCE_INFO("GetMiniGroupFeedDataBySType() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit);

  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(gid, memberid, name, render | WS_MINIFEED, feed_seq, begin, &ret);
  return ret;
}


feed::HtmlResult FeedViewI::GetMiniGroupFeedData2CS(Ice::Int gid, const std::string& name,  Ice::Int render, Ice::Int memberid, Ice::Int begin, Ice::Int limit, bool isAdmin,  Ice::Int view, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedData(gid, memberid, begin, limit);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetMiniGroupFeedData2() gid=" << gid << " memberid:" << memberid << " error:" << e;
    MCE_WARN("GetMiniGroupFeedData2CS() gid=" << gid << " memberid:" << memberid << " error:" << e);
    feed_seq.clear();
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetMiniGroupFeedData2() gid=" << gid << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin;
  MCE_INFO("GetMiniGroupFeedData2CS() gid=" << gid << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin << " render:" << render << " view:" << view);

  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(gid, memberid, name, render | WS_MINIFEED| WS_MINIGROUP, feed_seq, begin, &ret, isAdmin, view);
  return ret;
}


feed::HtmlResult FeedViewI::GetMiniGroupFeedData2(Ice::Int gid, const std::string& name,  Ice::Int render, Ice::Int memberid, Ice::Int begin, Ice::Int limit, bool isAdmin, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedData(gid, memberid, begin, limit);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetMiniGroupFeedData2() gid=" << gid << " memberid:" << memberid << " error:" << e;
    MCE_WARN("GetMiniGroupFeedData2() gid=" << gid << " memberid:" << memberid << " error:" << e);
    feed_seq.clear();
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetMiniGroupFeedData2() gid=" << gid << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin;
  MCE_INFO("GetMiniGroupFeedData2() gid=" << gid << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin << " render:" << render);

  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(gid, memberid, name, render | WS_MINIFEED| WS_MINIGROUP, feed_seq, begin, &ret, isAdmin);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniGroupFeedDataByType2(Ice::Int gid, const std::string& name,  Ice::Int render,  Ice::Int memberid, const ::xce::feed::IntSeq& stypes, Ice::Int begin, Ice::Int limit, bool isAdmin, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedDataByType(gid, memberid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetMiniGroupFeedDataByType2() gid=" << gid  << " memberid:" << memberid << " error:" << e;
    MCE_WARN("GetMiniGroupFeedDataByType2() gid=" << gid  << " memberid:" << memberid << " error:" << e);
    feed_seq.clear();
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetMiniGroupFeedDataByType2() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin;
  MCE_INFO("GetMiniGroupFeedDataByType2() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin);

  
  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(gid, memberid, name, render | WS_MINIFEED| WS_MINIGROUP, feed_seq, begin, &ret, isAdmin);
  return ret;
}

feed::HtmlResult FeedViewI::GetMiniGroupFeedDataByStype2(Ice::Int gid, const std::string& name, Ice::Int render, Ice::Int memberid, const ::xce::feed::IntSeq& stypes, Ice::Int begin, Ice::Int limit, bool isAdmin, const Ice::Current&) {

  feed::FeedDataSeq feed_seq;
  try {
    feed::FeedGroupManagerPrx fprx =  CachedFeedGroupAdapter(gid);
    feed_seq = fprx->GetFeedDataByStype(gid, memberid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    //LOG_F(WARNING) << "GetMiniGroupFeedDataBySType2() gid=" << gid  << " memberid:" << memberid << " error:" << e;
    MCE_WARN("GetMiniGroupFeedDataBySType2() gid=" << gid  << " memberid:" << memberid << " error:" << e);
    feed_seq.clear();
    //return kEmptyResult;
  }

  //LOG(INFO) << "GetMiniGroupFeedDataBySType2() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin;
  MCE_INFO("GetMiniGroupFeedDataBySType2() gid=" << gid  << " memberid:" << memberid << " count=" << feed_seq.size() << '/' << limit << " isAdmin:" << isAdmin);

  if (feed_seq.empty())
    return kEmptyResult;

  feed::HtmlResult ret;
  ExpandUserFeeds(gid, memberid, name, render | WS_MINIFEED | WS_MINIGROUP, feed_seq, begin, &ret, isAdmin);
  return ret;
}

static int ExpandNewFeedOnly(Ice::Int owner, Ice::Int uid, const std::string& name, Ice::Int render
    , feed::FeedDataSeq & feed_seq, Ice::Int view = 0) {
  MCE_INFO("ExpandNewFeedOnly() feed seq size : " << feed_seq.size());
  if (feed_seq.empty()) {
    return -1;
  }

  // 得到“赞” ilike 信息
  ::xce::ilikeit::LikeInfoSeq likeseq;
  std::vector<char> vbool(0, feed_seq.size()); // 有些类型并不需要 zan

  cwf::User u(name, uid);
  for (size_t i = 0; i<feed_seq.size(); ++i) {
    if (WeiredType(feed_seq[i]->type).version <= 0) {
      MCE_DEBUG("Old version feed doesn't expand.");
      continue;
    }

   const vipinfo::VipFeedDataSeq emptyVipData;

    std::string output;
    std::string stat_log;
    bool f = ExpandFeedData(feed_seq[i].get(), &u, owner, (WeiredSource)render, NULL, 0, emptyVipData, &output, stat_log, false, view);
    MCE_INFO("expand new feeds only data fid:" << feed_seq[i].get()->feed << " u:" << (&u)->id() << " result:" << f);
    if (f) {
      // html->push_back(output);
      feed_seq[i]->xml = output;
    } 
  }
  return 0;
}

feed::CSTplCheckRes FeedViewI::CheckCSTemplate(const std::string& tpl, const Ice::Current&) {
  feed::CSTplCheckRes res;
  int code = xce::notify::CSTemplate::CheckTpl(tpl, &res.reason);
  res.code = code;
  MCE_INFO("check tpl ------" << tpl << "--------- res : " << code << " " << res.reason);
  return res;
}

feed::FeedDataResPtr FeedViewI::GetHotDataByStype(::Ice::Int uid, const std::string& name, ::Ice::Int render,
    const ::xce::feed::TypeSeq& stypes, 
    ::Ice::Int begin, ::Ice::Int limit, 
    const ::Ice::Current&) {
  feed::FeedDataResPtr feed_res;

  try {
    feed_res = FeedNewsReplicaAdapter::instance().GetHotFeedDataByStype(uid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedNewsReplicaAdapter::GetHotFeedDataByStype() error : " << e);
    return NULL;
  }

  if (feed_res) {
    feed::FeedDataSeq & feed_seq  = feed_res->data;
    ExpandNewFeedOnly(0, uid, name, render, feed_seq);
  }

  return feed_res;
}

feed::FeedDataResPtr FeedViewI::GetLiveDataByStype(::Ice::Int uid, const std::string& name, ::Ice::Int render,
    const ::xce::feed::TypeSeq& stypes, 
    ::Ice::Int begin, ::Ice::Int limit, 
    const ::Ice::Current&) {
  feed::FeedDataResPtr feed_res;

  try {
    feed_res = FeedNewsReplicaAdapter::instance().getLiveFeedDataByStype(uid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedNewsReplicaAdapter::GetHotFeedDataByStype() error : " << e);
    return NULL;
  }

  if (feed_res) {
    feed::FeedDataSeq & feed_seq  = feed_res->data;
    ExpandNewFeedOnly(0, uid, name, render, feed_seq);
  }

  return feed_res;
}

feed::FeedDataSeq FeedViewI::GetMiniDataByStype(::Ice::Int uid, const std::string& name, ::Ice::Int render,
    const ::xce::feed::TypeSeq& stypes, 
    ::Ice::Int begin, ::Ice::Int limit, 
    const ::Ice::Current&) {
  feed::FeedDataSeq feed_seq;

  try {
    feed_seq = FeedMiniReplicaAdapter::instance().getFeedDataByStype(uid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedNewsReplicaAdapter::GetHotFeedDataByStype() error : " << e);
  }

  ExpandNewFeedOnly(0, uid, name, render, feed_seq);

  return feed_seq;
}


feed::FeedDataResPtr FeedViewI::GetHotDataByStype2(::Ice::Int uid, const std::string& name, ::Ice::Int render,
    const ::xce::feed::TypeSeq& stypes, 
    ::Ice::Int begin, ::Ice::Int limit, ::Ice::Int view, 
    const ::Ice::Current&) {
  feed::FeedDataResPtr feed_res;

  MCE_INFO("FeedViewI::GetHotDataByStype2 --> uid:" << uid);
  try {
    feed_res = FeedNewsReplicaAdapter::instance().GetHotFeedDataByStype(uid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedNewsReplicaAdapter::GetHotFeedDataByStype() error : " << e);
    return NULL;
  }

  if (feed_res) {
    feed::FeedDataSeq & feed_seq  = feed_res->data;
    ExpandNewFeedOnly(0, uid, name, render, feed_seq, view);
  }

  return feed_res;
}

feed::FeedDataResPtr FeedViewI::GetLiveDataByStype2(::Ice::Int uid, const std::string& name, ::Ice::Int render,
    const ::xce::feed::TypeSeq& stypes, 
    ::Ice::Int begin, ::Ice::Int limit, ::Ice::Int view, 
    const ::Ice::Current&) {
  feed::FeedDataResPtr feed_res;

  MCE_INFO("FeedViewI::GetLiveDataByStype2 --> uid:" << uid);
  try {
    feed_res = FeedNewsReplicaAdapter::instance().getLiveFeedDataByStype(uid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedNewsReplicaAdapter::GetHotFeedDataByStype() error : " << e);
    return NULL;
  }

  if (feed_res) {
    feed::FeedDataSeq & feed_seq  = feed_res->data;
    ExpandNewFeedOnly(0, uid, name, render, feed_seq, view);
  }

  return feed_res;
}

feed::FeedDataSeq FeedViewI::GetMiniDataByStype2(::Ice::Int uid, const std::string& name, ::Ice::Int render,
    const ::xce::feed::TypeSeq& stypes, 
    ::Ice::Int begin, ::Ice::Int limit, ::Ice::Int view, 
    const ::Ice::Current&) {
  feed::FeedDataSeq feed_seq;

  MCE_INFO("FeedViewI::GetMiniDataByStype2 --> uid:" << uid);
  try {
    feed_seq = FeedMiniReplicaAdapter::instance().getFeedDataByStype(uid, stypes, begin, limit);
  } catch (Ice::Exception & e) {
    MCE_WARN("FeedNewsReplicaAdapter::GetHotFeedDataByStype() error : " << e);
  }

  ExpandNewFeedOnly(0, uid, name, render, feed_seq, view);

  return feed_seq;
}

feed::FeedDataSeq FeedViewI::GetFavorite2(::Ice::Int uid, const std::string& name 
    , ::Ice::Int render, ::Ice::Int begin, ::Ice::Int limit, ::Ice::Int view, const Ice::Current&) {
  feed::FeedDataSeq feed_seq;
  
  MCE_INFO("FeedViewI::GetFavorite2 --> uid:" << uid);
  try {
    feed_seq = feed::FeedFocusAdapter::instance().GetFocusFeedData(uid, begin, limit);
    MCE_INFO("FeedViewI::GetFavorite2 --> uid:" << uid << " begin:"<< begin << " limit:" << limit << " feedseq size:" << feed_seq.size());
  } catch (Ice::Exception & e) { 
    MCE_WARN("FeedViewI::GetFavorite2 --> uid:" << uid << " error:" << e);
  }

  ExpandNewFeedOnly(0, uid, name, render, feed_seq, view);
  
  return feed_seq;
}

feed::FeedDataSeq FeedViewI::GetFavorite2ByStype(::Ice::Int uid, const std::string& name 
    , ::Ice::Int render, const ::xce::feed::TypeSeq& stypes, ::Ice::Int begin, ::Ice::Int limit, ::Ice::Int view, const Ice::Current&) {
  feed::FeedDataSeq feed_seq;
  
  MCE_INFO("FeedViewI::GetFavorite2ByStype --> uid:" << uid << "begin: " << begin << "limit: " << limit);
  try {
    feed_seq = feed::FeedFocusAdapter::instance().GetFocusFeedDataByStype(uid, begin, limit, stypes);
    MCE_INFO("FeedViewI::GetFavorite2ByStype --> uid:" << uid << "feedseq size: " << feed_seq.size());
  } catch (Ice::Exception & e) { 
    MCE_WARN("FeedViewI::GetFavorite2ByStype --> uid:" << uid << " error:" << e);
  }

  ExpandNewFeedOnly(0, uid, name, render, feed_seq, view);
  
  return feed_seq;
}

feed::FeedDataSeq FeedViewI::GetByIds2View(Ice::Int uid, const std::string& name, Ice::Int render
    , const feed::FeedIdSeq & fids, Ice::Int view, const Ice::Current& cur) {
  feed::FeedDataSeq feed_seq;
  if (fids.empty()) {
    MCE_INFO("FeedViewI::GetByIds2View --> uid:" << uid << " size=0");
  }
 
  try {
    feed_seq = FeedNewsReplicaAdapter::instance().GetFeedDataByIds(fids);
  } catch (Ice::Exception & e) { 
    MCE_WARN("FeedViewI::GetByIds2View --> uid:" << uid << " "<< e);  
  }
  
  MCE_INFO("FeedViewI::GetByIds2View --> uid:" << uid  << feed_seq.size()); 

  ExpandNewFeedOnly(0, uid, name, render, feed_seq, view);
  return feed_seq; 
}

feed::FeedDataSeq FeedViewI::GetGreaterThanFeedidWithUseridByType(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit, 
  ::Ice::Int uid, const ::xce::feed::TypeSeq& types, ::Ice::Int render, Ice::Int view, const ::Ice::Current&) {
  return feed::FeedDataSeq();
}

void FeedViewI::Test(Ice::Int uid, const std::string& name
    , Ice::Int render, Ice::Int from, Ice::Int count, const Ice::Current&) {
  feed::FeedDataResPtr feed_seq;

  MCE_INFO("FeedViewI::Test --> uid:" << uid); 
  
  try {
    feed::FeedNewsManagerPrx fprx = CachedFeedNews2Adapter(uid);
     //feed::FeedNewsManagerPrx fprx = CachedFeedNews3Adapter(uid);
    feed_seq = fprx->GetHotFeedData(uid, from, count);
  } catch (Ice::Exception & e) {
    LOG_F(WARNING) << "GetHot() uid:" << uid << " error:" << e;
    feed_seq = NULL;
    //return ;
  }

/*
  try{ 
    feed_seq = FeedNewsReplicaAdapter::instance().getHotFeedData(uid, from, count);
  }catch(Ice::Exception& e){
    MCE_WARN("FeedViewI::Test --> uid:" << uid << " error:" << e);
    return ;
  }
*/
  MCE_INFO("FeedViewI::Test--> uid:" << uid << " count=" << feed_seq->data.size() << '/' << count); 
  //LOG(INFO) << "GetHot() uid=" << uid << " count=" << feed_seq->data.size() << '/' << count;

  
  if (feed_seq->data.empty())
    return ;

  feed::HtmlResult ret;
  ExpandUserFeeds(0, uid, name, render | WS_HOT, feed_seq->data, from, &ret);
  //ExpandUserFeeds(0, uid, name, render | WS_HOT, feedSeq, from, &ret);
  //return ret; 
}

}

/*
BEGIN_SERVANT_MAP("feedview")
  SERVANT_ENTRY(feedview, &xce::ServantFactory< xce::FeedViewI >::CreateServant, &xce::ServantFactory< xce::FeedViewI >::Destroy)
END_SERVANT_MAP()

// 静态函数可能添加不上，需要main内部再显示添加一次
static void Init() {
  ctemplate::Template::SetTemplateRootDirectory("/data/xce/XFeed/etc/feed_view_tpl3");
  ctemplate::Template::AddAlternateTemplateRootDirectory("/data/cwf2/tpl3");

}

GLOBAL_INIT(FEEDVIEWI, {
  xce::AddStartup(&Init, true);
});

*/
