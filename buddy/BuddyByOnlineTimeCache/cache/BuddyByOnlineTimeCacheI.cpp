#include "BuddyByOnlineTimeCacheI.h"
#include "ServiceI.h"
#include <BuddyByOnlineTimeReplicaAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "AbstractCheckerAdapterFactory.h"
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
  service.registerObjectCache(BUDDY_BYONLINETIME_CACHE_DATA_I, "BBOTC", new BuddyByOnlineTimeCacheFactory);
  service.getAdapter()->add(&BuddyByOnlineTimeCacheManagerI::instance(),
      service.createIdentity("M", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByOnlineTimeCache.Mod");
  string endpoint = service.getCommunicator()->getProperties()->getProperty("BuddyByOnlineTimeCache.Endpoint");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByOnlineTimeCache.Interval", 5);
  BuddyByOnlineTimeCacheManagerI::instance().bootstrap(endpoint, mod);

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyByOnlineTimeCache",
      &BuddyByOnlineTimeCacheManagerI::instance(), mod, interval);

  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_REMOVE, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_NOTIFYONLINE, ThreadPoolConfig(1,1));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_NOTIFYONLINEBATCH, ThreadPoolConfig(1,1));
  service.registerXceObserver(new ServiceSchemaI);
}

/*****************************************************************************/

BuddyByOnlineTimeDataI::BuddyByOnlineTimeDataI(){
}

BuddyByOnlineTimeDataI::BuddyByOnlineTimeDataI( BuddyByOnlineTimeDataPtr data ){
  std::multimap<int, int> orderFriend;
  for (OnlineTimeSeq::const_iterator it = data->onlinetime.begin(); it
      != data->onlinetime.end(); ++it) {
    orderFriend.insert(make_pair<int,int>(it->time,it->userId));
  }
  MyUtil::IntSeq buddies;
  for (multimap<int, int>::reverse_iterator it = orderFriend.rbegin(); it
      != orderFriend.rend(); ++it ){
    buddies.push_back(it->second);
  }
  friends.swap( buddies );
}

Ice::Int BuddyByOnlineTimeDataI::get_count() {
  IceUtil::Mutex::Lock lock(mutex);
  return friends.size();
}

MyUtil::IntSeq BuddyByOnlineTimeDataI::get_most_recent(int begin, int limit) {
  IceUtil::Mutex::Lock lock(mutex);
  if( limit < 0 ){
    return friends;
  }
  if( ((unsigned int)begin >= friends.size()) || (begin < 0)){
    return MyUtil::IntSeq();
  } 
  if( (unsigned int)(begin+limit) >= friends.size() ){
    return MyUtil::IntSeq( friends.begin()+begin, friends.end());
  }else{
    return MyUtil::IntSeq( friends.begin()+begin, friends.begin()+begin+limit );
  }
}

void BuddyByOnlineTimeDataI::notify_somebody_online(int the_online_one){
  IceUtil::Mutex::Lock lock(mutex);
  MyUtil::IntSeq::iterator it = find( friends.begin(), friends.end(),
      the_online_one );
  if ( it != friends.end() ){
    friends.erase(it);
    friends.insert(friends.begin(),the_online_one);
  }
}

void BuddyByOnlineTimeDataI::remove(int userid){
  IceUtil::Mutex::Lock lock(mutex);
  MyUtil::IntSeq::iterator it = find( friends.begin(), friends.end(),
      userid );
  if ( it != friends.end() ){
    friends.erase(it);
  }
}

/*****************************************************************************/

void BuddyByOnlineTimeCacheManagerI::bootstrap(const string& endpoint, int mod) {
  _endpoint = endpoint;
  _mod = mod;
  MCE_INFO( "[bootstrap] endpoint=" << _endpoint << " mod=" << _mod );
}

::Ice::Int BuddyByOnlineTimeCacheManagerI::getFriendCount(
    ::Ice::Int hostId, const Ice::Current& current) {
  ::Ice::Int size = locateBuddyByOnlineTimeDataI(hostId)->get_count();
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << size;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return size;
}

::MyUtil::Int2IntMap BuddyByOnlineTimeCacheManagerI::getFriendCounts(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current){
  ostringstream msg;
  msg << "id.size=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  Int2IntMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyByOnlineTimeDataI(*it)->get_count() ) );
  }
  return result;
}

::MyUtil::IntSeq BuddyByOnlineTimeCacheManagerI::getFriendList(
    ::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
  BuddyByOnlineTimeDataIPtr obj = locateBuddyByOnlineTimeDataI(hostId);
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << obj->get_count() << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return obj->get_most_recent(0,limit);
}

