#include <vector>
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "DistBuddyByIdCacheReloaderI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include <QueryRunner.h>

using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace MyUtil;
using namespace mysqlpp;
using namespace std;
using namespace xce::buddy;
using namespace xce::distcache;
using namespace xce::distcache::buddybyidcache;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&DistBuddyByIdCacheReloaderI::instance(), 
    service.createIdentity(DIST_BUDDYBYID_CACHE_RELOADER, ""));
  TaskManager::instance().config(TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_RELOAD, MyUtil::ThreadPoolConfig(1, 10));
  TaskManager::instance().config(TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_STATUS_UPDATE, MyUtil::ThreadPoolConfig(1, 1)); 
  TaskManager::instance().config(TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_ADD, MyUtil::ThreadPoolConfig(1, 1));
  TaskManager::instance().config(TASK_LEVEL_DIST_BUDDY_BYID_CACHE_RELOADER_REMOVE, MyUtil::ThreadPoolConfig(1, 1));
 
  int mod = service.getCommunicator()->getProperties()
    ->getPropertyAsIntWithDefault("DistBuddyByIdCacheReloader.Mod", 0);
  int interval = service.getCommunicator()->getProperties()
    ->getPropertyAsIntWithDefault("DistBuddyByIdCacheReloader.Interval", 5);
  ServerStateSubscriber::instance().initialize("ControllerDistBuddyByIdCacheReloader", 
    &DistBuddyByIdCacheReloaderI::instance(), mod, interval);
  DistBuddyByIdCacheReloaderI::instance().setValid(true);

  StatusCacheInitThreadPtr init = new StatusCacheInitThread;
  init->start(1024*1024).detach();

  DistBuddyByIdCacheAdapter::instance().initialize();
}

void DistBuddyByIdStatusCache::reload(int id){
  MyUtil::IntSeq ids;
  ids.push_back(id);
  DistBuddyByIdCacheReloaderI::instance().reload(ids);
}

bool DistBuddyByIdCacheReloaderI::isValid(const Ice::Current&) {
  IceUtil::RWRecMutex::RLock lock(validmutex_);
  return valid_;
}

void DistBuddyByIdCacheReloaderI::setValid(bool valid, const Ice::Current&) {
	ostringstream oss;
	oss << "DistBuddyByIdCacheReloaderI::setValid " << valid;
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	IceUtil::RWRecMutex::WLock lock(validmutex_);
	valid_ = valid;
	if(valid_) {
		xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
	} else {
		xce::serverstate::ServerStateSubscriber::instance().setStatus(0);
	}
}

