#include "BuddyByIdCacheI.h"
#include "ServiceI.h"
#include <BuddyByIdReplicaAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "AbstractCheckerAdapterFactory.h"
#include "Checker/share/OperaData.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::serverstate;
using namespace xce::check;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(BUDDY_BYID_CACHE_DATA, "BBIC", new BuddyByIdCacheFactory);
  service.getAdapter()->add(&BuddyByIdCacheManagerI::instance(),
      service.createIdentity("M", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByIdCache.Mod");
  string endpoint = service.getCommunicator()->getProperties()->getProperty("BuddyByIdCache.Endpoint");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdCache.Interval", 5);
  BuddyByIdCacheManagerI::instance().bootstrap(endpoint, mod);

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyByIdCache",
      &BuddyByIdCacheManagerI::instance(), mod, interval);

  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_CACHE_REMOVE, ThreadPoolConfig(1,100));
  service.registerXceObserver(new ServiceSchemaI);
}

/*****************************************************************************/

BuddyByIdDataI::BuddyByIdDataI(){
}

BuddyByIdDataI::BuddyByIdDataI(BuddyByIdDataPtr data){
  IceUtil::Mutex::Lock lock(_mutex);
  //	_friends.swap(data->friends);
  //_friends = data->friends;
  data->friends.swap(_friends);
}

BuddyByIdDataI::BuddyByIdDataI(MyUtil::IntSeq& data){
  IceUtil::Mutex::Lock lock(_mutex);
  //_friends.swap(data);
  //_friends = data;
  data.swap(_friends);
}

Ice::Int BuddyByIdDataI::getCount() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _friends.size();
}

::MyUtil::IntSeq BuddyByIdDataI::getList(::Ice::Int begin, ::Ice::Int limit) {
  IceUtil::Mutex::Lock lock(_mutex);
  if( limit < 0 ){
    return _friends;
  }
  if( ((unsigned int)begin >= _friends.size()) || (begin < 0)){
    return MyUtil::IntSeq();
  }
  if( (unsigned int)(begin+limit) >= _friends.size()){
    return MyUtil::IntSeq( _friends.begin()+begin, _friends.end());
  }else{
    return MyUtil::IntSeq( _friends.begin()+begin, _friends.begin()+begin+limit );
  }
}

void BuddyByIdDataI::remove(::Ice::Int guestId) {
  IceUtil::Mutex::Lock lock(_mutex);
  MyUtil::IntSeq::iterator it = lower_bound( _friends.begin(), _friends.end(), guestId );
  if( it!=_friends.end() && *it==guestId ){
    _friends.erase( it );
  }
}
/*****************************************************************************/

void BuddyByIdCacheManagerI::bootstrap(const string& endpoint, int mod) {
  _endpoint = endpoint;
  _mod = mod;
  MCE_INFO( "[bootstrap] endpoint=" << _endpoint << " mod=" << _mod );
}

::Ice::Int BuddyByIdCacheManagerI::getFriendCount(
    ::Ice::Int hostId, const Ice::Current& current) {
  ::Ice::Int size = getBuddyByIdDataI(hostId)->getCount();
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << size;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return size;
}

::MyUtil::Int2IntMap BuddyByIdCacheManagerI::getFriendCounts(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current){
  ostringstream msg;
  msg << "id.size=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  Int2IntMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyByIdDataI(*it)->getCount() ) );
  }
  return result;
}

::MyUtil::IntSeq BuddyByIdCacheManagerI::getFriendList(
    ::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
  ostringstream msg;
  ::MyUtil::IntSeq obj = getBuddyByIdDataI(hostId)->getList(0,limit);
  msg << "id=" << hostId
    << " size=" << obj.size() << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return obj;
}

::MyUtil::IntSeq BuddyByIdCacheManagerI::getFriendListN(
    ::Ice::Int hostId, ::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
  ostringstream msg;
  ::MyUtil::IntSeq obj = getBuddyByIdDataI(hostId)->getList(begin,limit);
  msg << "id=" << hostId
    << " size=" << obj.size() << " begin=" << begin << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return obj;
}

