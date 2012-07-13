#include "BuddyByAddTimeCacheI.h"
#include "ServiceI.h"
#include <BuddyByAddTimeReplicaAdapter.h>
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
  service.registerObjectCache(BUDDY_BYADDTIME_CACHE_DATA, "BBATC", new BuddyByAddTimeCacheFactory);
  service.getAdapter()->add(&BuddyByAddTimeCacheManagerI::instance(),
      service.createIdentity("M", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByAddTimeCache.Mod");
  string endpoint = service.getCommunicator()->getProperties()->getProperty("BuddyByAddTimeCache.Endpoint");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByAddTimeCache.Interval", 5);
  BuddyByAddTimeCacheManagerI::instance().bootstrap(endpoint, mod);

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyByAddTimeCache",
      &BuddyByAddTimeCacheManagerI::instance(), mod, interval);

  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYADDTIME_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYADDTIME_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYADDTIME_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYADDTIME_CACHE_REMOVE, ThreadPoolConfig(1,100));
  service.registerXceObserver(new ServiceSchemaI);
}

/*****************************************************************************/
BuddyByAddTimeDataI::BuddyByAddTimeDataI(){
}

BuddyByAddTimeDataI::BuddyByAddTimeDataI(const IntSeq& friends,const IntSeq& times){
  if( friends.size() == times.size() ){
    IntSeq(friends).swap(_friends);
    IntSeq(times).swap(_times);
  }
}

BuddyByAddTimeDataI::BuddyByAddTimeDataI(BuddyByAddTimeDataNPtr data){
  if( data->friends.size() == data->times.size() ){
    data->friends.swap(_friends);
    data->times.swap(_times);
  }
}

Ice::Int BuddyByAddTimeDataI::getCount() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _friends.size();
}

::MyUtil::IntSeq BuddyByAddTimeDataI::getList(::Ice::Int begin,::Ice::Int limit) {
  IceUtil::Mutex::Lock lock(_mutex);
  if( limit < 0 ){
    return _friends;
  }
  if( (begin >= (_friends.end()-_friends.begin())) || (begin < 0)){
    return MyUtil::IntSeq();
  }
  if(begin+limit >= (_friends.end()-_friends.begin())){
    return MyUtil::IntSeq( _friends.begin()+begin, _friends.end());
  }else{
    return MyUtil::IntSeq( _friends.begin()+begin, _friends.begin()+begin+limit );
  }
}

BuddyByAddTimeNodeSeq BuddyByAddTimeDataI::getListWithinTime(::Ice::Int beginTime,::Ice::Int endTime){
  BuddyByAddTimeNodeSeq res; 
  if( beginTime > endTime ){
    return res;
  }
  IceUtil::Mutex::Lock lock(_mutex);
  MyUtil::IntSeq::iterator itF = _friends.begin();
  MyUtil::IntSeq::iterator itT = _times.begin();
  while( itT!=_times.end() && itF!=_friends.end() ){
    if( *itT>endTime ){
      ++itT;
      ++itF;
      continue;
    }
    if( *itT<beginTime ){
      break;
    }
    BuddyByAddTimeNode node;
    node.time = *itT;
    node.userId = *itF;
    res.push_back( node );
    ++itT;
    ++itF;
  }
  return res;
}

BuddyByAddTimeNodeSeq BuddyByAddTimeDataI::getListAndTime(::Ice::Int limit){
  BuddyByAddTimeNodeSeq res; 
  MyUtil::IntSeq friends;
  MyUtil::IntSeq times;
  {
    IceUtil::Mutex::Lock lock(_mutex);
    if( limit == 0){
      return res;
    } else if( (limit<0) || (limit>=_friends.end()-_friends.begin()) ){
      friends = _friends;
      times = _times;
    } else{
      friends = MyUtil::IntSeq( _friends.begin(), _friends.begin()+limit);
      times = MyUtil::IntSeq( _times.begin(), _times.begin()+limit);
    }
  }

  MyUtil::IntSeq::iterator itF = friends.begin();
  MyUtil::IntSeq::iterator itT = times.begin();
  while( itT!=times.end() && itF!=friends.end() ){
    BuddyByAddTimeNode node;
    node.time = *itT;
    node.userId = *itF;
    res.push_back( node );
    ++itT;
    ++itF;
  }
  return res;
}

