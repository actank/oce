/*
 * AdQualityI.cpp
 *
 *  Created on: 2010-12-6
 *      Author: zhaohui.tang
 */

#include "AdQualityManagerI.h"
#include "ServiceI.h"
#include "Date.h"
#include "client/linux/handler/exception_handler.h"
#include "AdQualityCache.h"
#include "AdCache.h"

using namespace com::xiaonei::xce;
using namespace xce::ad;
using namespace MyUtil;
using namespace std;

bool MyCallback(const char *dump_path, const char *minidump_id, void *context, bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}

void ::MyUtil::initialize() {
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  /* Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  MCE_DEBUG("MyUtil::initialize --> Ice.MessageSizeMax" << props->getProperty("Ice.MessageSizeMax"));
  MCE_DEBUG("MyUtil::initialize --> Ice.ThreadPool.Server.StackSize" << props->getProperty("AdQuality0.ThreadPool.StackSize")); */
  service.getAdapter()->add(&xce::ad::AdQualityManagerI::instance(), service.createIdentity("M",""));
  TaskManager::instance().execute(new InitTask());
  TaskManager::instance().scheduleRepeated(new QualityCacheReloadTimer());
}

namespace xce {
namespace ad {
using namespace std;
using namespace boost;
AdQualityMap AdQualityManagerI::GetQualitysByZone(Ice::Long zone_id, const ::Ice::Current&) {
  TimeStat ts;
  AdQualityMap aqm;
  if (init_ok()) {
    aqm = AdQualityCache::instance().GetQualityByZone(zone_id);
  }
  MCE_INFO("GetQualitysByZone() aqm.size=" << aqm.size() << "  used_time=" << ts.getTime());
  return aqm;
}
AdQualityMap AdQualityManagerI::GetQualitys(const string &, const ::Ice::Current&) {
  TimeStat ts;
  AdQualityMap aqm;
  if (init_ok()) {
    try {
      aqm = AdQualityCache::instance().GetAllQualities();
    } catch (std::exception& e) {
      MCE_WARN("AdQualityManagerI::GetQualitys error : " << e.what());
    }
  }
  MCE_INFO("AdQualityManagerI::GetQualitys() aqm.size = " << aqm.size() << ", used time = " << ts.getTime());
  return aqm;
}

AdQualityUMap AdQualityManagerI::GetQualityUsByZone(Ice::Long zone_id, const ::Ice::Current&) {
  TimeStat ts;
  AdQualityUMap aqm;
  if (init_ok()) {
    aqm = AdQualityCache::instance().GetQualityUByZone(zone_id);
  }
  MCE_INFO("GetQualitysByZone() aqm.size=" << aqm.size() << "  used_time=" << ts.getTime());
  return aqm;
}
AdQualityUMap AdQualityManagerI::GetQualityUs(const string &, const ::Ice::Current&) {
  TimeStat ts;
  AdQualityUMap aqm;
  if (init_ok()) {
    try {
      aqm = AdQualityCache::instance().GetAllQualitieUs();
    } catch (std::exception& e) {
      MCE_WARN("AdQualityManagerI::GetQualitys error : " << e.what());
    }
  }
  MCE_INFO("AdQualityManagerI::GetQualitys() aqm.size = " << aqm.size() << ", used time = " << ts.getTime());
  return aqm;
}
void AdQualityManagerI::update(const AdStatMap & stat_map, const ::Ice::Current&) {
  MCE_INFO("AdQualityManagerI::update --> Enter, stat_map size = " << stat_map.size());
  try {
    AdQualityCache::instance().update(stat_map);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdQualityManagerI::update error : " << e);
  } catch (std::exception& e) {
    MCE_WARN("AdQualityManagerI::update error : " << e.what());
  }
}

string AdQualityManagerI::GetLastUpdateTime(const ::Ice::Current&) {
  string ans = "-1";
  return ans;
}

void AdQualityManagerI::reload(const ::Ice::Current&) {
  MCE_INFO("AdQualityManagerI::reload --> Enter");
  try {
    AdQualityCache::instance().load();
  } catch (std::exception& e) {
    MCE_WARN("AdQualityManagerI::reload error : " << e.what());
  }
}

void AdQualityManagerI::Init() {
  MCE_INFO("AdQualityManagerI::Init --> begin init :" << Date(time(NULL)).toDateTime());
  AdMemberPool::instance().Init();
  MCE_INFO("AdQualityManagerI::Init --> AdMemberPool::instance().Init(); DONE");
  AdCampaignPool::instance().Init();
  MCE_INFO("AdQualityManagerI::Init-->  AdCampaignPool::instance().Init(); DONE");
  AdGroupPool::instance().Init();

  MCE_INFO("AdQualityManagerI::Init -->  AdGroupPool::instance().Init(); DONE");
  AdQualityCache::instance().load();
  MCE_INFO("AdQualityManagerI::Init -->  AdQualityCache Init; DONE");
  set_init_ok(true);
  MCE_INFO("AdQualityManagerI::Init -->  Init, DONE success!");
}

void InitTask::handle() {
  AdQualityManagerI::instance().Init();
}

}
}