void DistBuddyByIdCacheReloaderI::reload(const MyUtil::IntSeq& ids, const Ice::Current& current) {
	ostringstream oss;
	oss << "DistBuddyByIdCacheReloaderI::reload size = " << ids.size() << " " << current.con->toString().substr(current.con->toString().find("remote", 25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::IntSeq tempids;
	for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if(*it > 0) {
			tempids.push_back(*it);
		}
	}
	MyUtil::TaskManager::instance().execute(new ReloadTask(tempids, current));
}

void DistBuddyByIdCacheReloaderI::add(::Ice::Int host,::Ice::Int guest, const Ice::Current& current) {
  MyUtil::TaskManager::instance().execute(new AddTask(host,guest,current));
}

void DistBuddyByIdCacheReloaderI::remove(::Ice::Int host,::Ice::Int guest, const Ice::Current& current) {
  MyUtil::TaskManager::instance().execute(new RemoveTask(host,guest,current));
}

void ReloadTask::handle() {
  TimeCost tc = TimeCost::create("ReloadTask::handle", TimeCost::LOG_LEVEL_INFO);
  int page = ids_.size()/DIST_BUDDY_BYID_CACHE_RELOADER_CHUNCK_SIZE + 1;
  for(int i=0; i< page; ++i){
    MyUtil::IntSeq::iterator end = 
      ids_.begin()+i*DIST_BUDDY_BYID_CACHE_RELOADER_CHUNCK_SIZE > ids_.end() -DIST_BUDDY_BYID_CACHE_RELOADER_CHUNCK_SIZE ? 
      ids_.end() : ids_.begin()+i*DIST_BUDDY_BYID_CACHE_RELOADER_CHUNCK_SIZE;
    MyUtil::IntSeq tmpids(ids_.begin()+i*DIST_BUDDY_BYID_CACHE_RELOADER_CHUNCK_SIZE,end);
    MyUtil::ObjectResultPtr res = ((MemFactory)DistBuddyByIdFactory::instance()).create(tmpids);
    DistBuddyByIdCacheAdapter::instance().set(res);
  }
}

void AddTask::handle() {
  ostringstream oss;
  oss << "DistBuddyByIdCacheReloaderI::add " << host_ << " " << guest_ << current_.con->toString().substr(current_.con->toString().find("remote", 25));
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  BuddyByIdCacheDataPtr olddata = DistBuddyByIdCacheAdapter::instance().get(host_);
  BuddyByIdCacheDataPtr newdata = new BuddyByIdCacheData(); 
  if(!olddata || !newdata){
    MCE_WARN("add " << host_ << " " << guest_ << " get null object , will return.");
    MyUtil::IntSeq tempids;
    tempids.push_back(host_);
    MyUtil::TaskManager::instance().execute(new ReloadTask(tempids,current_));
    return;
  }

  bool inserted = false;
  for(int i = 0; i< olddata->friend__size();i++){
    if( !inserted && olddata->friend_(i) > guest_ ){
      newdata->add_friend_(guest_); 
      inserted = true;  
    }
    newdata->add_friend_(olddata->friend_(i));
  }
  newdata->set_size(newdata->friend__size());
  DistBuddyByIdCacheAdapter::instance().set(host_,newdata);
}

void RemoveTask::handle() {
  ostringstream oss;
  oss << "DistBuddyByIdCacheReloaderI::remove " << host_ << " " << guest_ << current_.con->toString().substr(current_.con->toString().find("remote", 25));
  TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
  BuddyByIdCacheDataPtr olddata = DistBuddyByIdCacheAdapter::instance().get(host_);
  BuddyByIdCacheDataPtr newdata = new BuddyByIdCacheData();
  if(!olddata || !newdata){
    MCE_WARN("remove " << host_ << " " << guest_ << " get null object , will return.");
    MyUtil::IntSeq tempids;
    tempids.push_back(host_);
    MyUtil::TaskManager::instance().execute(new ReloadTask(tempids,current_));
    return;
  }

  for(int i = 0; i< olddata->friend__size();i++){
    if(olddata->friend_(i) == guest_ ) {
      continue;
    }
    newdata->add_friend_(olddata->friend_(i));
  }
  newdata->set_size(newdata->friend__size());
  DistBuddyByIdCacheAdapter::instance().set(host_,newdata);
}

void DistBuddyByIdCacheReloaderI::statusUpdate( ::Ice::Int userId, ::Ice::Int status, const Ice::Current& current ){
  MCE_INFO( "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] DistBuddyByIdCacheReloaderI::statusUpdate userId=" << userId << " status=" << status );
  DistBuddyByIdStatusCache::instance().set( userId , status );
  MyUtil::TaskManager::instance().execute( new StatusUpdateTask( userId, status, current ) );
}

void StatusUpdateTask::handle(){
  
  BuddyByIdCacheDataPtr obj;
  try {
    obj = BuddyByIdCacheDataPtr::dynamicCast(DistBuddyByIdFactory::instance().createAllFriends(_id));
  } catch (Ice::Exception& e) {
    MCE_WARN("[StatusUpdateTask] Ice::Exception (updater=" << _id << ") : " << e.what());
    return;
  } catch (mysqlpp::Exception& e) {
    MCE_WARN("[StatusUpdateTask] mysqlpp::Exception (updater=" << _id << ") : " << e.what());
    return;
  } catch (std::exception& e) {
    MCE_WARN("[StatusUpdateTask] std::excetpion (updater=" << _id << ") : " << ": " << e.what());
    return;
  } catch (...) {
    MCE_WARN("[StatusUpdateTask] unknown exception (updater=" << _id << ") ." );
    return;
  }
  MyUtil::IntSeq ids;
  ids.push_back(_id);
  for(int i = 0; i< obj->friend__size(); ++i){
    ids.push_back(obj->friend_(i));
  }
  MyUtil::TaskManager::instance().execute(new ReloadTask(ids,current_));
}

Ice::ObjectPtr DistBuddyByIdFactory::createAllFriends(::Ice::Int id){
  BuddyByIdCacheDataPtr result = new BuddyByIdCacheData;
  MyUtil::IntSeq friends;
  com::xiaonei::xce::Statement sql;
  sql     << "SELECT guest"
    << " FROM relation_" << (id % 100)
    << " WHERE host = " << id;
  ostringstream pattern;
  pattern << "relation_" << (id % 100);
  com::xiaonei::xce::QueryRunner("relation_buddycache", com::xiaonei::xce::CDbRServer, pattern.str()).query(
      sql, BuddyByIdCacheResultHandlerI( friends ));
  for(MyUtil::IntSeq::iterator it = friends.begin(); it!=friends.end(); ++it){
    result->add_friend_(*it);
  }
  result->set_size(result->friend__size());
  return result;
}
