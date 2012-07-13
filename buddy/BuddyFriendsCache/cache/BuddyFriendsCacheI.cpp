#include <algorithm>
#include <set>
#include "BuddyFriendsCacheI.h"
#include "ServiceI.h"
#include <BuddyFriendsAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include "buddy/BuddyFriendsCache/share/FriendsCompute.h"
#include "buddy/BuddyFriendsCache/share/BuddyFriendsShare.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::serverstate;
using namespace xce::buddyfriends;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(BUDDY_FRIENDS_CACHE_DATA, "BFC", new BuddyFriendsCacheFactory);
  service.getAdapter()->add(&BuddyFriendsCacheManagerI::instance(),
      service.createIdentity("M", ""));

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyFriendsCache.Mod");
  string endpoint = service.getCommunicator()->getProperties()->getProperty("BuddyFriendsCache.Endpoint");
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyFriendsCache.Interval", 5);
  BuddyFriendsCacheManagerI::instance().bootstrap(endpoint, mod);

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyFriendsCache",
      &BuddyFriendsCacheManagerI::instance(), mod, interval);

  TaskManager::instance().config(TASK_LEVEL_BUDDY_FRIENDS_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_FRIENDS_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_FRIENDS_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_FRIENDS_CACHE_REMOVE, ThreadPoolConfig(1,100));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_FRIENDS_CACHE_ADD, ThreadPoolConfig(1,100));
  service.registerXceObserver(new ServiceSchemaI);
}

/*****************************************************************************/

BuddyFriendsDataI::BuddyFriendsDataI(){
}

BuddyFriendsDataI::BuddyFriendsDataI(BuddyFriendsDataPtr data){
  IceUtil::Mutex::Lock lock(_mutex);
  //	_friends.swap(data->friends);
  //_friends = data->friends;
  data->friends.swap(_friends);
}

BuddyFriendsDataI::BuddyFriendsDataI(MyUtil::IntSeq& data){
  IceUtil::Mutex::Lock lock(_mutex);
  //_friends.swap(data);
  //_friends = data;
  data.swap(_friends);
}

Ice::Int BuddyFriendsDataI::getCount() {
  IceUtil::Mutex::Lock lock(_mutex);
  return _friends.size();
}

::MyUtil::IntSeq BuddyFriendsDataI::getList(::Ice::Int begin, ::Ice::Int limit) {
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

void BuddyFriendsDataI::remove(::Ice::Int guestId) {
  IceUtil::Mutex::Lock lock(_mutex);
  MyUtil::IntSeq::iterator it = lower_bound( _friends.begin(), _friends.end(), guestId );
  if( it!=_friends.end() && *it==guestId ){
    _friends.erase( it );
  }
}

void BuddyFriendsDataI::add(::Ice::Int guestId) {
  IceUtil::Mutex::Lock lock(_mutex);

  MyUtil::IntSeq::iterator it = lower_bound( _friends.begin(), _friends.end(), guestId );
  if( it==_friends.end() || *it!=guestId ){
    _friends.insert(it,guestId );
  }
}
/*****************************************************************************/

void BuddyFriendsCacheManagerI::bootstrap(const string& endpoint, int mod) {
  _endpoint = endpoint;
  _mod = mod;
  MCE_INFO( "[bootstrap] endpoint=" << _endpoint << " mod=" << _mod );
}

::Ice::Int BuddyFriendsCacheManagerI::getFriendCount(
    ::Ice::Int hostId, const Ice::Current& current) {
  ::Ice::Int size = getBuddyFriendsDataI(hostId)->getCount();
  ostringstream msg;
  msg << "id=" << hostId
    << " size=" << size;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return size;
}

::MyUtil::Int2IntMap BuddyFriendsCacheManagerI::getFriendCounts(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current){
  ostringstream msg;
  msg << "ids.size=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  Int2IntMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyFriendsDataI(*it)->getCount() ) );
  }
  return result;
}

::MyUtil::IntSeq BuddyFriendsCacheManagerI::getFriendList(
    ::Ice::Int hostId, ::Ice::Int begin, ::Ice::Int limit, const Ice::Current& current) {
  ostringstream msg;
  ::MyUtil::IntSeq obj = getBuddyFriendsDataI(hostId)->getList(begin,limit);
  msg << "id=" << hostId
    << " size=" << obj.size() << " begin=" << begin << " limit=" << limit;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  return obj;
}

::MyUtil::Int2IntSeqMap BuddyFriendsCacheManagerI::getFriendLists(
    const MyUtil::IntSeq& hostIds, const Ice::Current& current){
  ostringstream msg;
  msg << "hostIds.size()=" << hostIds.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  
  Int2IntSeqMap result;
  for(MyUtil::IntSeq::const_iterator it = hostIds.begin();it!=hostIds.end();++it) {
    result.insert( make_pair( *it, getBuddyFriendsDataI(*it)->getList(0,-1) ) );
  }
  return result;
}

