/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantCacheI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月24日 14时23分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "BuddyByApplicantCacheI.h"
#include <ServiceI.h>
#include "util/cpp/TimeCost.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BuddyByApplicantAdapter.h"
#include "buddy/BuddyByApplicantCache/share/BuddyByApplicantFactory.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using namespace MyUtil;
using namespace xce::buddy;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;
using namespace xce::buddy::adapter;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.registerObjectCache(BUDDY_BYAPPLICANT_DATA_I, "BA", &BuddyByApplicantCacheFactory::instance());

  service.getAdapter()->add(&BuddyByApplicantCacheManagerI::instance(), service.createIdentity("M", ""));

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  //----------------------------HA----------------------------------------------
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByApplicantCache.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByApplicantCache.Interval", 5);

  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyByApplicantCache",
      &BuddyByApplicantCacheManagerI::instance(), mod, interval);
}

//-----------------------------------------BuddyByApplicantCacheManagerI---------------------------------------
BuddyByApplicantDataPtr BuddyByApplicantCacheManagerI::getAccepters(Ice::Int id, Ice::Int begin, Ice::Int limit, 
    const Ice::Current& current) {
  ostringstream msg;
  msg << " id=" << id
    << " begin=" << begin << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyByApplicantDataIPtr data = locateBuddyByApplicantCache(id);
  if (data) {
    return data->get(id, begin, limit);
  } 
  return new BuddyByApplicantData;
}

BuddyByApplicantDataPtr BuddyByApplicantCacheManagerI::getAcceptersSync(Ice::Int id, Ice::Int begin, Ice::Int limit,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " id=" << id
    << " begin=" << begin << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  BuddyByApplicantDataIPtr dataI = findBuddyByApplicantCache(id);
  if(!dataI){
	  BuddyByApplicantDataPtr data = BuddyByApplicantDataPtr::dynamicCast(BuddyByApplicantFactory::instance().create(id));
	  reload(id, data);
	  dataI = locateBuddyByApplicantCache(id);
  }
  return dataI->get(id, begin, limit);
}

void BuddyByApplicantCacheManagerI::addAccepter(Ice::Int id, const AccepterInfo& accepter,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " id=" << id
    << " accepter=" << accepter.accepter;
  
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyByApplicantDataIPtr data = findBuddyByApplicantCache(id);
  if (data) {
    data->add(accepter);
  } else {
    syncAddAccepter(id, accepter);
  }
  return;
}

void BuddyByApplicantCacheManagerI::removeAccepter(Ice::Int id, Ice::Int accepterId,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " id=" << id
    << " accepter=" << accepterId;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyByApplicantDataIPtr data = locateBuddyByApplicantCache(id);
  if (data) {
    data->remove(accepterId);
  } 
  return;
}

void BuddyByApplicantCacheManagerI::reload(Ice::Int id, const Ice::ObjectPtr& data, const Ice::Current& current) {
  ostringstream msg;
  msg << " id=" << id;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  ServiceI::instance().getObjectCache()->addObject(BUDDY_BYAPPLICANT_DATA_I, id, new BuddyByApplicantDataI(BuddyByApplicantDataPtr::dynamicCast(data)));
}

void BuddyByApplicantCacheManagerI::clear(Ice::Int id, const Ice::Current& current) {
  ostringstream msg;
  msg << " id=" << id;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  locateBuddyByApplicantCache(id)->clear();
}

BuddyByApplicantDataIPtr BuddyByApplicantCacheManagerI::locateBuddyByApplicantCache(Ice::Int id) {
  return ServiceI::instance().locateObject<BuddyByApplicantDataIPtr> (
      BUDDY_BYAPPLICANT_DATA_I, id);
}

BuddyByApplicantDataIPtr BuddyByApplicantCacheManagerI::findBuddyByApplicantCache(Ice::Int id) {
  return ServiceI::instance().findObject<BuddyByApplicantDataIPtr> (
      BUDDY_BYAPPLICANT_DATA_I, id);
}

void BuddyByApplicantCacheManagerI::syncAddAccepter(Ice::Int id, const AccepterInfo& accepter) {
  BuddyByApplicantDataIPtr data = new BuddyByApplicantDataI(BuddyByApplicantDataPtr::dynamicCast(BuddyByApplicantFactory::instance().create(id)));
  data->add(accepter);
  ServiceI::instance().getObjectCache()->addObject(BUDDY_BYAPPLICANT_DATA_I, id, data);
  MCE_INFO( "syncAddAccepter done. id/ObjectCache size : " << id << " / " << ServiceI::instance().getObjectCache()->locateCategory( BUDDY_BYAPPLICANT_DATA_I)->size() );
}

bool BuddyByApplicantCacheManagerI::isValid(const Ice::Current&) {
  MCE_INFO( "[isValid] " << _isValid );
  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _isValid;
}

void BuddyByApplicantCacheManagerI::setValid(bool valid,const Ice::Current&) {
  MCE_INFO( "[setValid] " << valid );
  IceUtil::RWRecMutex::WLock lock(_validMutex);
  _isValid = valid;
  if( valid ){
    ServerStateSubscriber::instance().setStatus(1);
  } else{
    ServerStateSubscriber::instance().setStatus(0);
  }
}