void BuddyByAddTimeDataI::remove(::Ice::Int guestId) {
  IceUtil::Mutex::Lock lock(_mutex);
  MyUtil::IntSeq::iterator itF = _friends.begin();
  MyUtil::IntSeq::iterator itT = _times.begin();
  while( itF!=_friends.end() && itT != _times.end() ){
    if( *itF == guestId ){
      _friends.erase( itF );
      _times.erase( itT );
      break;
    }
    ++itF;
    ++itT;
  }
}

/*****************************************************************************/
void BuddyByAddTimeCacheManagerI::bootstrap(const string& endpoint, int mod) {
  _endpoint = endpoint;
  _mod = mod;
  MCE_INFO( "[bootstrap] endpoint=" << _endpoint << " mod=" << _mod );
}

::Ice::Int BuddyByAddTimeCacheManagerI::getFriendCount(
    ::Ice::Int hostId, const Ice::Current& current) {
  ::Ice::Int size = getBuddyByAddTimeData(hostId)->getCount();
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << size;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return size;
}

::MyUtil::Int2IntMap BuddyByAddTimeCacheManagerI::getFriendCounts(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current){
  ostringstream msg;
  msg << "id.size=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  
  Int2IntMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyByAddTimeData(*it)->getCount() ) );
  }
  return result;
}

::MyUtil::IntSeq BuddyByAddTimeCacheManagerI::getFriendList(
    ::Ice::Int hostId, ::Ice::Int limit, const Ice::Current& current) {
  BuddyByAddTimeDataIPtr obj = getBuddyByAddTimeData(hostId);
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << obj->getCount() << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return obj->getList(0,limit);
}

::MyUtil::IntSeq BuddyByAddTimeCacheManagerI::getFriendListN(
    ::Ice::Int hostId, ::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
  BuddyByAddTimeDataIPtr obj = getBuddyByAddTimeData(hostId);
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << obj->getCount() << " begin=" << begin << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  return obj->getList(begin,limit);
}

::MyUtil::Int2IntSeqMap BuddyByAddTimeCacheManagerI::getFriendLists(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current) {
  ostringstream msg;
  msg << "hostIds.size()=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  Int2IntSeqMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyByAddTimeData(*it)->getList(0,-1) ) );
  }
  return result;
}

BuddyByAddTimeNodeSeq BuddyByAddTimeCacheManagerI::getFriendListWithinTime(::Ice::Int userId,
    ::Ice::Long beginTime, ::Ice::Long endTime, const Ice::Current& current) {
  BuddyByAddTimeDataIPtr obj = getBuddyByAddTimeData(userId);
  ostringstream msg;
  msg << "id=" << userId
    << " size=" << obj->getCount() << " beginTime=" << beginTime << " endTime=" << endTime;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return obj->getListWithinTime((int)(beginTime/1000),(int)(endTime/1000));
}

BuddyByAddTimeNodeSeq BuddyByAddTimeCacheManagerI::getFriendListAndTime(::Ice::Int userId,
    ::Ice::Int limit, const Ice::Current& current) {
  BuddyByAddTimeDataIPtr obj = getBuddyByAddTimeData(userId);
  ostringstream msg;
  msg << "id=" << userId
    << " size=" << obj->getCount() << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  return obj->getListAndTime(limit);
}

void BuddyByAddTimeCacheManagerI::load(::Ice::Int userId, const ::Ice::ObjectPtr& o, const Ice::Current& current) {
  ostringstream oss;
  oss << "userId="<<userId;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_DEBUG);
  string times = "";
  map<string, string>::const_iterator itTimes = current.ctx.find("TIMES");
  if (itTimes != current.ctx.end()) {
    times = (*itTimes).second;
  }
  TaskManager::instance().execute( new LoadTask( userId, o, times ) );
}

void BuddyByAddTimeCacheManagerI::remove( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  ostringstream oss;
  oss << "hostId=" << hostId << ", guestId=" << guestId;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_INFO);
  TaskManager::instance().execute( new RemoveTask( hostId, guestId ) );
}

/*****************************************************************************/

bool BuddyByAddTimeCacheManagerI::isValid(const Ice::Current& current) {
  ostringstream oss;
  oss << _isValid;
  InvokeClient ic = InvokeClient::create(current,oss.str(),InvokeClient::INVOKE_LEVEL_DEBUG);
  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _isValid;
}

void BuddyByAddTimeCacheManagerI::setValid(bool valid,const Ice::Current& current) {
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

void BuddyByAddTimeCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& buddyData, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute( new FillTask(buddyData) );
}       