void BuddyFriendsCacheManagerI::load(::Ice::Int userId, const ::Ice::ObjectPtr& o, const Ice::Current& current){
  ostringstream msg;
  msg << "userId="<<userId;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  TaskManager::instance().execute( new LoadTask( userId, o ) );
}


void BuddyFriendsCacheManagerI::add( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  ostringstream msg;
  msg << "hostId=" << hostId << ", guestId=" << guestId;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  TaskManager::instance().execute( new AddTask( hostId, guestId ) );
}
void BuddyFriendsCacheManagerI::remove( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  ostringstream msg;
  msg << "hostId=" << hostId << ", guestId=" << guestId;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  TaskManager::instance().execute( new RemoveTask( hostId, guestId ) );
}

bool BuddyFriendsCacheManagerI::isFriendsFriend( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  ostringstream msg;
  msg << "hostId=" << hostId << ", guestId=" << guestId; 
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  
  ::MyUtil::IntSeq hostFriends = getBuddyFriendsDataI(hostId)->getList(0,-1);
  ::MyUtil::IntSeq guestFriends = getBuddyFriendsDataI(guestId)->getList(0,-1);
  return FriendsCompute::instance().hasCommonFriend(hostFriends,guestFriends);
}

::MyUtil::IntSeq BuddyFriendsCacheManagerI::getCommonFriends( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  ostringstream msg;
  msg << "hostId=" << hostId << ", guestId=" << guestId;
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  
  if(hostId == guestId){
    return ::MyUtil::IntSeq();
  } 
  ::MyUtil::IntSeq hostFriendsSeq = getBuddyFriendsDataI(hostId)->getList(0,-1);
  ::MyUtil::IntSeq guestFriendsSeq = getBuddyFriendsDataI(guestId)->getList(0,-1);
  MCE_INFO("getCommonFriends hostFriendsSeq.size=" << hostId << " " << hostFriendsSeq.size());
  MCE_INFO("getCommonFriends guestFriendsSeq.size=" << guestId << " " << guestFriendsSeq.size());

//  //以下代码有内存泄漏的嫌疑，具体问题在哪未查明
//  MyUtil::IntSeq result = FriendsCompute::instance().getCommonFriend(hostFriendsSeq,guestFriendsSeq);
//  MCE_INFO("getCommonFriends commonFriend " << hostId << " " << guestId << " " << result.size());
//  return result;

//  //以下代码可能会越界访问
//  set<int> hostFriends(hostFriendsSeq.begin(),hostFriendsSeq.end());
//  set<int> guestFriends(guestFriendsSeq.begin(),guestFriendsSeq.end());
//  ::MyUtil::IntSeq commonFriend(2000,0);
//  ::MyUtil::IntSeq::iterator commonFriendEnd = set_intersection(hostFriends.begin(),hostFriends.end(),guestFriends.begin(),guestFriends.end(),commonFriend.begin());
//  MCE_INFO("getCommonFriends commonFriend " << hostId << " " << guestId << " " << commonFriendEnd - commonFriend.begin());
//  return ::MyUtil::IntSeq(commonFriend.begin(), commonFriendEnd);

    //以上两处问题代码，修改为以下代码
    set<int> hostFriends(hostFriendsSeq.begin(),hostFriendsSeq.end());
    set<int> guestFriends(guestFriendsSeq.begin(),guestFriendsSeq.end());
    unsigned hSize = hostFriends.size();
    unsigned gSize = guestFriends.size();
    unsigned size = hSize < gSize ? hSize : gSize;
    ::MyUtil::IntSeq commonFriend(size,0);
    ::MyUtil::IntSeq::iterator commonFriendEnd = set_intersection(hostFriends.begin(),hostFriends.end(),guestFriends.begin(),guestFriends.end(),commonFriend.begin());
    MCE_INFO("getCommonFriends commonFriend " << hostId << " " << guestId << " " << commonFriendEnd - commonFriend.begin());
    return ::MyUtil::IntSeq(commonFriend.begin(), commonFriendEnd);
}

::MyUtil::Str2IntSeqMap BuddyFriendsCacheManagerI::getCommonFriendsBatch(const ::MyUtil::Int2IntMap& idpairs, const Ice::Current& current ){
  ostringstream msg;
  msg << "idpairs.size=" << idpairs.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  ::MyUtil::Str2IntSeqMap result;
  for(::MyUtil::Int2IntMap::const_iterator it = idpairs.begin(); it!= idpairs.end(); ++it){
    if(it->first == it->second)
      continue;
    MyUtil::IntSeq commons;
    commons = getCommonFriends(it->first, it->second);
    ostringstream ss;
    ss<<it->first<<it->second;
    result.insert(make_pair(ss.str(), commons));
  }
  return result;
}

