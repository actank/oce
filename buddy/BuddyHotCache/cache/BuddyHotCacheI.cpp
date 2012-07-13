#include <boost/lexical_cast.hpp>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BuddyHotCacheI.h"
#include "util/cpp/TimeCost.h"
#include "ConnectionQuery.h"
#include "Date.h"
#include "../share/BuddyHotFactory.h"

using namespace std;
using namespace MyUtil;
using namespace xce::buddy;
using namespace com::xiaonei::xce;
using namespace ::xce::serverstate;

void MyUtil::initialize()  {

  ServiceI& service = ServiceI::instance();
  service.registerObjectCache(BHC, "BHC",new BuddyHotCacheFactory, false);
  service.getAdapter()->add(&BuddyHotCacheManagerI::instance(), service.createIdentity("M", ""));
  int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotCache.Mod",0);
  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotCache.Interval",5);
  ServerStateSubscriber::instance().initialize("ControllerBuddyHotCache",&BuddyHotCacheManagerI::instance(),mod,interval);

  TaskManager::instance().config(TASK_LEVEL_BUDDY_HOT_CACHE_FILL, ThreadPoolConfig(1,100));
  service.registerXceObserver(new ServiceSchemaI);
  /*{
    int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyHotCache.Status",0);
    ServerStateSubscriber::instance().setStatus(status);
    std::ostringstream os;
    os<<"MyUtil::initialize set status : "<<status;
    MCE_INFO( os.str() );
    }*/
}

//*******************************************************************************************************

void BuddyHotCacheManagerI::addHotFriend(::Ice::Int host, const HotDataPtr& data, const Ice::Current& current){
  TaskManager::instance().execute(new AddHotFriendTask(host, data, current));
}

void BuddyHotCacheManagerI::deleteHotFriend(::Ice::Int host, const HotDataPtr& data, const Ice::Current& current){
  TaskManager::instance().execute(new DeleteHotFriendTask(host, data, current));
}

void BuddyHotCacheManagerI::setHotFriend(::Ice::Int host, const HotDataSeq& data, const Ice::Current& current){
  TaskManager::instance().execute(new SetHotFriendTask(host, data, current));
}

MyUtil::IntSeq BuddyHotCacheManagerI::getHotFriendList(int host, const Ice::Current& current){
  ostringstream msg;
  msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17)
    << "] BuddyHotCacheManagerI::getHotFriendList host: " << host ; 
  TimeCost tc = TimeCost::create(msg.str());
  return locate(host)->getInt();
}

HotDataSeq BuddyHotCacheManagerI::getHotFriendListData(int host, const Ice::Current& current){
  ostringstream msg;
  msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17)
    << "] BuddyHotCacheManagerI::getHotFriendListData host: " << host ;    
  TimeCost tc = TimeCost::create(msg.str());
  return locate(host)->getAll();
}

int BuddyHotCacheManagerI::getHotFriendCount(int host, const Ice::Current& current){
  ostringstream msg;
  msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17)
    << "] BuddyHotCacheManagerI::getHotFriendCount host: " << host ;    
  TimeCost tc = TimeCost::create(msg.str());
  return locate(host)->getSize();
}

void BuddyHotCacheManagerI::reload(::Ice::Int host, const Ice::Current& current){
  ostringstream msg;
  msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17)
    << "] BuddyHotCacheManagerI::reload host: " << host ;    
  TimeCost tc = TimeCost::create(msg.str());
}

CriticalHotDataPtr BuddyHotCacheManagerI::locate(int host){
  return ServiceI::instance().locateObject<CriticalHotDataPtr>(BHC, host);
}

//HA
bool BuddyHotCacheManagerI::isValid(const Ice::Current&) {
  MCE_INFO( "[isValid] " << _isValid );
  IceUtil::RWRecMutex::RLock lock(_validMutex);
  return _isValid;
}

void BuddyHotCacheManagerI::setValid(bool valid,const Ice::Current&) {
  MCE_INFO( "[setValid] " << valid );
  IceUtil::RWRecMutex::WLock lock(_validMutex);
  _isValid = valid;
  if( valid ){
    ServerStateSubscriber::instance().setStatus(1);
  } else{
    ServerStateSubscriber::instance().setStatus(0);
  }
}

//preloader
void BuddyHotCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& objs, const Ice::Current& current) {
  MCE_INFO("[BuddyHotCacheManagerI::setData data size = " << objs->data.size());
  TaskManager::instance().execute(new FillTask(objs));
}

//*******************************************************************************************************

void BuddyHotCacheDAO::insertHotFriend(::Ice::Int host, const HotDataPtr& data){
  MyUtil::Date now(time(NULL));
  Statement sql;
  sql << "INSERT INTO " << DB_HOT_FRIEND_TABLE << " (HOST, GUEST, TIME, MEMO, POSITION)"
    << " VALUES ('" << host << "', '" << data->hotfriend << "', '" << now.str() << "', '" 
    << data->memo << "', '" << data->position << "') ON DUPLICATE KEY UPDATE TIME = VALUES(TIME), \
    MEMO = VALUES(MEMO), POSITION = VALUES(POSITION)";
  QueryRunner(DB_BUDDY_HOT_FRIEND, CDbWServer,DB_HOT_FRIEND_TABLE).execute(sql);
}

