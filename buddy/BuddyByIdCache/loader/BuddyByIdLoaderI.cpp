#include "BuddyByIdLoaderI.h"
#include "BuddyByIdReplicaAdapter.h"
#include "BuddyByNameReplicaAdapter.h"
#include "BuddyByOnlineTimeReplicaAdapter.h"
#include "BuddyByAddTimeReplicaAdapter.h"
#include "FriendCountCacheReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "AbstractCheckerAdapterFactory.h"

using namespace std;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::cacheloader;
using namespace xce::check;

void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&BuddyByIdLoaderI::instance(), service.createIdentity("L", ""));

  MyUtil::TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_LOADER_LOAD, ThreadPoolConfig(1, 5));
  MyUtil::TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_LOADER_RELOAD, ThreadPoolConfig(1, 5));
  MyUtil::TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_LOADER_ADD, ThreadPoolConfig(1, 5));
  MyUtil::TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_LOADER_STATUS_UPDATE, ThreadPoolConfig(1, 1));
  MyUtil::TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_LOADER_FACADE, ThreadPoolConfig(1, 5));
  MyUtil::TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_LOADER_COUNT_RECORD_FCC, ThreadPoolConfig(1, 1));
  MyUtil::TaskManager::instance().config(TASK_LEVEL_BUDDY_BYID_LOADER_FACADENONREALTIME, ThreadPoolConfig(1, 1));
  service.registerXceObserver(new ServiceSchemaI);

  BuddyByIdLoaderI::instance().initialize();
  StatusCacheInitThreadPtr init = new StatusCacheInitThread;
  init->start(1024*1024).detach();
  //SetDataHelperNonRealTime::instance().initialize();

  SetDataHelper::instance().initialize();

  string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyByIdCache.Controller", "ControllerBuddyByIdCache");
  int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdCache.Proxy.Timeout", 300);
  int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.ProduceBatchSize", 10000);
  int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.ConsumeBatchSize", 3000);
  int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.WriteBatchSize", 1000);
  int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.ThreadSize", 5);

  CacheLoader<BuddyByIdCacheManagerPrx>::instance().initialize( 
      new BatchUsersIdProducer,
      &BuddyByIdLoaderFactory::instance(),
      ctlEndpoint,
      cInterval,
      timeout,
      produceBatchSize,
      consumeBatchSize,
      writeBatchSize,
      threadSize );

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByIdLoader.Mod");
  int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByIdLoader.Interval", 5);
  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyByIdLoader",
      &BuddyByIdLoaderI::instance(), mod, sInterval);
}
/*****************************************************************************/
void BuddyByIdStatusCache::reload(int id){
  BuddyByIdLoaderI::instance().reload(id);
} 
/*****************************************************************************/
void BuddyByIdLoaderI::initialize(){
  MyUtil::TaskManager::instance().scheduleRepeated(&ReloadTimer::instance());
}

void BuddyByIdLoaderI::load(::Ice::Int hostId,const MyUtil::IntSeq& buddies,
    const Ice::Current& current) {
  MCE_INFO("[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [BuddyByIdLoaderI::load] id=" << hostId << " size=" << buddies.size() );
  MyUtil::TaskManager::instance().execute(new LoadTask(hostId,buddies));
}

void BuddyByIdLoaderI::reload( ::Ice::Int id, const Ice::Current& current ){
  MCE_INFO( "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [BuddyByIdLoaderI::reload] id=" << id );
  //MyUtil::TaskManager::instance().execute( new ReloadTask( id ) );
  ReloadTimer::instance().addReload(id);
}

void BuddyByIdLoaderI::add( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  MCE_INFO( "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [BuddyByIdLoaderI::add] hostId=" << hostId << " guestId=" << guestId );
  {
    ostringstream sth;
    sth << guestId;
    OperaDataPtr opera = new OperaData(hostId,"add",sth.str());
    MyUtil::TaskManager::instance().execute( new CheckAddBuddyTask(opera, "" ) );
  }
  MyUtil::TaskManager::instance().execute( new AddTask( hostId, guestId ) );
}
void BuddyByIdLoaderI::statusUpdate( ::Ice::Int userId, ::Ice::Int status, const Ice::Current& current ){
  MCE_INFO( "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [BuddyByIdLoaderI::statusUpdate] userId=" << userId << " status=" << status );
  StatusCache::instance().set( userId , status );
  MyUtil::TaskManager::instance().execute( new StatusUpdateTask( userId, status ) );
}