::MyUtil::IntSeq BuddyByOnlineTimeCacheManagerI::getFriendListN(
    ::Ice::Int hostId, ::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
  BuddyByOnlineTimeDataIPtr obj = locateBuddyByOnlineTimeDataI(hostId);
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << obj->get_count() << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return obj->get_most_recent(begin,limit);
}


::MyUtil::Int2IntSeqMap BuddyByOnlineTimeCacheManagerI::getFriendLists(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current){
  ostringstream msg;
  msg << "hostIds.size()=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  Int2IntSeqMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, locateBuddyByOnlineTimeDataI(*it)->get_most_recent(0,-1) ) );
  }
  return result;
}

void BuddyByOnlineTimeCacheManagerI::load(::Ice::Int userId, const ::Ice::ObjectPtr& o, const Ice::Current& current){
  MCE_DEBUG( "[load] userId=" << userId );
  TaskManager::instance().execute( new LoadTask( userId, o ) );
}

void LoadTask::handle() {
  if (_obj) {
    try {
      BuddyByOnlineTimeDataIPtr obj = new BuddyByOnlineTimeDataI( BuddyByOnlineTimeDataPtr::dynamicCast( _obj ) );
      if ( obj ) {
        ServiceI::instance().addObject(obj,BUDDY_BYONLINETIME_CACHE_DATA_I,_id);
      }
      MCE_INFO( "[LoadTask] userId=" << _id << " size=" << obj->get_count() );
    } catch( Ice::Exception e ){
      MCE_WARN("[LoadTask] userId=" << _id << " found Ice::Exception : " << e.what() );
    } catch( ... ){
      MCE_WARN("[LoadTask] userId=" << _id << " found unknown exception ." );
    }
  }
}

void BuddyByOnlineTimeCacheManagerI::remove( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  ostringstream oss;
  oss << "hostId=" << hostId << ", guestId=" << guestId;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_INFO);
  TaskManager::instance().execute( new RemoveTask( hostId, guestId ) );
}

void BuddyByOnlineTimeCacheManagerI::notifySomebodyOnline(
    ::Ice::Int hostId, ::Ice::Int onlineId, const Ice::Current& current) {
  MCE_INFO( "[notifySomebodyOnline] id=" << hostId << " onlineId=" << onlineId );
  TaskManager::instance().execute( new NotifyOnlineTask( hostId, onlineId ) );
}

void BuddyByOnlineTimeCacheManagerI::notifyOnlineBatch(
    const OnlineNoteSeq& notes, const Ice::Current& current) {
  MCE_INFO( "[notifyOnlineBatch] size=" << notes.size() );
  TaskManager::instance().execute( new NotifyOnlineBatchTask( notes ) );
}

/*****************************************************************************/

bool BuddyByOnlineTimeCacheManagerI::isValid(const Ice::Current& current) {
  ostringstream oss;
  oss << _isValid;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_DEBUG);
 
  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _isValid;
}

void BuddyByOnlineTimeCacheManagerI::setValid(bool valid,const Ice::Current& current) {
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

void BuddyByOnlineTimeCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& buddyData, const Ice::Current& current) {
  MCE_INFO("[BuddyByOnlineTimeCacheManagerI::setData data size = " << buddyData->data.size());
  TaskManager::instance().execute(new FillTask(buddyData));
}       

BuddyByOnlineTimeDataIPtr BuddyByOnlineTimeCacheManagerI::getBuddyByOnlineTimeDataI(::Ice::Int userId) {
  BuddyByOnlineTimeDataIPtr res = ServiceI::instance().findObject<BuddyByOnlineTimeDataIPtr> (
      BUDDY_BYONLINETIME_CACHE_DATA_I, userId);
  if(res){
    return res;
  }
  return new BuddyByOnlineTimeDataI;
}

BuddyByOnlineTimeDataIPtr BuddyByOnlineTimeCacheManagerI::locateBuddyByOnlineTimeDataI(::Ice::Int userId) {
  BuddyByOnlineTimeDataIPtr res = ServiceI::instance().findObject<BuddyByOnlineTimeDataIPtr> (
      BUDDY_BYONLINETIME_CACHE_DATA_I, userId);
  if(res){
    return res;
  }
  TaskManager::instance().execute(new ReloadTask(userId));
  BuddyByOnlineTimeDataIPtr result = new BuddyByOnlineTimeDataI;
  return result;
}

/*****************************************************************************/

Ice::ObjectPtr BuddyByOnlineTimeCacheFactory::create(Ice::Int id) {
  TaskManager::instance().execute(new ReloadTask(id));
  BuddyByOnlineTimeDataIPtr result = new BuddyByOnlineTimeDataI;
  return result;
}

/************* Tasks *********************************************************/

