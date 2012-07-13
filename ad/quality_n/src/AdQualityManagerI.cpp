/*
 * AdQualityI.cpp
 *
 *  Created on: 2011-04-12
 *      Author: zhizhao.sun
 */

#include "AdQualityManagerI.h"
#include "ServiceI.h"
#include "Date.h"
#include "client/linux/handler/exception_handler.h"
#include "AdQualityCache.h"
#include "AdConfig.h"

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
  service.getAdapter()->add(&xce::ad::AdQualityManagerI::instance(), service.createIdentity("M",""));

  string index = service.getCommunicator()->getProperties()->getPropertyWithDefault("ServerIndex", "0");
  xce::ad::AdConfig::instance().set_server_index(index);
  MyUtil::TaskManager::instance().schedule(new xce::ad::AdConfig::ConfigLoadTimer(0 * 1000));

  AdQualityCache::instance().load();
  TaskManager::instance().scheduleRepeated(new QualityCacheReloadTimer());
}

namespace xce {
namespace ad {
using namespace std;
using namespace boost;

AdQualityUMap AdQualityManagerI::GetQualityUsByZone(Ice::Long zone_id, const ::Ice::Current&) {
  TimeStat ts;
  AdQualityUMap aqm;
  aqm = AdQualityCache::instance().GetQualityUByZone(zone_id);
  MCE_INFO("GetQualitysByZone() aqm.size=" << aqm.size() << "  used_time=" << ts.getTime());
  return aqm;
}


/***********************************************
 * 以下接口为了兼容老版本，保留，在以后重构时考虑去除
 ***********************************************/
AdQualityMap AdQualityManagerI::GetQualitysByZone(Ice::Long zone_id, const ::Ice::Current&) {
  TimeStat ts;
  AdQualityMap aqm;
  return aqm;
}
AdQualityMap AdQualityManagerI::GetQualitys(const string &, const ::Ice::Current&) {
  TimeStat ts;
  AdQualityMap aqm;
  return aqm;
}


AdQualityUMap AdQualityManagerI::GetQualityUs(const string &, const ::Ice::Current&) {
  TimeStat ts;
  AdQualityUMap aqm;
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

}
}
