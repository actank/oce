#include "BuddyByNameCacheI.h"
#include "ServiceI.h"
#include <BuddyByNameReplicaAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "NameCnvtr.h"
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
  service.registerObjectCache(BUDDY_BYNAME_CACHE_DATA_I, "BBNC", new BuddyByNameCacheFactory);
  service.getAdapter()->add(&BuddyByNameCacheManagerI::instance(),
      service.createIdentity("M", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByNameCache.Mod");
  string endpoint = service.getCommunicator()->getProperties()->getProperty("BuddyByNameCache.Endpoint");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNameCache.Interval", 5);
  BuddyByNameCacheManagerI::instance().bootstrap(endpoint, mod);

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyByNameCache",
      &BuddyByNameCacheManagerI::instance(), mod, interval);

  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYNAME_CACHE_FILL, ThreadPoolConfig(1,10));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYNAME_CACHE_LOAD, ThreadPoolConfig(1,10));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYNAME_CACHE_RELOAD, ThreadPoolConfig(1,10));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYNAME_CACHE_REMOVE, ThreadPoolConfig(1,10));
  service.registerXceObserver(new ServiceSchemaI);
}

/*****************************************************************************/

BuddyByNameDataI::BuddyByNameDataI(){
}

BuddyByNameDataI::BuddyByNameDataI( BuddyByNameDataPtr data ){
  std::multimap<std::string,int,KeyCmp> friendmap;
  NameCnvtrPtr cc = new NameCnvtr("utf-8","big5","gb2312");
  ostringstream nameInfo;
  for(NameNoteSeq::const_iterator it=data->names.begin();it!=data->names.end();++it){
    std::string name;
    if( it->name != "" ){
      name = cc->cnvt_indirect(it->name);
    } else{
      name = _maxWord;
    }
    nameInfo << it->id << "(" << it->name << ") ";
    if( strcmp(const_cast<char*>(name.c_str()),const_cast<char*>(_minWord.c_str())) < 0 ){
      name = _maxWord + name;
    }
    friendmap.insert( std::multimap<string,int>::value_type(name,it->id) );
  }
  MCE_DEBUG("names: " << nameInfo.str());
  for(std::multimap<string,int,KeyCmp>::const_iterator itMap = friendmap.begin();itMap!=friendmap.end();++itMap){
    _friends.push_back( itMap->second );
  }
}

Ice::Int BuddyByNameDataI::getCount() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _friends.size();
}

::MyUtil::IntSeq BuddyByNameDataI::getList(::Ice::Int begin,::Ice::Int limit) {
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

void BuddyByNameDataI::remove(::Ice::Int guestId) {
  IceUtil::Mutex::Lock lock(_mutex);
  MyUtil::IntSeq::iterator it = find( _friends.begin(), _friends.end(), guestId );
  if( it!=_friends.end() ){
    _friends.erase( it );
  }
}


/*****************************************************************************/

void BuddyByNameCacheManagerI::bootstrap(const string& endpoint, int mod) {
  _endpoint = endpoint;
  _mod = mod;
  MCE_INFO( "[bootstrap] endpoint=" << _endpoint << " mod=" << _mod );
}

::Ice::Int BuddyByNameCacheManagerI::getFriendCount(
    ::Ice::Int hostId, const Ice::Current& current) {
  ::Ice::Int size = getBuddyByNameDataI(hostId)->getCount();
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << size;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return size;
}

::MyUtil::Int2IntMap BuddyByNameCacheManagerI::getFriendCounts(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current){
  ostringstream msg;
  msg << "id.size=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Int2IntMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyByNameDataI(*it)->getCount() ) );
  }
  return result;
}

::MyUtil::IntSeq BuddyByNameCacheManagerI::getFriendList(
    ::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
  BuddyByNameDataIPtr obj = getBuddyByNameDataI(hostId);
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << obj->getCount() << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  return obj->getList(0,limit);
}

::MyUtil::IntSeq BuddyByNameCacheManagerI::getFriendListN(
    ::Ice::Int hostId, ::Ice::Int begin,::Ice::Int limit, const Ice::Current& current) {
  BuddyByNameDataIPtr obj = getBuddyByNameDataI(hostId);
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << obj->getCount() << " begin=" << begin << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);  

  return obj->getList(begin,limit);
}

::MyUtil::Int2IntSeqMap BuddyByNameCacheManagerI::getFriendLists(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current) {
  ostringstream msg;
  msg << "hostIds.size()=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  Int2IntSeqMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyByNameDataI(*it)->getList(0,-1) ) );
  }
  return result;
}