BuddyByAddTimeDataIPtr BuddyByAddTimeCacheManagerI::getBuddyByAddTimeData(::Ice::Int userId) {
  BuddyByAddTimeDataIPtr res = ServiceI::instance().findObject<BuddyByAddTimeDataIPtr> (
      BUDDY_BYADDTIME_CACHE_DATA, userId);
  if(res){
    return res;
  }
  TaskManager::instance().execute(new ReloadTask(userId));
  BuddyByAddTimeDataIPtr result = new BuddyByAddTimeDataI;
  return result;
}

/*****************************************************************************/

Ice::ObjectPtr BuddyByAddTimeCacheFactory::create(Ice::Int id) {
  TaskManager::instance().execute(new ReloadTask(id));
  BuddyByAddTimeDataIPtr result = new BuddyByAddTimeDataI;
  return result;
}

/************Tasks ***********************************************************/

void FillTask::handle() {
  MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
  MCE_INFO("[FillTask::handle] process data size is " << _buddyData->data.size());
  ostringstream idsKey;
  for ( std::map<long, Ice::ObjectPtr>::const_iterator it = _buddyData->data.begin();
      it!= _buddyData->data.end(); ++it ){
    try{
      MCE_INFO("[FillTask::handle] id = " << it->first << " add to res");
      BuddyByAddTimeDataIPtr obj = new BuddyByAddTimeDataI( BuddyByAddTimeDataNPtr::dynamicCast( it->second ) );
      MCE_DEBUG("id " << it->first << " getCount " << obj->getCount() << " transfer success");
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

  ServiceI::instance().getObjectCache()->addObjects(BUDDY_BYADDTIME_CACHE_DATA, res->data);
  MCE_DEBUG( "data.size: " << _buddyData->data.size() << " idsKey: " << idsKey.str());
  MCE_INFO("FillTask done. Task size / ObjectCache size : " << _buddyData->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory(BUDDY_BYADDTIME_CACHE_DATA)->size());

  {
    if(BuddyByAddTimeCacheManagerI::instance().isValid()){
      //MCE_INFO( "isValid " << _isValid );
      for ( std::map<long, Ice::ObjectPtr>::const_iterator it = res->data.begin();
          it!= res->data.end(); ++it ){
        string doStr = "setData";
        stringstream sthStr;
        sthStr << BuddyByAddTimeDataIPtr::dynamicCast(it->second)->getCount();
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
      /////////////////////////////////
      MyUtil::IntSeq times;
      string delim = " ";
      size_t last = 0;
      size_t index=_times.find_first_of(delim,last);
      while (index!=std::string::npos)
      {
        std::string tt = _times.substr(last,index-last);
        cout << tt << endl;
        times.push_back(boost::lexical_cast<int>(tt));
        last=index+1;
        index=_times.find_first_of(delim,last);
      }
      /////////////////////////////////
      BuddyByAddTimeDataIPtr obj = new BuddyByAddTimeDataI( BuddyByAddTimeDataPtr::dynamicCast(_obj)->friends,times );

      if(obj){
        ServiceI::instance().addObject(obj,BUDDY_BYADDTIME_CACHE_DATA,_id);
      }
      MCE_INFO("LoadTask done. id=" << _id << " size=" << obj->getCount()
          << " objectcache.size=" << ServiceI::instance().getObjectCache()->locateCategory(BUDDY_BYADDTIME_CACHE_DATA)->size() );
    } catch( Ice::Exception e ){
      MCE_WARN("[LoadNTask] userId=" << _id << " found Ice::Exception : " << e.what() );
    } catch( ... ){
      MCE_WARN("[LoadNTask] userId=" << _id << " found unknown exception ." );
    }
  }
}

void ReloadTask::handle() {
  BuddyByAddTimeLoaderAdapter::instance().reload(_id);
}

void RemoveTask::handle() {
  BuddyByAddTimeDataIPtr res = ServiceI::instance().findObject<BuddyByAddTimeDataIPtr> (
      BUDDY_BYADDTIME_CACHE_DATA, _hostId);
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
  TaskManager::instance().config(props, "Fill", TASK_LEVEL_BUDDY_BYADDTIME_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_BYADDTIME_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_BYADDTIME_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Remove", TASK_LEVEL_BUDDY_BYADDTIME_CACHE_REMOVE, ThreadPoolConfig(1,100));
}

