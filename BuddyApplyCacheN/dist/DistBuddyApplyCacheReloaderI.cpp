#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "DistBuddyApplyCacheReloaderI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include <QueryRunner.h>

using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace MyUtil;
using namespace mysqlpp;
using namespace std;
using namespace xce::buddyapplycache;
using namespace xce::distcache;
using namespace xce::distcache::buddyapplycache;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&DistBuddyApplyCacheReloaderI::instance(), 
      service.createIdentity(DIST_BUDDYAPPLY_CACHE_RELOADER, ""));
  TaskManager::instance().config(TASK_LEVEL_RELOAD, MyUtil::ThreadPoolConfig(1, 10));
  TaskManager::instance().config(TASK_LEVEL_ADD, MyUtil::ThreadPoolConfig(1, 10));
  TaskManager::instance().config(TASK_LEVEL_REMOVE, MyUtil::ThreadPoolConfig(1, 5));

  int mod = service.getCommunicator()->getProperties()
    ->getPropertyAsIntWithDefault("DistBuddyApplyCacheReloader.Mod", 0);
  int interval = service.getCommunicator()->getProperties()
    ->getPropertyAsIntWithDefault("DistBuddyApplyCacheReloader.Interval", 5);
  ServerStateSubscriber::instance().initialize("ControllerDistBuddyApplyCacheReloader", 
      &DistBuddyApplyCacheReloaderI::instance(), mod, interval);
  DistBuddyApplyCacheReloaderI::instance().setValid(true);

  DistBuddyApplyCacheAdapter::instance().initialize();
}

bool DistBuddyApplyCacheReloaderI::isValid(const Ice::Current&) {
  IceUtil::RWRecMutex::RLock lock(validmutex_);
  return valid_;
}

void DistBuddyApplyCacheReloaderI::setValid(bool valid, const Ice::Current&) {
  ostringstream oss;
  oss << "DistBuddyApplyCacheReloaderI::setValid " << valid;
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  IceUtil::RWRecMutex::WLock lock(validmutex_);
  valid_ = valid;
  if(valid_) {
    xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
  } else {
    xce::serverstate::ServerStateSubscriber::instance().setStatus(0);
  }
}

void DistBuddyApplyCacheReloaderI::reload(const MyUtil::IntSeq& ids, const Ice::Current& current) {
  ostringstream oss;
  oss << "DistBuddyApplyCacheReloaderI::reload size = " << ids.size() << " " << current.con->toString().substr(current.con->toString().find("remote", 25));
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  MyUtil::IntSeq tempids;
  for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if(*it > 0) {
      tempids.push_back(*it);
    }
  }
  MyUtil::TaskManager::instance().execute(new ReloadTask(tempids, current));
}

void DistBuddyApplyCacheReloaderI::add(int accepter, const BuddyApplyCacheDataN& applicant,const Ice::Current& current){
  ostringstream oss;
  oss << "DistBuddyApplyCacheReloaderI::add " << accepter << " " << applicant.applicant << current.con->toString().substr(current.con->toString().find("remote", 25));
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  BuddyApplyCacheDataPtr olddata = DistBuddyApplyCacheAdapter::instance().get(accepter);
  BuddyApplyCacheDataPtr newdata = new BuddyApplyCacheData;
  if(!olddata && !newdata){
    return;
  }

  ApplyData* item = newdata->add_item();
  item->set_applicant(applicant.applicant);
  item->set_time(applicant.time);
  item->set_why(applicant.why);
  item->set_statfrom(applicant.statfrom);
  item->set_groupname(applicant.groupname);

  for(int i = 0; i< olddata->item_size() && i < 99 ;i++){
    if(olddata->item(i).applicant() != applicant.applicant){
      ApplyData* item = newdata->add_item();
      item->CopyFrom(olddata->item(i));
    }
  }
  DistBuddyApplyCacheAdapter::instance().set(accepter,newdata);
}

void DistBuddyApplyCacheReloaderI::remove(int accepter, int applicant, const Ice::Current& current){
  ostringstream oss;
  oss << "DistBuddyApplyCacheReloaderI::remove " << accepter << " " << applicant << current.con->toString().substr(current.con->toString().find("remote", 25));
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  BuddyApplyCacheDataPtr olddata = DistBuddyApplyCacheAdapter::instance().get(accepter);
  BuddyApplyCacheDataPtr newdata = new BuddyApplyCacheData;
  if(!olddata && !newdata){
    return;
  }

  for(int i = 0; i< olddata->item_size() && i < 99 ;i++){
    if(olddata->item(i).applicant() != applicant){
      ApplyData* item = newdata->add_item();
      item->CopyFrom(olddata->item(i));
    }
  }
  DistBuddyApplyCacheAdapter::instance().set(accepter,newdata);
}

ReloadTask::ReloadTask(const MyUtil::IntSeq& ids, const Ice::Current& current) : 
  MyUtil::Task(TASK_LEVEL_RELOAD),ids_(ids), current_(current) {
  }

void ReloadTask::handle() {
  TimeCost tc = TimeCost::create("ReloadTask::handle", TimeCost::LOG_LEVEL_INFO);
  MyUtil::ObjectResultPtr res = ((MemFactory)BuddyApplyFactory::instance()).create(ids_);
  DistBuddyApplyCacheAdapter::instance().set(res);
}
