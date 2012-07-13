#include "ServiceI.h"
#include "BuddyApplyCacheNI.h"
#include <algorithm>
#include <UserCountAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BuddyCoreAdapter.h"
#include "UserCountAdapter.h"
#include "AbstractCheckerAdapterFactory.h"
#include "util/cpp/TimeCost.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using namespace ::xce::buddyapplycache;
using namespace ::MyUtil;
using namespace ::com::xiaonei::xce;
using namespace xce::serverstate;
using namespace ::xce::usercount::adapter;
using namespace com::xiaonei::service::buddy;
using namespace xce::check;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();

  service.registerObjectCache(BUDDYAPPLYCACHE, "B", new SyncServantFactory(
        &BuddyApplyCacheFactoryN::instance()));
  TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));	
  int hardLimit = service.getCommunicator()->getProperties()
    ->getPropertyAsIntWithDefault("BuddyApplyCacheN.HardLimit", 100);

  BuddyApplyCacheManagerNIPtr bacmp = new BuddyApplyCacheManagerNI(hardLimit);

  service.getAdapter()->add(bacmp,
      service.createIdentity("M", ""));
  
  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  //----------------------------HA----------------------------------------------

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyApplyCacheN.Mod");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyApplyCacheN.Interval", 5);

  // �~P~QController注�~F~L
  ServerStateSubscriber::instance().initialize("ControllerBuddyApplyCacheN",
      bacmp, mod, interval);

}

//***************************************************************************

BuddyApplyCacheManagerNI::BuddyApplyCacheManagerNI(int hardLimit)
  : _hardLimit(hardLimit) {
  }


void BuddyApplyCacheManagerNI::checkApplyCache(::Ice::Int id){
  time_t now = time(0);
  BuddyApplyCacheNIPtr ptr = locateBuddyApplyCache(id);
  time_t basicTime = ptr->getBasicTime();
  int _ownerId = ptr->getId();
  if( now - basicTime > 7200  ) {
    MCE_DEBUG("now - basicTime is : " << now - basicTime << ",  so rebuild " << _ownerId );
    int coreSize = -1;
    try {
      coreSize = BuddyCoreAdapter::instance().getAppliedCount(_ownerId);
    } catch (Ice::Exception& e) {
      MCE_WARN("BuddyCoreAdapter.getAppliedCount Ice::Exception " << e);
    } catch (std::exception & e) {
      MCE_WARN("BuddyCoreAdapter.getAppliedCount Ice::Exception " << e.what());
    } catch (...) {
      MCE_WARN("BuddyCoreAdapter.getAppliedCount Ice::Exception ");
    }

    int countSize = -1;
    try {
      countSize = UserCountAdapter::instance().get(_ownerId,::xce::usercount::CGuestRequest);
    } catch (Ice::Exception& e) {
      MCE_WARN("UserCountAdapter.get " << _ownerId << " Ice::Exception " << e);
    } catch (std::exception & e) {
      MCE_WARN("UserCountAdapter.get " << _ownerId << " std::exception " << e.what());
    } catch (...) {
      MCE_WARN("UserCountAdapter.get " << _ownerId << " exception ");
    }

    if(coreSize != -1 || countSize != -1)
    {
      int cacheNSize = ptr->getTotalCount();
      if( ((coreSize != cacheNSize) || (countSize != cacheNSize)) && (cacheNSize != 100))
      {
        MCE_INFO( "Reload id: " << _ownerId << " CoreSize: " << coreSize << ", CountSize: " << countSize << ", CacheNSize:" << cacheNSize );
        BuddyApplyCacheNIPtr data = BuddyApplyCacheNIPtr::dynamicCast(BuddyApplyCacheFactoryN::instance().createFromMaster(_ownerId));
        ServiceI::instance().getObjectCache()->addObject(BUDDYAPPLYCACHE, _ownerId, data );
        BuddyCoreAdapter::instance().reload(_ownerId);
        ::xce::usercount::adapter::UserCountAdapter::instance().set(_ownerId, ::xce::usercount::CGuestRequest, data->getTotalCount());
      }else{
        ptr->setBasicTime(now);
      }
    }

  }else if( ptr->hasExtra() && ptr->getData().size() < 100 ){
    MCE_DEBUG("_extra == true && _buddyApplyCaches.size() < 100,  so rebuild " << _ownerId );
    ServiceI::instance().getObjectCache()->reloadObject(BUDDYAPPLYCACHE, _ownerId);
  }else{
    //do nothing
  }
}

