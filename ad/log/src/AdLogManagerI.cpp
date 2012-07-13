#include "AdLogManagerI.h"
//#include "AdUnion/gate/ad_logger.h"
#include "ad_logger.h"

using namespace std;
using namespace IceUtil;
using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace MyUtil;

void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdLogManagerI::instance(), service.createIdentity("M", ""));
//  InitAdLogger("ad", "../log/" + ServiceI::instance().getName() + "/ad_log", "INFO");
  InitAdPVLogger("ad_pv", "../log/" + ServiceI::instance().getName() + "/pv_val", "INFO");
  InitAdClickLogger("ad_click", "../log/" + ServiceI::instance().getName() + "/click_val", "INFO");
  InitAdChargeLogger("ad_charge", "../log/" + ServiceI::instance().getName() + "/charge_val", "INFO");
  InitAdPvChargeLogger("ad_pv_charge", "../log/" + ServiceI::instance().getName() + "/pv_charge_val", "INFO");

  // InitAdEngineLogLogger("ad_engine_log", "../log/" + ServiceI::instance().getName() + "/stat/stat", "INFO");
  InitAdStatLogger("ad_stat", "../log/" + ServiceI::instance().getName() + "/stat_val", "INFO");
  TaskManager::instance().scheduleRepeated(new AdLogTimer());
}

void AdLogManagerI::ClickForBp(const string& log,const Ice::Current& current){
  ADCK_INFO(log);
}

void AdLogManagerI::ClickForCharge(const string& log,const Ice::Current& current){
  ADCH_INFO(log);
}

void AdLogManagerI::PvForCharge(const ::MyUtil::StrSeq& logs,const Ice::Current& current){
  for (::MyUtil::StrSeq::const_iterator it = logs.begin(); it != logs.end(); it++) {
    ADPC_INFO(*it);
  }
}
void AdLogManagerI::Pv(const string& log, const Ice::Current& current){
  ADPV_INFO(log);
}

void AdLogManagerI::PvBatch(const ::MyUtil::StrSeq& logs, const Ice::Current& current){
  for (::MyUtil::StrSeq::const_iterator it = logs.begin(); it != logs.end(); it++) {
    ADPV_INFO(*it);
  }
}

void AdLogManagerI::EngineLog(const string& log, const Ice::Current& current){
  // AD_ENGINE_LOG_INFO(log);
  AD_STAT_INFO(log);
}

void AdLogManagerI::EngineLogBatch(const ::MyUtil::StrSeq& logs, const Ice::Current& current){
  for (::MyUtil::StrSeq::const_iterator it = logs.begin(); it != logs.end(); it++) {
    // AD_ENGINE_LOG_INFO(*it);
    AD_STAT_INFO(*it);
  }
}

//void AdLogManagerI::EngineClick(const string& log, const Ice::Current& current){
//  AD_ENGINE_PV_INFO(log);
//}