int BuddyByIdLoaderI::filter(::Ice::Int hostId,const MyUtil::IntSeq& buddies, const Ice::Current& current) {
  MCE_INFO( "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [BuddyByIdLoaderI::filter] id=" << hostId << " size=" << buddies.size() );
  MyUtil::IntSeq filterBuddies = BuddyFilter::instance().filter(hostId,buddies);
  return filterBuddies.size();
}

MyUtil::ByteSeq BuddyByIdLoaderI::getStatusMinMax(::Ice::Int minId,::Ice::Int maxId, const Ice::Current& current) {
  MCE_INFO("BuddyByIdLoaderI::getStatusMinMax minId = " << minId << " maxId = " << maxId);
  return StatusCache::instance().getStatusMinMax(minId, maxId);
}

/*****************************************************************************/

Ice::ObjectPtr BuddyByIdLoaderFactory::createAllFriends(Ice::Int id ){
  BuddyByIdDataPtr obj = new BuddyByIdData;
  MyUtil::IntSeq orderFriend;
  Statement sql;
  sql     << "SELECT guest"
    << " FROM relation_" << (id % 100)
    << " WHERE host = " << id;
  ostringstream pattern;
  pattern << "relation_" << (id % 100);
  QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
      sql, AllOrderByIdAscHandler( orderFriend, "guest" ));
  orderFriend.swap(obj->friends);
  return obj;
}

Ice::ObjectPtr BuddyByIdLoaderFactory::create(Ice::Int id ){
  BuddyByIdDataPtr obj = new BuddyByIdData;
  std::map<int, int> orderFriend;
  Statement sql;
  sql	<< "SELECT guest"
    << " FROM relation_" << (id % 100)
    << " WHERE host = " << id;
  ostringstream pattern;
  pattern << "relation_" << (id % 100);
  QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
      sql, OrderByIdAscHandler( orderFriend, "guest", "guest" ));

  MyUtil::IntSeq( BuddyFilter::instance().filter(id,orderFriend) ).swap( obj->friends );
  //MyUtil::IntSeq( BuddyFilter::instance().filter(id,buddies) ).swap( obj->friends );
  return obj;
}

