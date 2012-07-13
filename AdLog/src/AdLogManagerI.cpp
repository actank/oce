#include "AdLogManagerI.h"
#include "AdUnion/gate/ad_logger.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace IceUtil;
using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace MyUtil;

void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdLogManagerI::instance(), service.createIdentity("M", ""));
  InitAdLogger("ad", "../log/" + ServiceI::instance().getName() + "/ad_log", "INFO");
  InitAdPVLogger("ad_pv", "../log/" + ServiceI::instance().getName() + "/pv_val", "INFO");
  InitAdClickLogger("ad_click", "../log/" + ServiceI::instance().getName() + "/click_val", "INFO");
  TaskManager::instance().scheduleRepeated(new AdLogTimer());
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Service.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service.Interval",5);
  xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerAdLog",&AdLogManagerI::instance(),mod,interval, new AdChannel());
  MCE_INFO("MyUtil::initialize. mod:" << mod << " interval:" << interval);
}

void AdLogManagerI::ClickForBp(const string& log,const Ice::Current& current){
  ADCK_INFO(log);
}

void AdLogManagerI::ClickForCharge(const string& log,const Ice::Current& current){
  AD_INFO(log);
}

void AdLogManagerI::PvForCharge(const ::MyUtil::StrSeq& logs,const Ice::Current& current){
}

void AdLogManagerI::Pv(const string& log, const Ice::Current& current){
//  ADPV_INFO(log);
  PvLogHandler::instance().PushBack(log);
}
void AdLogManagerI::PvBySeq(const MyUtil::StrSeq& logs, const Ice::Current& current){
//  for(MyUtil::StrSeq::const_iterator it = logs.begin(); it != logs.end(); ++it){
//    ADPV_INFO(*it);
//  }
  PvLogHandler::instance().PushBack(logs);

}

Ice::Int AdLogManagerI::UpdateOnceCount(const Ice::Int count,
    const Ice::Current& current) {
  MCE_DEBUG("Enter UpdateOnceCount --> old count = "
      << PvLogHandler::instance().once_count() << ", new count = " << count);
  PvLogHandler::instance().set_once_count(count);
  return PvLogHandler::instance().once_count();
}

void PvLogHandler::PushBack(const MyUtil::StrSeq& logs) {
  IceUtil::Mutex::Lock lock(mutex_);
  for (MyUtil::StrSeq::const_iterator it = logs.begin(); it != logs.end(); ++it) {
    queue_.push(*it);
  }
}

bool PvLogHandler::Execute() {
  ostringstream log;
  int count = 0;
  {
    IceUtil::Mutex::Lock lock(mutex_);
    MCE_DEBUG("Enter Execute --> queue size = " << queue_.size());
    if (queue_.empty() || queue_.size() < once_count_) {
      return false;
    }
    for (int i = 0; i < once_count_; i++) {
      log << queue_.front();
      queue_.pop();
      count++;
      if (queue_.empty()) {
        break;
      }
      if (i < once_count_ - 1) {
        log << "\n";
      }
    }
    /* if (queue_.empty()) {
      return false;
    }
    for (int i = 0; i < ONCE_COUNT; i++) {
      log << queue_.front();
      queue_.pop();
      count++;
      if (queue_.empty()) {
        break;
      }
      if (i < ONCE_COUNT - 1) {
        log << "\n";
      }
    } */
    MCE_DEBUG("Exit Execute --> queue size = " << queue_.size() << ", this time count = " << count);
  }
  ADPV_INFO(log.str());
  return true;
}

void AdLogManagerI::PvBatch(const ::MyUtil::StrSeq& logs, const Ice::Current& current){
}

void AdLogManagerI::EngineLog(const string& log, const Ice::Current& current){
}

void AdLogManagerI::EngineLogBatch(const ::MyUtil::StrSeq& logs, const Ice::Current& current){
}