::MyUtil::Int2IntSeqMap BuddyByIdCacheManagerI::getFriendLists(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current){
  ostringstream msg;
  msg << "hostIds.size()=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Int2IntSeqMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyByIdDataI(*it)->getList(0,-1) ) );
  }
  return result;
}

void BuddyByIdCacheManagerI::load(::Ice::Int userId, const ::Ice::ObjectPtr& o, const Ice::Current& current){
  ostringstream oss;
  oss << "userId="<<userId;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_DEBUG);
  TaskManager::instance().execute( new LoadTask( userId, o ) );
}

void BuddyByIdCacheManagerI::remove( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  ostringstream oss;
  oss << "hostId=" << hostId << ", guestId=" << guestId;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_INFO);
  TaskManager::instance().execute( new RemoveTask( hostId, guestId ) );
}

/*****************************************************************************/

bool BuddyByIdCacheManagerI::isValid(const Ice::Current& current) {
  ostringstream oss;
  oss << _isValid;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_DEBUG);
  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _isValid;
}

void BuddyByIdCacheManagerI::setValid(bool valid,const Ice::Current& current) {
  ostringstream oss;
  oss << valid;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_INFO);
  IceUtil::RWRecMutex::WLock lock(_validMutex);
  _isValid = valid;
  if( valid ){
    ServerStateSubscriber::instance().setStatus(1);
  } else{
    ServerStateSubscriber::instance().setStatus(0);
  }
}

void BuddyByIdCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& buddyData, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  {
    IceUtil::RWRecMutex::RLock lock(_validMutex);
    if(_isValid){
      //MCE_INFO( "isValid " << _isValid );
      lock.release();
      for ( std::map<long, Ice::ObjectPtr>::const_iterator it = buddyData->data.begin();
          it!= buddyData->data.end(); ++it ){
        string doStr = "setData";
        stringstream sthStr;
        sthStr << BuddyByIdDataPtr::dynamicCast(it->second)->friends.size();
        OperaDataPtr key = new OperaData(it->first, doStr, sthStr.str());

        ostringstream value;
        value << BuddyByIdDataPtr::dynamicCast(it->second)->friends.size();
        string business =  ServiceI::instance().getBusiness();
        string name = ServiceI::instance().getName();
        string replica = business + name.at(name.size()-1);
        AbstractCheckerAdapterFactory::instance().getBuddyChecker( replica+"CheckBuddyData", "@CheckerBuddy",
            "CheckerBuddyData", business, replica)->asycNotify(key,value.str());	
      }
    }
  }
  TaskManager::instance().execute(new FillTask(buddyData));
}

BuddyByIdDataIPtr BuddyByIdCacheManagerI::getBuddyByIdDataI(::Ice::Int userId) {
  return ServiceI::instance().locateObject<BuddyByIdDataIPtr> (
      BUDDY_BYID_CACHE_DATA, userId);
}

/*****************************************************************************/
Ice::ObjectPtr BuddyByIdCacheFactory::create(Ice::Int id) {
  std::map<int, int> orderFriend;
  Statement sql;
  sql     << "SELECT guest"
    << " FROM relation_" << (id % 100)
    << " WHERE host = " << id;
  ostringstream pattern;
  pattern << "relation_" << (id % 100);
  QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
      sql, OrderByIdAscHandler( orderFriend, "guest", "guest" ));
  /*
     ostringstream filter_str;
     bool filted = false;
     map<int,int>::const_iterator it = orderFriend.begin();
     if( it != orderFriend.end() ){
     ostringstream ids;
     ids << it->first;
     for(;it!=orderFriend.end();++it){
     ids << ", " << it->first;
     }
     Statement filter_sql;
     filter_sql     << "SELECT id, status"
     << " FROM user_passport"
     << " WHERE id IN (" << ids.str() << ")";
     QueryRunner("user_passport_status", CDbRServer ).query(
     filter_sql, StatusFilterHandler( orderFriend, filter_str, filted, "id", "status" ));
     }
   */
  MyUtil::IntSeq buddies;
  for (map<int, int>::iterator mapIt = orderFriend.begin(); mapIt
      != orderFriend.end(); ++mapIt ){
    buddies.push_back(mapIt->first);
  }
  /*
     if( filted ){
     MCE_INFO("[BuddyByIdCacheFactory::create] userId=" << id << " size=" << buddies.size() << " + filterd: " << filter_str.str() );
     } else{
     MCE_INFO("[BuddyByIdCacheFactory::create] userId=" << id << " size=" << buddies.size() );
     }*/

  BuddyByIdDataIPtr obj = new BuddyByIdDataI(buddies);
  TaskManager::instance().execute(new ReloadTask(id));
  return obj;
}