BuddyApplyCacheDataSeqN BuddyApplyCacheManagerNI::getBuddyApplyCache(
    ::Ice::Int id, ::Ice::Int begin, ::Ice::Int limit,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " id: " << id << " begin: " << begin << " limit: " << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  checkApplyCache(id);
  return locateBuddyApplyCache(id)->get(begin, limit);
}

BuddyApplyCacheWithTotalN BuddyApplyCacheManagerNI::getBuddyApplyCacheWithTotal(
    ::Ice::Int id, ::Ice::Int begin, ::Ice::Int limit,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " id: " << id << " begin: " << begin << " limit: " << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

  checkApplyCache(id);

  BuddyApplyCacheWithTotalN res;
  BuddyApplyCacheNIPtr cache = locateBuddyApplyCache(id);
  res.caches = cache->get(begin, limit);
  res.totalCount = cache->getTotalCount();
  return res;
}

void BuddyApplyCacheManagerNI::removeBuddyApplyCache(::Ice::Int id,
    ::Ice::Int applicant, const Ice::Current& current) {
  ostringstream msg;
  msg << " accepter: " << id << ", applicant: " << applicant << " original apply count before remove = " << locateBuddyApplyCache(id)->getTotalCount();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  locateBuddyApplyCache(id)->remove(applicant);
  //------add check------

  string doStr = "removeApplied";
  stringstream sthStr;
  sthStr << applicant;

  OperaDataPtr key = new OperaData(id, doStr, sthStr.str());
  ostringstream value;
  value << sthStr.str();
  string business =  ServiceI::instance().getBusiness();
  string name = ServiceI::instance().getName();
  string replica = business + name.at(name.size()-1);
  AbstractCheckerAdapterFactory::instance().getOceChecker( replica+"CheckerRemoveApply", "@CheckerApply",
      "CheckerRemoveApply", business, replica)->asycNotify(key,value.str());


}

void BuddyApplyCacheManagerNI::clear(::Ice::Int id, const Ice::Current& current) {
  ostringstream msg;
  msg << " id: " << id ;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  locateBuddyApplyCache(id)->clear();
}

void BuddyApplyCacheManagerNI::rebuildBuddyApplyCache(::Ice::Int id,
    const Ice::Current& current) {
  ostringstream msg;
  msg << " id: " << id << " original apply count before add = " << locateBuddyApplyCache(id)->getTotalCount();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  
  //ServiceI::instance().getObjectCache()->reloadObject(BUDDYAPPLYCACHE, id);
  BuddyApplyCacheDataN buddyapply ;
  buddyapply.applicant = atoi(const_cast<Ice::Context&>(current.ctx)["APPLICANT"].c_str());
  Date date(const_cast<Ice::Context&>(current.ctx)["TIME"]);
  buddyapply.time = date.toSeconds();
  buddyapply.why = const_cast<Ice::Context&>(current.ctx)["WHY"];
  buddyapply.statfrom = const_cast<Ice::Context&>(current.ctx)["STAT_FROM"];
  buddyapply.groupname = const_cast<Ice::Context&>(current.ctx)["GROUP_NAME"];
  MCE_TRACE("buddyapply.applicant = " << buddyapply.applicant);
  MCE_TRACE("buddyapply.time      = " << buddyapply.time);
  MCE_TRACE("buddyapply.why       = " << buddyapply.why);
  MCE_TRACE("buddyapply.statfrom  = " << buddyapply.statfrom);
  MCE_TRACE("buddyapply.groupname = " << buddyapply.groupname);
  locateBuddyApplyCache(id)->add(buddyapply);

  //------add check------
  string doStr = "applied";
  stringstream sthStr;
  sthStr << buddyapply.applicant;

  OperaDataPtr key = new OperaData(id, doStr, sthStr.str());
  ostringstream value;
  value << sthStr.str();
  string business =  ServiceI::instance().getBusiness();
  string name = ServiceI::instance().getName();
  string replica = business + name.at(name.size()-1);
  AbstractCheckerAdapterFactory::instance().getOceChecker( replica+"CheckerAddApply", "@CheckerApply",
      "CheckerAddApply", business, replica)->asycNotify(key,value.str());

}

::Ice::Int BuddyApplyCacheManagerNI::getHardLimit(const Ice::Current& current){
  ostringstream msg;
  msg << " ";
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  return _hardLimit;
}

BuddyApplyCacheNIPtr BuddyApplyCacheManagerNI::locateBuddyApplyCache(::Ice::Int id) {
  return ServiceI::instance().locateObject<BuddyApplyCacheNIPtr> (
      BUDDYAPPLYCACHE, id);
}