void BuddyHotCacheDAO::deleteHotFriend(::Ice::Int host, const HotDataPtr& data){
  Statement sql;
  sql << "DELETE FROM " << DB_HOT_FRIEND_TABLE << " WHERE "
    << "HOST = '" << host << "' and GUEST = '" << data->hotfriend << "'";
  QueryRunner(DB_BUDDY_HOT_FRIEND, CDbWServer, DB_HOT_FRIEND_TABLE).execute(sql);
}

void BuddyHotCacheDAO::deleteAll(::Ice::Int host ){
  Statement sql;
  sql << "DELETE FROM " << DB_HOT_FRIEND_TABLE << " WHERE "
    << "HOST = '" << host << "'";
  QueryRunner(DB_BUDDY_HOT_FRIEND, CDbWServer, DB_HOT_FRIEND_TABLE).execute(sql);
}

void BuddyHotCacheDAO::insertAll(::Ice::Int host, const HotDataSeq& data){
  MyUtil::Date now(time(NULL));
  Statement sql;
  sql << "INSERT INTO " << DB_HOT_FRIEND_TABLE << " (HOST, GUEST, TIME, MEMO, POSITION) VALUES ";
  HotDataSeq::const_iterator it = data.begin();
  while(it != data.end()){
    sql << "('" << host << "', '" << (*it)->hotfriend << "', '" << now.str() << "', '" 
      << (*it)->memo << "', '" << (*it)->position << "')";
    if(++it != data.end()){
      sql << ", ";
    }
  } 
  sql <<" ON DUPLICATE KEY UPDATE TIME = VALUES(TIME),MEMO = VALUES(MEMO),POSITION = VALUES(POSITION)";
  QueryRunner(DB_BUDDY_HOT_FRIEND, CDbWServer,DB_HOT_FRIEND_TABLE).execute(sql);
}

//*******************************************************************************************************
void AddHotFriendTask::handle() {
  ostringstream msg;
  msg << "[" << _current.con->toString().substr(_current.con->toString().find("remote",25)+17)
    << "] BuddyHotCacheManagerI::addHotFriend host: " << _host << "data->hotfriend: "
    << _data->hotfriend << "data->position: " << _data->position ;
  TimeCost tc = TimeCost::create(msg.str());
  BuddyHotCacheDAO::instance().insertHotFriend(_host,_data);
  BuddyHotCacheManagerI::instance().locate(_host)->add(_data);
}

void DeleteHotFriendTask::handle() {
  ostringstream msg;
  msg << "[" << _current.con->toString().substr(_current.con->toString().find("remote",25)+17)
    << "] BuddyHotCacheManagerI::deleteHotFriend host: " << _host << "data->hotfriend: "
    << _data->hotfriend << "data->position: " << _data->position ;
  TimeCost tc = TimeCost::create(msg.str());
  BuddyHotCacheDAO::instance().deleteHotFriend(_host,_data);
  BuddyHotCacheManagerI::instance().locate(_host)->remove(_data);
}

void SetHotFriendTask::handle() {
  ostringstream msg;
  msg << "[" << _current.con->toString().substr(_current.con->toString().find("remote",25)+17)
    << "] BuddyHotCacheManagerI::setHotFriend host: " << _host << "datasize: " << _data.size();
  TimeCost tc = TimeCost::create(msg.str());
  BuddyHotCacheDAO::instance().deleteAll(_host);
  BuddyHotCacheManagerI::instance().locate(_host)->remove();
  BuddyHotCacheDAO::instance().insertAll(_host,_data);
  BuddyHotCacheManagerI::instance().locate(_host)->add(_data);
}

void FillTask::handle() {
  MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
  //MCE_INFO("[FillTask::handle] process data size is " << _objs->data.size());
  ostringstream idsKey;
  for ( std::map<long, Ice::ObjectPtr>::const_iterator it = _objs->data.begin();
      it!= _objs->data.end(); ++it ){
    try{
      //MCE_INFO("[FillTask::handle] id = " << it->first << " add to res");
      //MCE_DEBUG("id " << it->first << " names.size() " << BuddyByNameDataPtr::dynamicCast( it->second )->names.size() << " transfer begin");
      CriticalHotDataPtr obj = new CriticalHotData( BuddyHotDataPtr::dynamicCast( it->second ) );
      res->data.insert( make_pair<long, Ice::ObjectPtr>( it->first, obj ) );
      idsKey << it->first << "/" << obj->getSize() << " ";
    } catch( Ice::Exception e ){
      MCE_WARN( "FillTask " << it->first << " found Ice::Exception : " << e.what() );
      continue;
    } catch( ... ){
      MCE_WARN( "FillTask " << it->first << " found unknown exception ." );
      continue;
    }
  }
  ServiceI::instance().getObjectCache()->addObjects( BHC, res->data );
  MCE_DEBUG( "data.size: " << _objs->data.size() << " idsKey: " << idsKey.str());
  MCE_INFO( "FillTask done. Task size / ObjectCache size : " << res->data.size() << "/" << _objs->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory( BHC)->size() );

}

//*******************************************************************************************************

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "Fill", TASK_LEVEL_BUDDY_HOT_CACHE_FILL, ThreadPoolConfig(1,100));
}