MyUtil::ObjectResultPtr BuddyByIdLoaderFactory::create(const MyUtil::IntSeq& ids) {
  MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
  for (size_t i = 0; i < ids.size(); ++i) {
    try {
      Ice::ObjectPtr obj = create( (Ice::Int)ids.at(i) );
      if (obj) {
        res->data[ids.at(i)] = obj;
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("[create] Ice::Exception " << ids.at(i) << ": " << e.what());
    } catch (std::exception& e) {
      MCE_WARN("[create] std::excetpion " << ids.at(i) << ": " << e.what());
    } catch (...) {
      MCE_WARN("[create] exception" << ids.at(i) );
    }
  }
  return res;
}

MyUtil::IntSeq BuddyFilter::filter(int userId, const MyUtil::IntSeq& buddies){
  std::map<int, int> orderFriend;
  for (MyUtil::IntSeq::const_iterator it = buddies.begin(); it
      != buddies.end(); ++it ){
    orderFriend.insert( make_pair(*it,*it) );
  }
  return filter(userId,orderFriend);
}

MyUtil::IntSeq BuddyFilter::filter(int userId, std::map<int, int>& orderFriend){
  return StatusCache::instance().filter( userId, orderFriend );
}
/*****************************************************************************/
bool OrderByIdAscHandler::handle(mysqlpp::Row& res) const {
  _data.insert(make_pair((int) res[_keyField.c_str()],
        (int) res[_valueField.c_str()]));
  return true;
}

bool AllOrderByIdAscHandler::handle(mysqlpp::Row& res) const {
  _data.push_back((int) res[_keyField.c_str()]);
  return true;
}

/*
   bool StatusFilterHandler::handle(mysqlpp::Row& res) const {
   int status = (int) res[_statusField.c_str()];
   if( _statusField == "" || 0 == status ){
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

/*****************************************************************************/


void LoadTask::handle(){
  BuddyByIdDataPtr obj = new BuddyByIdData;
  MyUtil::IntSeq( BuddyFilter::instance().filter(_id,_buddies) ).swap( obj->friends );
  //SetDataHelper::instance().invokeDelay(_id,obj);
  SetDataHelper::instance().invokeNow(_id,obj);
}

void ReloadTask::handle(){
  BuddyByIdDataPtr obj;
  try {
    obj = BuddyByIdDataPtr::dynamicCast( BuddyByIdLoaderFactory::instance().create(_id) );
  } catch (Ice::Exception& e) {
    MCE_WARN("[ReloadTask] Ice::Exception (create:" << _id << ") : " << e.what());
    return;
  } catch (mysqlpp::Exception& e) {
    MCE_WARN("[ReloadTask] mysqlpp::Exception (create:" << _id << ") : " << e.what());
    return;
  } catch (std::exception& e) {
    MCE_WARN("[ReloadTask] std::excetpion (create:" << _id << ") : " << ": " << e.what());
    return;
  } catch (...) {
    MCE_WARN("[ReloadTask] unknown exception (create:" << _id << ") ." );
    return;
  }
  if( obj == 0 ){
    return;
  }
  SetDataHelper::instance().invokeDelay(_id,obj);
  //SetDataHelper::instance().invokeNow(_id,obj);
}

void BuddyByIdLoaderI::ReloadTimer::handle() {
  MCE_INFO( "Reload Timer begin" );
  std::vector<int> todo;
  swapReload(todo);
  for(std::vector<int>::iterator it = todo.begin();it!= todo.end();++it){
    MyUtil::TaskManager::instance().execute( new ReloadTask( *it ) );
  }
  MCE_INFO( "Reload Timer end" );
}

void StatusUpdateTask::handle(){
  MCE_INFO("[StatusUpdateTask]  (updater=" << _id << ") : ");
  BuddyByIdDataPtr obj;
  try {
    obj = BuddyByIdDataPtr::dynamicCast( BuddyByIdLoaderFactory::instance().createAllFriends(_id) );
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
  if( obj == 0 ){
    MCE_INFO("[StatusUpdateTask]  (updater=" << _id << ") : obj == null");
    return;
  }
  SetDataHelper::instance().invokeDelay(_id,obj);
  //SetDataHelper::instance().invokeNow(_id,obj);

  if( obj->friends.size() == 0 ){
    MCE_INFO("[StatusUpdateTask]  (updater=" << _id << ") : has no friend ");
    return;
  }
  MyUtil::ObjectResultPtr objs;
  try {
    objs = BuddyByIdLoaderFactory::instance().create( obj->friends );
  } catch (Ice::Exception& e) {
    MCE_WARN("[StatusUpdateTask] Ice::Exception create friends of updater(" << _id << ") : " << e.what());
    return;
  } catch (mysqlpp::Exception& e) {
    MCE_WARN("[StatusUpdateTask] mysqlpp::Exception create friends of updater(" << _id << ") : " << e.what());
    return;
  } catch (std::exception& e) {
    MCE_WARN("[StatusUpdateTask] std::excetpion create friends of updater(" << _id << ") : " << ": " << e.what());
    return;
  } catch (...) {
    MCE_WARN("[StatusUpdateTask] unknown exception create friends of updater(" << _id << ") ." );
    return;
  }

  for( map<long,Ice::ObjectPtr>::iterator it = objs->data.begin(); it != objs->data.end(); ++it ){
    SetDataHelper::instance().invokeDelay(it->first,it->second);
    //SetDataHelper::instance().invokeNow(_id,obj);
  }
}

void AddTask::handle(){
  BuddyByIdDataPtr obj_host;
  BuddyByIdDataPtr obj_guest;
  try {
    obj_host = BuddyByIdDataPtr::dynamicCast( BuddyByIdLoaderFactory::instance().create(_hostId) );
    obj_guest = BuddyByIdDataPtr::dynamicCast( BuddyByIdLoaderFactory::instance().create(_guestId) );
  } catch (Ice::Exception& e) {
    MCE_WARN("[AddTask] Ice::Exception (host=" << _hostId << " guest= " << _guestId << ") : " << e.what());
    return;
  } catch (mysqlpp::Exception& e) {
    MCE_WARN("[AddTask] mysqlpp::Exception (host=" << _hostId << " guest= " << _guestId << ") : " << e.what());
    return;
  } catch (std::exception& e) {
    MCE_WARN("[AddTask] std::excetpion (host=" << _hostId << " guest= " << _guestId << ") : " << ": " << e.what());
    return;
  } catch (...) {
    MCE_WARN("[AddTask] unknown exception (host=" << _hostId << " guest= " << _guestId << ") ." );
    return;
  }

  if(StatusCache::instance().isGoodUser(_guestId)){	
    MyUtil::IntSeq::iterator it_host = lower_bound( obj_host->friends.begin(), obj_host->friends.end(), _guestId );
    if( it_host==obj_host->friends.end() || *it_host!=_guestId ){
      obj_host->friends.insert( it_host, _guestId );
    }
  }

  if(StatusCache::instance().isGoodUser(_hostId)){
    MyUtil::IntSeq::iterator it_guest = lower_bound( obj_guest->friends.begin(), obj_guest->friends.end(), _hostId );
    if( it_guest==obj_guest->friends.end() || *it_guest!=_hostId ){
      obj_guest->friends.insert( it_guest, _hostId );
    }
  }

  SetDataHelper::instance().invokeDelay(_hostId,obj_host);
  //SetDataHelper::instance().invokeNow(_hostId,obj_host);
  //SetDataHelper::instance().invokeDelay(_guestId,obj_guest);
  SetDataHelper::instance().invokeNow(_guestId,obj_guest);
}

/*****************************************************************************/

void FacadeTask::handle(){
  try {
    BuddyByNameLoaderAdapter::instance().load( _lists );
  } catch (Ice::Exception& e) {
    MCE_WARN("[FacadeTask] Ice::Exception when notify BuddyByNameLoader: " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[FacadeTask] std::exception when notify BuddyByNameLoader: " << e.what());
  } catch (...) {
    MCE_WARN("[FacadeTask] unknown exception when notify BuddyByNameLoader" );
  }

  try {
    BuddyByOnlineTimeLoaderAdapter::instance().load( _lists );
  } catch (Ice::Exception& e) {
    MCE_WARN("[FacadeTask] Ice::Exception when notify BuddyByOnlineTimeLoader: " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[FacadeTask] std::exception when notify BuddyByOnlineTimeLoader: " << e.what());
  } catch (...) {
    MCE_WARN("[FacadeTask] unknown exception when notify BuddyByOnlineTimeLoader" );
  }

  try {
    BuddyByAddTimeLoaderAdapter::instance().load( _lists );
  } catch (Ice::Exception& e) {
    MCE_WARN("[FacadeTask] Ice::Exception when notify BuddyByAddTimeLoader: " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[FacadeTask] std::exception when notify BuddyByAddTimeLoader: " << e.what());
  } catch (...) {
    MCE_WARN("[FacadeTask] unknown exception when notify BuddyByAddTimeLoader" );
  }
}

void FacadeNonRealTimeTask::handle(){
  try {
    BuddyByNameLoaderAdapter::instance().load( _lists );
  } catch (Ice::Exception& e) {
    MCE_WARN("[FacadeTask] Ice::Exception when notify BuddyByNameLoader: " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[FacadeTask] std::exception when notify BuddyByNameLoader: " << e.what());
  } catch (...) {
    MCE_WARN("[FacadeTask] unknown exception when notify BuddyByNameLoader" );
  }

  try {
    BuddyByOnlineTimeLoaderAdapter::instance().load( _lists );
  } catch (Ice::Exception& e) {
    MCE_WARN("[FacadeTask] Ice::Exception when notify BuddyByOnlineTimeLoader: " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[FacadeTask] std::exception when notify BuddyByOnlineTimeLoader: " << e.what());
  } catch (...) {
    MCE_WARN("[FacadeTask] unknown exception when notify BuddyByOnlineTimeLoader" );
  }

  try {
    BuddyByAddTimeLoaderAdapter::instance().load( _lists );
  } catch (Ice::Exception& e) {
    MCE_WARN("[FacadeTask] Ice::Exception when notify BuddyByAddTimeLoader: " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[FacadeTask] std::exception when notify BuddyByAddTimeLoader: " << e.what());
  } catch (...) {
    MCE_WARN("[FacadeTask] unknown exception when notify BuddyByAddTimeLoader" );
  }
}

void CountRecordFCCTask::handle(){
  if( _counts.empty() ){
    return;
  }
  FriendCountCacheAdapter::instance().setFriendCounts( _counts );
  MCE_INFO("[CountRecordFCCTask] record: " << _counts.size() << " counts." );
}

/*****************************************************************************/

void SetDataHelper::initialize(){
  setter_ = new SetDataThread(*this);
  setter_->start(1024*1024).detach();
}

void SetDataHelper::invokeDelay( Ice::Int id, const Ice::ObjectPtr& o ){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
  if (datas_.size() >= MAX_DATA_SIZE ) mutex_.notify();
}
void SetDataHelper::invokeNow( Ice::Int id, const Ice::ObjectPtr& o ){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
  mutex_.notify();
}
void SetDataHelper::swap(vector< pair<long,Ice::ObjectPtr> >& empty_vector){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  if (datas_.size() < MAX_DATA_SIZE ) mutex_.timedWait(IceUtil::Time::seconds(3));
  datas_.swap(empty_vector);
}
void SetDataHelper::work(std::vector< pair<long,Ice::ObjectPtr> >& todo){
  if(todo.end()-todo.begin() <= MAX_DATA_SIZE){
    notify(todo);
    return;
  }
  vector<pair<long,Ice::ObjectPtr> > dovec(todo.begin(),todo.begin()+MAX_DATA_SIZE-1);
  notify(dovec);
  vector<pair<long,Ice::ObjectPtr> > willdo(todo.begin()+MAX_DATA_SIZE,todo.end());
  work(willdo);	
}
void SetDataHelper::notify(std::vector< pair<long,Ice::ObjectPtr> >& todo){
  ostringstream idstr;
  MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
  MyUtil::Int2IntSeqMap lists;
  MyUtil::Int2IntMap counts;
  for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator it = todo.begin();
      it != todo.end(); ++it ){
    MyUtil::IntSeq& list = BuddyByIdDataPtr::dynamicCast( it->second )->friends;
    std::pair<MyUtil::Int2IntSeqMap::iterator,bool> ret;
    ret = lists.insert( make_pair<Ice::Int,MyUtil::IntSeq>(it->first,list) );
    if(ret.second){
      objs->data.insert(*it);
      counts.insert( make_pair<Ice::Int,Ice::Int>(it->first,list.size()) );
    }else{
      objs->data[it->first] = it->second;
      lists[it->first] = list;
      counts[it->first] = list.size();	
    }
    idstr << " " << it->first << "(" << list.size() << ")";
  }
  for ( MyUtil::Int2IntMap::iterator it = counts.begin(); it != counts.end(); ++it){
    ostringstream sth;
    sth << it->second;
    OperaDataPtr opera = new OperaData(it->first,"setData",sth.str());
    TaskManager::instance().execute( new CheckBuddyDataTask(opera, sth.str() ) );
  }
  MCE_INFO("[SetDataHelper::SetDataThread::run] setData:" << idstr.str() );
  BuddyByIdCacheAdapter::instance().setData(objs);
  TaskManager::instance().execute( new FacadeTask( lists ) );
  TaskManager::instance().execute( new CountRecordFCCTask( counts ) );
}

void SetDataHelper::SetDataThread::run() {
  while (true){
    std::vector< pair<long,Ice::ObjectPtr> > to_be_set_;
    helper_.swap( to_be_set_ );
    if( to_be_set_.empty() ){
      MCE_INFO("[SetDataHelper::SetDataThread::run] setData: 0 objs." );
      continue;
    }

    helper_.work( to_be_set_ ); 
  }
}

/*****************************************************************************/
void SetDataHelperNonRealTime::initialize(){
  setter_ = new SetDataThread(*this);
  setter_->start(1024*1024).detach();
}

void SetDataHelperNonRealTime::invokeDelay( Ice::Int id, const Ice::ObjectPtr& o ){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
  if (datas_.size() >= 30) mutex_.notify();
}
void SetDataHelperNonRealTime::invokeNow( Ice::Int id, const Ice::ObjectPtr& o ){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
  mutex_.notify();
}
void SetDataHelperNonRealTime::swap(vector< pair<long,Ice::ObjectPtr> >& empty_vector){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  if (datas_.size() < 30) mutex_.timedWait(IceUtil::Time::seconds(3));
  datas_.swap(empty_vector);
}
void SetDataHelperNonRealTime::SetDataThread::run() {
  while (true){
    std::vector< pair<long,Ice::ObjectPtr> > to_be_set_;
    helper_.swap( to_be_set_ );
    if( to_be_set_.empty() ){
      MCE_INFO("[SetDataHelperNonRealTime::SetDataThread::run] setData: 0 objs." );
      continue;
    }
    ostringstream idstr;
    MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
    MyUtil::Int2IntSeqMap lists;
    MyUtil::Int2IntMap counts;
    for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator it = to_be_set_.begin();
        it != to_be_set_.end(); ++it ){
      objs->data.insert(*it);
      MyUtil::IntSeq& list = BuddyByIdDataPtr::dynamicCast( it->second )->friends;
      lists.insert( make_pair<Ice::Int,MyUtil::IntSeq>(it->first,list) );
      counts.insert( make_pair<Ice::Int,Ice::Int>(it->first,list.size()) );
      idstr << " " << it->first << "(" << list.size() << ")";
    }
    MCE_INFO("[SetDataHelperNonRealTime::SetDataThread::run] setData:" << idstr.str() );
    BuddyByIdCacheAdapter::instance().setData(objs);
    MyUtil::TaskManager::instance().execute( new FacadeTask( lists ) );
    MyUtil::TaskManager::instance().execute( new CountRecordFCCTask( counts ) );
    //nanosleep(&_expired,NULL);
  }
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  MyUtil::TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_BYID_LOADER_LOAD, MyUtil::ThreadPoolConfig(1,5));
  MyUtil::TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_BYID_LOADER_RELOAD, MyUtil::ThreadPoolConfig(1,5));
  MyUtil::TaskManager::instance().config(props, "Add", TASK_LEVEL_BUDDY_BYID_LOADER_ADD, MyUtil::ThreadPoolConfig(1,5));
  MyUtil::TaskManager::instance().config(props, "StatusUpdate", TASK_LEVEL_BUDDY_BYID_LOADER_STATUS_UPDATE, MyUtil::ThreadPoolConfig(1,1));
  MyUtil::TaskManager::instance().config(props, "Facade", TASK_LEVEL_BUDDY_BYID_LOADER_FACADE, MyUtil::ThreadPoolConfig(1,5));
  MyUtil::TaskManager::instance().config(props, "CountRecordFCC", TASK_LEVEL_BUDDY_BYID_LOADER_COUNT_RECORD_FCC, MyUtil::ThreadPoolConfig(1,1));
  MyUtil::TaskManager::instance().config(props, "FacadeNonRealTime", TASK_LEVEL_BUDDY_BYID_LOADER_FACADENONREALTIME, MyUtil::ThreadPoolConfig(1, 1));
}

void CheckAddBuddyTask::handle() {
  AbstractCheckerAdapterFactory::instance().getBuddyChecker("BuddyByIdLoaderCheckAddBuddy","@CheckerBuddy", 
      "CheckerAddBuddy","BuddyByIdLoader","BuddyByIdLoader0")->asycNotify(_opera,_checksum);
}

void CheckBuddyDataTask::handle() {
  AbstractCheckerAdapterFactory::instance().getBuddyChecker("BuddyByIdLoaderCheckBuddyData","@CheckerBuddy", 
      "CheckerBuddyData","BuddyByIdLoader","BuddyByIdLoader0")->asycAdd(_opera,_checksum);
}