BuddyApplyCacheNIPtr BuddyApplyCacheManagerNI::findBuddyApplyCache(::Ice::Int id) {
  MCE_DEBUG("findBuddyApplyCache(" << id
      << ")");
  return ServiceI::instance().findObject<BuddyApplyCacheNIPtr> (
      BUDDYAPPLYCACHE, id);
}

void BuddyApplyCacheManagerNI::reload( ::Ice::Int id, const Ice::Current& current) {
  ostringstream msg;
  msg << " id: " << id;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyApplyCacheNIPtr data = BuddyApplyCacheNIPtr::dynamicCast(BuddyApplyCacheFactoryN::instance().createFromMaster(id));
  ServiceI::instance().getObjectCache()->addObject(BUDDYAPPLYCACHE, id, data );

}

void BuddyApplyCacheManagerNI::setData(const ObjectResultPtr& data, const Ice::Current& current) 
{
  ostringstream msg;
  msg << "BuddyApplyCacheManagerNI::setData -> size: "<<data->data.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  TaskManager::instance().execute(new FillTask(data));
}
void BuddyApplyCacheManagerNI::setValid(bool valid, const Ice::Current& current) 
{
  ostringstream msg;
  msg << "valid -> "<<valid;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  IceUtil::RWRecMutex::WLock lock(_validMutex);
  _valid = valid;
  ServerStateSubscriber::instance().setStatus((int)valid);
}

bool BuddyApplyCacheManagerNI::isValid( const Ice::Current& current) 
{
  ostringstream msg;
  msg << "";
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _valid;
}


//***************************************************************************

Ice::ObjectPtr BuddyApplyCacheFactoryN::create(::Ice::Int id) {
  MCE_DEBUG("BuddyApplyCacheFactoryN::create(" << id << ")");
  BuddyApplyCacheNPtr buddyApplyCacheResult = new BuddyApplyCacheN();
  buddyApplyCacheResult->ownerId = id;
  Statement sql;
  sql
    << "SELECT applicant,UNIX_TIMESTAMP(time) AS unix_time,why,stat_from,group_name"
    << " FROM buddyapply_accepter_" << abs(id % 100) << " WHERE accepter = '" << id << "'"
    << " ORDER BY time DESC"
    << " LIMIT 100";
  BuddyApplyCacheResultHandlerI handler(buddyApplyCacheResult);
  ostringstream pattern;
  pattern << "buddyapply_accepter_" << id % 100;
  QueryRunner("buddyapplycache_accepter", CDbRServer,pattern.str()).query(sql, handler);
  BuddyApplyCacheNIPtr obj = new BuddyApplyCacheNI(buddyApplyCacheResult);
  return obj;
}
Ice::ObjectPtr BuddyApplyCacheFactoryN::createFromMaster(::Ice::Int id) {
  MCE_DEBUG("BuddyApplyCacheFactoryN::createFromMaster(" << id << ")");
  BuddyApplyCacheNPtr buddyApplyCacheResult = new BuddyApplyCacheN();
  buddyApplyCacheResult->ownerId = id;
  Statement sql;
  sql
    << "SELECT applicant,UNIX_TIMESTAMP(time) AS unix_time,why,stat_from,group_name"
    << " FROM buddyapply_accepter_" << abs(id % 100) << " WHERE accepter = '" << id << "'"
    << " ORDER BY time DESC"
    << " LIMIT 100";
  BuddyApplyCacheResultHandlerI handler(buddyApplyCacheResult);
  ostringstream pattern;
  pattern << "buddyapply_accepter_" << id % 100;
  QueryRunner("buddycore_accepter", CDbRServer,pattern.str()).query(sql, handler);
  BuddyApplyCacheNIPtr obj = new BuddyApplyCacheNI(buddyApplyCacheResult);	
  return obj;
}
void BuddyApplyCacheFactoryN::SetGuestRequestCountTask::handle() {
  int oldValue = UserCountAdapter::instance().get(_userId,3);
  if (oldValue != _newValue) {
    UserCountAdapter::instance().set(_userId,3,_newValue);
  }
}

//***************************************************************************


void FillTask::handle() 
{
  ObjectResultPtr results = new ObjectResult;
  for(MyUtil::ObjectMap::iterator it = _data->data.begin();it != _data->data.end();++it) 
  { 
    BuddyApplyCacheNPtr temp = BuddyApplyCacheNPtr::dynamicCast(it->second);
    BuddyApplyCacheNIPtr data = new BuddyApplyCacheNI(temp);
    results->data.insert(make_pair(data->getId(),data));
  }
  MyUtil::ServiceI::instance().getObjectCache()->addObjects(BUDDYAPPLYCACHE, results->data); 
}