void BuddyByApplicantCacheManagerI::setData(const MyUtil::ObjectResultPtr& userData, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  MyUtil::ObjectResultPtr results = new MyUtil::ObjectResult;
  for (map<long, Ice::ObjectPtr>::const_iterator i = userData->data.begin(); i != userData->data.end(); ++i) {
    results->data.insert(make_pair(i->first, new BuddyByApplicantDataI(BuddyByApplicantDataPtr::dynamicCast(i->second))));
  }
  ServiceI::instance().getObjectCache()->addObjects(BUDDY_BYAPPLICANT_DATA_I, results->data);
  MCE_INFO( "setData done. setData size / ObjectCache size : " << userData->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory( BUDDY_BYAPPLICANT_DATA_I)->size() );
}
//-----------------------------------------BuddyByApplicantCacheManagerI End-----------------------------------
//
//-----------------------------------------BuddyByApplicantDataI-----------------------------------------------
BuddyByApplicantDataI::BuddyByApplicantDataI() {
  _data = new BuddyByApplicantData;
  _data->totalCount = 0;
  _extra = false;
  _basicTime = time(0);
  setExtra(_data->accepters.size() == ACCEPTER_SIZE_LIMIT ? true : false);
}

BuddyByApplicantDataI::BuddyByApplicantDataI(const BuddyByApplicantDataPtr& data) {
  _data = new BuddyByApplicantData;
  _data->accepters.swap(data->accepters);
  _data->totalCount = data->totalCount;
  _extra = false;
  _basicTime = time(0);
  setExtra(_data->accepters.size() == ACCEPTER_SIZE_LIMIT ? true : false);
}

BuddyByApplicantDataPtr BuddyByApplicantDataI::get(int id, int begin, int limit) {

  time_t now = time(0);
  if( now - _basicTime > RELOAD_TIME_LIMIT ) {
    ServiceI::instance().getObjectCache()->reloadObject(BUDDY_BYAPPLICANT_DATA_I, id);
    _basicTime = now;
  }else if( hasExtra() &&_data->accepters.size() < RELOAD_SIZE_LIMIT ){
    ServiceI::instance().getObjectCache()->reloadObject(BUDDY_BYAPPLICANT_DATA_I, id);
    _basicTime = now;	
  }else{
    //do nothing
  }

  BuddyByApplicantDataPtr result = new BuddyByApplicantData;
  {
    IceUtil::Mutex::Lock lock(_mutex);
    for (AccepterInfoSeq::const_iterator it =_data->accepters.begin(); it != _data->accepters.end(); ++it) {
      if (begin-- > 0) {
        continue;
      }
      if (limit > 0 && (int)result->accepters.size() >= limit) {
        break;
      }
      result->accepters.push_back(*it);
    }
    result->totalCount = this->getTotalCount();
  }
  return result;
}

void BuddyByApplicantDataI::add(const AccepterInfo& accepter) {
  IceUtil::Mutex::Lock lock(_mutex);
  AccepterInfoSeq::iterator it = find_if(_data->accepters.begin(), _data->accepters.end(), 
      bind2nd(equal_accepter(), accepter));
  if( it != _data->accepters.end()){
    _data->accepters.erase(it);
  }
  if( _data->accepters.size() >= ACCEPTER_SIZE_LIMIT){
    _data->accepters.pop_back();
  }
  _data->accepters.insert(_data->accepters.begin(),accepter);
}

void BuddyByApplicantDataI::remove(int accepterId) {

  IceUtil::Mutex::Lock lock(_mutex);
  AccepterInfo data;
  data.accepter = accepterId;
  data.time = 0;
  AccepterInfoSeq::iterator it = find_if(_data->accepters.begin(), _data->accepters.end(),   
      bind2nd(equal_accepter(), data));
  if(it != _data->accepters.end()){
    _data->accepters.erase(it);
  }
}

void BuddyByApplicantDataI::clear() {

  IceUtil::Mutex::Lock lock(_mutex);
  _data->accepters.clear();
}

uint BuddyByApplicantDataI::getTotalCount() {
  if(_extra) {
    return ACCEPTER_SIZE_LIMIT;
  }
  return _data->accepters.size();
}

void BuddyByApplicantDataI::setExtra(bool extra) {
  IceUtil::Mutex::Lock lock(_mutex);
  _extra = extra;
}

bool BuddyByApplicantDataI::hasExtra() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _extra;
}

//-----------------------------------------BuddyByApplicantDataI End-------------------------------------------
//
//-----------------------------------------BuddyByApplicantCacheFactory----------------------------------------

Ice::ObjectPtr BuddyByApplicantCacheFactory::create(::Ice::Int id) {
  MCE_DEBUG("BuddyByApplicantCacheFactory::create(" << id << ")");
  BuddyByApplicantLoaderAdapter::instance().reload(id);
  return new BuddyByApplicantDataI;
}
//-----------------------------------------BuddyByApplicantCacheFactory End------------------------------------