/***
 * implements abstract function isFriendsFriendBatch of BuddyFriendsCacheManager in BuddyFriends.ice
 ***/
MyUtil::Int2IntMap BuddyFriendsCacheManagerI::isFriendsFriendBatch(int hostId, const MyUtil::IntSeq& potentialFriends, const Ice::Current& current) {
  ostringstream msg;
  msg << "hostId" << hostId 
    << ", potentialFriends size =" << potentialFriends.size();
  InvokeClient ic = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);

  MyUtil::IntSeq result;
  MyUtil::Int2IntMap finalResult;
  MyUtil::IntSeq friends = getBuddyFriendsDataI(hostId)->getList(0, -1);
  MyUtil::IntSeq temp_potentialFriends;
  //delete direct friends from potentialFriends
  MyUtil::IntSeq temp_friends;
  for(MyUtil::IntSeq::const_iterator it = potentialFriends.begin(); it != potentialFriends.end(); ++it) {
    if(!binary_search(friends.begin(), friends.end(), *it)) {
      temp_potentialFriends.push_back(*it);
    } else {
      finalResult.insert(make_pair(*it, 0));
    }
  }
  //sort temp_potentialFriends and delete duplicated items
  sort(temp_potentialFriends.begin(), temp_potentialFriends.end());
  MyUtil::IntSeq::iterator it = unique(temp_potentialFriends.begin(), temp_potentialFriends.end());
  //unique function doesn't change the size of the parameters, it just move the duplicated items to the back of the vector
  temp_potentialFriends.resize(it - temp_potentialFriends.begin());

  for(MyUtil::IntSeq::const_iterator it = friends.begin(); it != friends.end(); ++it) {
    MyUtil::IntSeq temp_friends = getBuddyFriendsDataI(*it)->getList(0, -1);
    MyUtil::IntSeq temp_result = getCommonItems(temp_friends, temp_potentialFriends);
    result.insert(result.end(), temp_result.begin(), temp_result.end()); 
  }
  for(MyUtil::IntSeq::const_iterator it = result.begin(); it != result.end(); ++it) {
    finalResult.insert(make_pair(*it, 1));
  }
  for(MyUtil::IntSeq::const_iterator it = temp_potentialFriends.begin(); it != temp_potentialFriends.end(); ++it) {
    finalResult.insert(make_pair(*it, 0));
  }
  return finalResult;
}

/***
 * getCommonItems of two sorted seqence, the common items will be deleted from the second seqence, here is friends2
 ***/
MyUtil::IntSeq BuddyFriendsCacheManagerI::getCommonItems(const MyUtil::IntSeq& friends1, MyUtil::IntSeq& friends2) {
  MCE_INFO("[BuddyFriendssCacheManagerI::getCommonItems] friends1.size = " << friends1.size() << ", friends2.size = " << friends2.size());
  MyUtil::IntSeq result;
  MyUtil::IntSeq::const_iterator start1 = friends1.begin();
  MyUtil::IntSeq::const_iterator end1 = friends1.end();
  MyUtil::IntSeq::iterator start2 = friends2.begin();
  MyUtil::IntSeq::iterator end2 = friends2.end();
  while(start1 != end1 && start2 != end2) {
    if(*start1 > *start2) {
      //find an item in friends2 which not smaller than *start1
      start2 = lower_bound(start2 + 1, end2, *start1);
    } else if(*start2 > *start1) {
      //find an item in friends1 which not smaller than *start2
      start1 = lower_bound(start1 + 1, end1, *start2);
    } else {
      //push common item to result and record the position in sequence pos
      result.push_back(*start2);
      ++start1;
      ++start2;
    }
  }
  if(result.size() > 0) {
    MyUtil::IntSeq temp_friends2;
    //delete common items from friends2
    for(MyUtil::IntSeq::iterator it = friends2.begin(); it != friends2.end(); ++it) {
      if(!binary_search(result.begin(), result.end(), *it)) {
        temp_friends2.push_back(*it);
      }
    }
    swap(friends2, temp_friends2);
  }
  return result;
}

/*****************************************************************************/

bool BuddyFriendsCacheManagerI::isValid(const Ice::Current& current) {
  ostringstream oss;
  oss <<  _isValid ;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);
  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _isValid;
}

void BuddyFriendsCacheManagerI::setValid(bool valid,const Ice::Current& current) {
  ostringstream oss;
  oss << valid;
  InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);
  IceUtil::RWRecMutex::WLock lock(_validMutex);
  _isValid = valid;
  if( valid ){
    ServerStateSubscriber::instance().setStatus(1);
  } else{
    ServerStateSubscriber::instance().setStatus(0);
  }
}

void BuddyFriendsCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& buddyData, const Ice::Current& current) {
  InvokeClient ic = InvokeClient::create(current);
  TaskManager::instance().execute(new FillTask(buddyData));
}

BuddyFriendsDataIPtr BuddyFriendsCacheManagerI::getBuddyFriendsDataI(::Ice::Int userId) {
  return ServiceI::instance().locateObject<BuddyFriendsDataIPtr> (
      BUDDY_FRIENDS_CACHE_DATA, userId);
}

/*****************************************************************************/

Ice::ObjectPtr BuddyFriendsCacheFactory::create(Ice::Int id) {
  TaskManager::instance().execute(new ReloadTask(id));
  return new BuddyFriendsDataI;
}
/*Ice::ObjectPtr BuddyFriendsCacheFactory::create(Ice::Int id) {
  std::map<int, int> orderFriend;
  Statement sql;
  sql     << "SELECT guest"
  << " FROM relation_" << (id % 100)
  << " WHERE host = " << id;
  ostringstream pattern;
  pattern << "relation_" << (id % 100);
  QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
  sql, OrderFriendsAscHandler( orderFriend, "guest", "guest" ));

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

  MyUtil::IntSeq buddies;
  for (map<int, int>::iterator mapIt = orderFriend.begin(); mapIt
  != orderFriend.end(); ++mapIt ){
  buddies.push_back(mapIt->first);
  }

  if( filted ){
  MCE_INFO("[BuddyFriendsCacheFactory::create] userId=" << id << " size=" << buddies.size() << " + filterd: " << filter_str.str() );
  } else{
  MCE_INFO("[BuddyFriendsCacheFactory::create] userId=" << id << " size=" << buddies.size() );
  }

  BuddyFriendsDataIPtr obj = new BuddyFriendsDataI(buddies);
  TaskManager::instance().execute(new ReloadTask(id));
  return obj;
  }
 */
/*****************************************************************************/
/*bool OrderFriendsAscHandler::handle(mysqlpp::Row& res) const {
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
 */
/************Tasks ***********************************************************/

void FillTask::handle() {
  MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
  for ( std::map<long, Ice::ObjectPtr>::const_iterator it = _buddyData->data.begin();
      it!= _buddyData->data.end(); ++it ){
    try{
      BuddyFriendsDataIPtr obj = new BuddyFriendsDataI( BuddyFriendsDataPtr::dynamicCast( it->second ) );
      res->data.insert( make_pair<long, Ice::ObjectPtr>( it->first, obj ) );
    } catch( Ice::Exception e ){
      MCE_WARN( "FillTask " << it->first << " found Ice::Exception : " << e.what() );
      continue;
    } catch( ... ){
      MCE_WARN( "FillTask " << it->first << " found unknown exception ." );
      continue;
    }
  }

  ServiceI::instance().getObjectCache()->addObjects(BUDDY_FRIENDS_CACHE_DATA, res->data);
  MCE_INFO("FillTask done. Task size / ObjectCache size : " << res->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory(BUDDY_FRIENDS_CACHE_DATA)->size());
}

void LoadTask::handle() {
  if (_obj) {
    try {
      BuddyFriendsDataIPtr obj = new BuddyFriendsDataI( BuddyFriendsDataPtr::dynamicCast(_obj) ) ;
      ServiceI::instance().addObject(obj,BUDDY_FRIENDS_CACHE_DATA,_id);
      MCE_INFO( "[LoadTask] userId=" << _id << " size=" << obj->getCount() );
    } catch( Ice::Exception e ){
      MCE_WARN("[LoadTask] userId=" << _id << " found Ice::Exception : " << e.what() );
    } catch( ... ){
      MCE_WARN("[LoadTask] userId=" << _id << " found unknown exception ." );
    }
  }
}

void ReloadTask::handle() {
  BuddyFriendsLoaderAdapter::instance().reload(_id);
}

void RemoveTask::handle() {
  BuddyFriendsDataIPtr res = ServiceI::instance().findObject<BuddyFriendsDataIPtr> (
      BUDDY_FRIENDS_CACHE_DATA, _hostId);
  if( !res ){
    return;
  }
  res->remove(_guestId);
}

void AddTask::handle() {
  BuddyFriendsDataIPtr res = ServiceI::instance().findObject<BuddyFriendsDataIPtr> (
      BUDDY_FRIENDS_CACHE_DATA, _hostId);
  if( !res ){
    return;
  }
  res->add(_guestId);
}


/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "Fill", TASK_LEVEL_BUDDY_FRIENDS_CACHE_FILL, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_FRIENDS_CACHE_LOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_FRIENDS_CACHE_RELOAD, ThreadPoolConfig(1,100));
  TaskManager::instance().config(props, "Remove", TASK_LEVEL_BUDDY_FRIENDS_CACHE_REMOVE, ThreadPoolConfig(1,100));
}