/*****************************************************************************/
bool OrderByIdAscHandler::handle(mysqlpp::Row& res) const {
  _data.insert(make_pair((int) res[_keyField.c_str()],
        (int) res[_valueField.c_str()]));
  return true;
}

bool StatusFilterHandler::handle(mysqlpp::Row& res) const {
  int status = (int) res[_statusField.c_str()];
  if( _statusField == "" || 0 == status || 3 == status){
    return true;
  }
  int userId = (int) res[_idField.c_str()];
  map<int, int>::iterator it = _orderFriend.find(userId);
  if( it != _orderFriend.end() ){
    _orderFriend.erase(it);
    _filted = true;
    _filter_str << userId << "(" << status << ") ";
  }
  return true;
}

/************Tasks ***********************************************************/

void FillTask::handle() {
  MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
  for ( std::map<long, Ice::ObjectPtr>::const_iterator it = _buddyData->data.begin();
      it!= _buddyData->data.end(); ++it ){
    try{
      BuddyByIdDataIPtr obj = new BuddyByIdDataI( BuddyByIdDataPtr::dynamicCast( it->second ) );
      res->data.insert( make_pair<long, Ice::ObjectPtr>( it->first, obj ) );

    } catch( Ice::Exception e ){
      MCE_WARN( "FillTask " << it->first << " found Ice::Exception : " << e.what() );
      continue;
    } catch( ... ){
      MCE_WARN( "FillTask " << it->first << " found unknown exception ." );
      continue;
    }
  }

  ServiceI::instance().getObjectCache()->addObjects(BUDDY_BYID_CACHE_DATA, res->data);
  MCE_INFO("FillTask done. Task size / ObjectCache size : " << res->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory(BUDDY_BYID_CACHE_DATA)->size());
}

void LoadTask::handle() {
  if (_obj) {
    try {
      BuddyByIdDataIPtr obj = new BuddyByIdDataI( BuddyByIdDataPtr::dynamicCast(_obj) ) ;
      ServiceI::instance().addObject(obj,BUDDY_BYID_CACHE_DATA,_id);
      MCE_INFO( "[LoadTask] userId=" << _id << " size=" << obj->getCount() );
    } catch( Ice::Exception e ){
      MCE_WARN("[LoadTask] userId=" << _id << " found Ice::Exception : " << e.what() );
    } catch( ... ){
      MCE_WARN("[LoadTask] userId=" << _id << " found unknown exception ." );
    }
  }
}

void ReloadTask::handle() {
  BuddyByIdLoaderAdapter::instance().reload(_id);
}

void RemoveTask::handle() {
  BuddyByIdDataIPtr res = ServiceI::instance().findObject<BuddyByIdDataIPtr> (
      BUDDY_BYID_CACHE_DATA, _hostId);
  if( !res ){
    return;
  }
  res->remove(_guestId);

  string doStr = "remove";
  stringstream sthStr;
  sthStr << _guestId;
  OperaDataPtr key = new OperaData(_hostId, doStr, sthStr.str());
  string business =  ServiceI::instance().getBusiness();
  string name = ServiceI::instance().getName();
  string replica = business + name.at(name.size()-1);
  ostringstream value;
  value<<res->getCount();
  AbstractCheckerAdapterFactory::instance().getBuddyChecker( replica+"CheckRemoveBuddy", "@CheckerBuddy",
      "CheckerRemoveBuddy", business, replica)->asycNotify( key, value.str());

}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "Fill", TASK_LEVEL_BUDDY_BYID_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_BYID_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_BYID_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Remove", TASK_LEVEL_BUDDY_BYID_CACHE_REMOVE, ThreadPoolConfig(1,100));
}