void BuddyByNameCacheManagerI::load(::Ice::Int userId, const ::Ice::ObjectPtr& o, const Ice::Current& current) {
  ostringstream oss;
  oss << "userId="<<userId;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_DEBUG);
  TaskManager::instance().execute( new LoadTask( userId, o ) );
}

void BuddyByNameCacheManagerI::remove( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  ostringstream oss;
  oss << "hostId=" << hostId << ", guestId=" << guestId;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_INFO);
  TaskManager::instance().execute( new RemoveTask( hostId, guestId ) );
}

/*****************************************************************************/

bool BuddyByNameCacheManagerI::isValid(const Ice::Current& current) {
  ostringstream oss;
  oss << _isValid;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_DEBUG);
  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _isValid;
}

void BuddyByNameCacheManagerI::setValid(bool valid,const Ice::Current& current) {
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

void BuddyByNameCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& buddyData, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new FillTask(buddyData));
}       

BuddyByNameDataIPtr BuddyByNameCacheManagerI::getBuddyByNameDataI(::Ice::Int userId) {
  BuddyByNameDataIPtr res = ServiceI::instance().findObject<BuddyByNameDataIPtr> (
      BUDDY_BYNAME_CACHE_DATA_I, userId);
  if(res){
    return res;
  }
  TaskManager::instance().execute(new ReloadTask(userId));
  return new BuddyByNameDataI;
}

/*****************************************************************************/

Ice::ObjectPtr BuddyByNameCacheFactory::create(Ice::Int id) {
  TaskManager::instance().execute(new ReloadTask(id));
  return new BuddyByNameDataI;
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
      MCE_DEBUG("id " << it->first << " names.size() " << BuddyByNameDataPtr::dynamicCast( it->second )->names.size() << " transfer begin");
      BuddyByNameDataIPtr obj = new BuddyByNameDataI( BuddyByNameDataPtr::dynamicCast( it->second ) );
      res->data.insert( make_pair<long, Ice::ObjectPtr>( it->first, obj ) );
      idsKey << it->first << "/" << obj->getCount() << " ";
    } catch( Ice::Exception e ){
      MCE_WARN( "FillTask " << it->first << " found Ice::Exception : " << e.what() );
      continue;
    } catch( ... ){
      MCE_WARN( "FillTask " << it->first << " found unknown exception ." );
      continue;
    }
  }
  ServiceI::instance().getObjectCache()->addObjects( BUDDY_BYNAME_CACHE_DATA_I, res->data );
  MCE_DEBUG( "data.size: " << _buddyData->data.size() << " idsKey: " << idsKey.str());
  MCE_INFO( "FillTask done. Task size / ObjectCache size : " << res->data.size() << "/" << _buddyData->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory( BUDDY_BYNAME_CACHE_DATA_I)->size() );

  {
    if(BuddyByNameCacheManagerI::instance().isValid()){
      //MCE_INFO( "isValid " << _isValid );
      for ( std::map<long, Ice::ObjectPtr>::const_iterator it = res->data.begin(); it!= res->data.end(); ++it ){
        string doStr = "setData";
        stringstream sthStr;
        sthStr << BuddyByNameDataIPtr::dynamicCast(it->second)->getCount();
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

void LoadTask::handle() {
  if (_obj) {
    try {
      BuddyByNameDataIPtr obj = new BuddyByNameDataI( BuddyByNameDataPtr::dynamicCast( _obj ) );
      if ( obj ) {
        ServiceI::instance().addObject(obj,BUDDY_BYNAME_CACHE_DATA_I,_id);
      }
      MCE_INFO( "[LoadTask] userId=" << _id << " size=" << obj->getCount() );
    } catch( Ice::Exception e ){
      MCE_WARN("[LoadTask] userId=" << _id << " found Ice::Exception : " << e.what() );
    } catch( ... ){
      MCE_WARN("[LoadTask] userId=" << _id << " found unknown exception ." );
    }
  }
}

void ReloadTask::handle() {
  BuddyByNameLoaderAdapter::instance().reload(_id);
}

void RemoveTask::handle() {
  BuddyByNameDataIPtr res = ServiceI::instance().findObject<BuddyByNameDataIPtr> (
      BUDDY_BYNAME_CACHE_DATA_I, _hostId);
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
  TaskManager::instance().config(props, "Fill", TASK_LEVEL_BUDDY_BYNAME_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_BYNAME_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_BYNAME_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Remove", TASK_LEVEL_BUDDY_BYNAME_CACHE_REMOVE, ThreadPoolConfig(1,100));
}

