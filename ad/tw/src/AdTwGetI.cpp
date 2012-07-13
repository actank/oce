/*
 * AdTwGetI.cpp
 *
 *  Created on: 2010-9-13
 *      Author: ark
 */

#include "AdTwGetI.h"
#include "IceLogger.h"
#include "ServiceI.h"
#include "AdUrlUtil.h"
#include "client/linux/handler/exception_handler.h"
#include "AdUrlMaping.h"
#include "AdData.h"
#include "AdTwDebugI.h"

bool MyCallback(const char *dump_path,
                const char *minidump_id,
                void *context,
                bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}

void MyUtil::initialize() {
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
	ServiceI& service = ServiceI::instance();
  xce::ad::AdTwConfig::instance().set_DB(service.getCommunicator()->getProperties()->getPropertyWithDefault("DbInstance", "ade_db"));
  xce::ad::AdTwConfig::instance().set_server_name(service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ServerIndex", 0));
	service.getAdapter()->add(&xce::ad::AdTwGetI::instance(),
			service.createIdentity("M", ""));
	service.getAdapter()->add(&xce::ad::AdTwDebugI::instance(),
			service.createIdentity("D", ""));

	TaskManager::instance().schedule(new xce::ad::AdGroupLoadTimer(0));
	TaskManager::instance().schedule(new xce::ad::TwZoneLoadTimer(0));
	TaskManager::instance().schedule(new xce::ad::AdUrlMapingLoadTimer(0));
  xce::ad::LowQualityAdCache::instance().LoadGidsEcpmLevelPool();
  TaskManager::instance().scheduleRepeated(new xce::ad::LowQualityAdCacheTimer());
//加载模板对应类型
  xce::ad::Widget2TypeCache::instance().Load();
  TaskManager::instance().scheduleRepeated(new xce::ad::Widget2TypeCacheTimer());
  //TaskManager::instance().scheduleRepeated(new xce::ad::TwEngineParametersTimer());
}

namespace xce {
namespace ad {

AdTwGetI::AdTwGetI() {
	// TODO Auto-generated constructor stub

}

AdTwGetI::~AdTwGetI() {
	// TODO Auto-generated destructor stub
}

int AdTwGetI::index = 0;

AdResultMap AdTwGetI::GetAds(const string& userProfile,
                             const string& referer,
                             int rotate_index,
                             const Ice::Current&) {
  try {
    MCE_DEBUG("Enter GetAds: referer = " << referer);
    AdResultMap result;
    index = 0; // (index+1)%1;
    result = AdUrlUtil::Get(userProfile, referer, index);
    return result;
  } catch(Ice::Exception& e) {
    MCE_WARN("AdTwGetI::GetAds error : " << e << ", referer = " << referer);
  } catch(std::exception& e) {
    MCE_WARN("AdTwGetI::GetAds std::exception error : " << e.what() << ", referer = " << referer);
  } catch(...) {
    MCE_WARN("AdTwGetI::GetAds unknown error : " << "referer = " << referer);
  }
  return AdResultMap();
}

void AdTwGetI::Click(int uid,
                     const string& ip,
                     const string& text,
                     const string& referer,
                     const string& click_url,
                     const Ice::Current& ) {
  try {
    MCE_DEBUG("Enter AdTwGetI::Click");
    AdUrlUtil::Click(uid, ip, text, referer, click_url);
  } catch(Ice::Exception& e) {
    MCE_WARN("AdTwGetI::Click error : " << e << ", uid = " << uid);
  } catch(std::exception& e) {
    MCE_WARN("AdTwGetI::Click std::exception error : " << e.what() << ", uid = " << uid);
  } catch(...) {
    MCE_WARN("AdTwGetI::Click unknown error : " << "uid = " << uid);
  }
}

AdResultMap AdTwGetI::GetAdsTest(Ice::Int uid,
                                 Ice::Int stage,
                                 Ice::Int gender,
                                 Ice::Int age,
                                 const ::std::string& currentArea,
                                 const ::std::string& ipArea,
                                 Ice::Long school,
                                 Ice::Int grade,
                                 int count,
                                 const string & referer,
                                 int rotate_index,
                                 const Ice::Current&) {
  try {
    MCE_DEBUG("Enter GetAds: referer = " << referer);
    AdResultMap result;
    index = (index+1)%4;
    result = AdUrlUtil::GetTest(uid, stage, gender, age, ipArea, school,  grade, count, referer, index);
    return result;
  } catch(Ice::Exception& e) {
    MCE_WARN("AdTwGetI::GetAdsTest error : " << e << ", referer = " << referer);
  } catch(std::exception& e) {
    MCE_WARN("AdTwGetI::GetAdsTest std::exception error : " << e.what() << ", referer = " << referer);
  } catch(...) {
    MCE_WARN("AdTwGetI::GetAdsTest unknown error : " << "referer = " << referer);
  }
  return AdResultMap();
}

AdZone2Creatives AdTwGetI::GetAdsByZone(const ::std::string&, const ::std::string&, ::Ice::Int, const ::Ice::Current&) {
  return AdZone2Creatives();
}

}
}