void FillTask::handle() {
  MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
  MCE_INFO("[FillTask::handle] process data size is " << _buddyData->data.size());
  ostringstream idsKey;
  for ( std::map<long, Ice::ObjectPtr>::const_iterator it = _buddyData->data.begin();
      it!= _buddyData->data.end(); ++it ){
    try{
      MCE_INFO("[FillTask::handle] id = " << it->first << " add to res");
      BuddyByOnlineTimeDataIPtr obj = new BuddyByOnlineTimeDataI( BuddyByOnlineTimeDataPtr::dynamicCast( it->second ) );
      res->data.insert( make_pair<long, Ice::ObjectPtr>( it->first, obj ) );
      MCE_DEBUG("FillTask " << it->first << " size: "  << obj->friends.size());
      idsKey << it->first << "/" << obj->friends.size() << " ";
    } catch( Ice::Exception e ){
      MCE_WARN( "FillTask " << it->first << " found Ice::Exception : " << e.what() );
      continue;
    } catch( ... ){
      MCE_WARN( "FillTask " << it->first << " found unknown exception ." );
      continue;
    }
  }
  ServiceI::instance().getObjectCache()->addObjects( BUDDY_BYONLINETIME_CACHE_DATA_I, res->data );
  MCE_DEBUG( "data.size: " << _buddyData->data.size() << " idsKey: " << idsKey.str());
  MCE_INFO( "FillTask done. Task size / ObjectCache size : " << res->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory( BUDDY_BYONLINETIME_CACHE_DATA_I)->size() );

  {
    if(BuddyByOnlineTimeCacheManagerI::instance().isValid()){
      //MCE_INFO( "isValid " << _isValid );
      for ( std::map<long, Ice::ObjectPtr>::const_iterator it = res->data.begin();
          it!= res->data.end(); ++it ){
        string doStr = "setData";
        stringstream sthStr;
        sthStr << BuddyByOnlineTimeDataIPtr::dynamicCast(it->second)->get_count();
        OperaDataPtr key = new OperaData(it->first, doStr, sthStr.str());

        ostringstream value;
        value << sthStr.str();
        string business =  ServiceI::instance().getBusiness();
        string name = ServiceI::instance().getName();
        string replica = business + name.at(name.size()-1);
        AbstractCheckerAdapterFactory::instance().getBuddyChecker( replica+"CheckBuddyData", "@CheckerBuddy",
            "CheckerBuddyData", business, replica)->asycNotify(key,value.str());
        MCE_INFO("[FillTask::handle] id = " << it->first << " doStr = " << doStr << " sthStr = " << sthStr.str() << " business = " << business 
            << " name = " << name << " replica = " << replica); 
      }
    }
  }
}

void ReloadTask::handle() {
  BuddyByOnlineTimeLoaderAdapter::instance().reload(_id);
}

void RemoveTask::handle() {
  BuddyByOnlineTimeDataIPtr res = ServiceI::instance().findObject<BuddyByOnlineTimeDataIPtr> (
      BUDDY_BYONLINETIME_CACHE_DATA_I, _hostId);
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
  value<<res->get_count();
  AbstractCheckerAdapterFactory::instance().getBuddyChecker( replica+"CheckRemoveBuddy", "@CheckerBuddy",
      "CheckerRemoveBuddy", business, replica)->asycNotify( key, value.str());
}

void NotifyOnlineTask::handle() {
  BuddyByOnlineTimeDataIPtr res = ServiceI::instance().findObject<BuddyByOnlineTimeDataIPtr>(
      BUDDY_BYONLINETIME_CACHE_DATA_I, _hostId );
  if( res ){
    res->notify_somebody_online( _onlineId );
    MCE_DEBUG( "[NotifyOnlineBatchTask] id=" << _hostId << " onlineId=" << _onlineId );
  }
}

void NotifyOnlineBatchTask::handle() {
  for( OnlineNoteSeq::const_iterator it = _notes.begin();it != _notes.end(); ++it ){
    BuddyByOnlineTimeDataIPtr res = ServiceI::instance().findObject<BuddyByOnlineTimeDataIPtr>(
        BUDDY_BYONLINETIME_CACHE_DATA_I, it->userId );
    if( res ){
      res->notify_somebody_online( it->onlineId );
      MCE_DEBUG( "[NotifyOnlineBatchTask] id=" << it->userId << " onlineId=" << it->onlineId );
    }
  }
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "Fill", TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Remove", TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_REMOVE, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "NotifyOnline", TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_NOTIFYONLINE, ThreadPoolConfig(1,1));
  TaskManager::instance().config(props, "NotifyOnlineBatch", TASK_LEVEL_BUDDY_BYONLINETIME_CACHE_NOTIFYONLINEBATCH, ThreadPoolConfig(1,100));
}

