#include "BuddyFriendsLoaderI.h"
#include "BuddyFriendsAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "StatusCache.h"

using namespace std;
using namespace xce::buddy;
using namespace xce::buddyfriends;
using namespace xce::buddy::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::cacheloader;

void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&BuddyFriendsLoaderI::instance(), service.createIdentity("L", ""));

  TaskManager::instance().config(TASK_LEVEL_BUDDY_FRIENDS_LOADER_LOAD, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_FRIENDS_LOADER_RELOAD, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_FRIENDS_LOADER_ADD, ThreadPoolConfig(1, 5));
  service.registerXceObserver(new ServiceSchemaI);

  SetDataHelper::instance().initialize();

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyFriendsLoader.Mod");
  int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyFriendsLoader.Interval", 5);
  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyFriendsLoader",
      &BuddyFriendsLoaderI::instance(), mod, sInterval);

}

/*****************************************************************************/
void BuddyFriendsLoaderI::load(::Ice::Int hostId,const MyUtil::IntSeq& buddies,
    const Ice::Current& current) {
  MCE_INFO("[BuddyFriendsLoaderI::load] id=" << hostId << " size=" << buddies.size() );
  TaskManager::instance().execute(new LoadTask(hostId,buddies));
}

void BuddyFriendsLoaderI::reload( ::Ice::Int id, const Ice::Current& current ){
  MCE_INFO( "[BuddyFriendsLoaderI::reload] id=" << id );
  TaskManager::instance().execute( new ReloadTask( id ) );
}

void BuddyFriendsLoaderI::add( ::Ice::Int hostId, ::Ice::Int guestId, const Ice::Current& current ){
  MCE_INFO( "[BuddyFriendsLoaderI::add] hostId=" << hostId << " guestId=" << guestId );
  TaskManager::instance().execute( new AddTask( hostId, guestId ) );
}

/*****************************************************************************/


void LoadTask::handle(){
  BuddyFriendsDataPtr obj = new BuddyFriendsData;
  _buddies.swap( obj->friends );
  //SetDataHelper::instance().invokeDelay(_id,obj);
  SetDataHelper::instance().invokeNow(_id,obj);
}

void ReloadTask::handle(){
  BuddyFriendsDataPtr obj;
  try {
    obj = BuddyFriendsDataPtr::dynamicCast( BuddyFriendsFactory::instance().create(_id) );
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

void AddTask::handle(){
  BuddyFriendsDataPtr obj_host;
  BuddyFriendsDataPtr obj_guest;
  try {
    obj_host = BuddyFriendsDataPtr::dynamicCast( BuddyFriendsFactory::instance().create(_hostId) );
    obj_guest = BuddyFriendsDataPtr::dynamicCast( BuddyFriendsFactory::instance().create(_guestId) );
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

  MyUtil::IntSeq::iterator it_host = lower_bound( obj_host->friends.begin(), obj_host->friends.end(), _guestId );
  if( it_host==obj_host->friends.end() || *it_host!=_guestId ){
    obj_host->friends.insert( it_host, _guestId );
  }

  MyUtil::IntSeq::iterator it_guest = lower_bound( obj_guest->friends.begin(), obj_guest->friends.end(), _hostId );
  if( it_guest==obj_guest->friends.end() || *it_guest!=_hostId ){
    obj_guest->friends.insert( it_guest, _hostId );
  }

  SetDataHelper::instance().invokeDelay(_hostId,obj_host);
  //SetDataHelper::instance().invokeNow(_hostId,obj_host);
  //SetDataHelper::instance().invokeDelay(_guestId,obj_guest);
  SetDataHelper::instance().invokeNow(_guestId,obj_guest);
}

/*****************************************************************************/

void SetDataHelper::initialize(){
  setter_ = new SetDataThread(*this);
  setter_->start(1024*1024).detach();
}

void SetDataHelper::invokeDelay( Ice::Int id, const Ice::ObjectPtr& o ){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
  if (datas_.size() >= 1000) mutex_.notify();
}
void SetDataHelper::invokeNow( Ice::Int id, const Ice::ObjectPtr& o ){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
  mutex_.notify();
}
void SetDataHelper::swap(vector< pair<long,Ice::ObjectPtr> >& empty_vector){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  if (datas_.size() < 1000) mutex_.timedWait(IceUtil::Time::seconds(5));
  datas_.swap(empty_vector);
}

void SetDataHelper::SetDataThread::run() {
  while (true){
    std::vector< pair<long,Ice::ObjectPtr> > to_be_set_;
    helper_.swap( to_be_set_ );
    if( to_be_set_.empty() ){
      MCE_INFO("[SetDataHelper::SetDataThread::run] setData: 0 objs." );
      continue;
    }
    ostringstream idstr;
    MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
    MyUtil::Int2IntSeqMap lists;
    MyUtil::Int2IntMap counts;
    for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator it = to_be_set_.begin();
        it != to_be_set_.end(); ++it ){
      objs->data.insert(*it);
      MyUtil::IntSeq& list = BuddyFriendsDataPtr::dynamicCast( it->second )->friends;
      lists.insert( make_pair<Ice::Int,MyUtil::IntSeq>(it->first,list) );
      counts.insert( make_pair<Ice::Int,Ice::Int>(it->first,list.size()) );
      idstr << " " << it->first << "(" << list.size() << ")";
    }
    MCE_INFO("[SetDataHelper::SetDataThread::run] setData:" << idstr.str() );
    BuddyFriendsCacheAdapter::instance().setData(objs);
  }
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_FRIENDS_LOADER_LOAD, ThreadPoolConfig(1,5));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_FRIENDS_LOADER_RELOAD, ThreadPoolConfig(1,5));
  TaskManager::instance().config(props, "Add", TASK_LEVEL_BUDDY_FRIENDS_LOADER_ADD, ThreadPoolConfig(1,5));
}

