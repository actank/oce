#include "BuddyByAddTimeLoaderI.h"
#include "BuddyByIdReplicaAdapter.h"
#include "BuddyByAddTimeReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&BuddyByAddTimeLoaderI::instance(), service.createIdentity("L", ""));

  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYADDTIME_LOADER_LOAD, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYADDTIME_LOADER_RELOAD, ThreadPoolConfig(1, 5));
  service.registerXceObserver(new ServiceSchemaI);

  SetDataHelper::instance().initialize();

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByAddTimeLoader.Mod");
  int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByAddTimeLoader.Interval", 5);
  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyByAddTimeLoader",
      &BuddyByAddTimeLoaderI::instance(), mod, sInterval);

}

/*****************************************************************************/

void BuddyByAddTimeLoaderI::reload( ::Ice::Int id, const Ice::Current& current ){
  MCE_INFO( "[BuddyByAddTimeLoaderI::reload] id=" << id );
  TaskManager::instance().execute( new ReloadTask( id ) );
}

void BuddyByAddTimeLoaderI::load(const MyUtil::Int2IntSeqMap& lists,
    const Ice::Current& current) {
  MCE_INFO("[BuddyByAddTimeLoaderI::load] lists.size()=" << lists.size() );
  TaskManager::instance().execute(new LoadTask(lists));
}

/*****************************************************************************/

void SetDataHelper::initialize(){
  setter_ = new SetDataThread(*this);
  setter_->start(1024*1024).detach();
}

void SetDataHelper::invokeDelay( Ice::Int id, const Ice::ObjectPtr& o, const string& times ){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
  times_.push_back( make_pair<long,string>(id,times) );
  if (datas_.size() >= 1000) mutex_.notify();
}
void SetDataHelper::invokeNow(){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  mutex_.notify();
}
void SetDataHelper::invokeNow( Ice::Int id, const Ice::ObjectPtr& o, const string& times ){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
  datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
  times_.push_back( make_pair<long,string>(id,times) );
  mutex_.notify();
}
void SetDataHelper::swap(vector< pair<long,Ice::ObjectPtr> >& empty_datas,vector< pair<long,string> >& empty_times){
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  if (datas_.size() < 1000) mutex_.timedWait(IceUtil::Time::seconds(1));
  datas_.swap(empty_datas);
  times_.swap(empty_times);
}

void SetDataHelper::SetDataThread::run() {
  while (true){
    vector< pair<long,Ice::ObjectPtr> > datas_to_be_set_;
    vector< pair<long,string> > times_to_be_set_;
    helper_.swap( datas_to_be_set_, times_to_be_set_ );
    if( datas_to_be_set_.empty() ){
      MCE_INFO("[SetDataHelper::SetDataThread::run] setData: 0 objs." );
      continue;
    }
    ostringstream idstr;
    MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
    vector< pair<long, Ice::ObjectPtr> >::iterator itB = datas_to_be_set_.begin();
    //vector< pair<long, string> >::iterator itT = times_to_be_set_.begin();
    while( itB!=datas_to_be_set_.end() ){

      idstr << " " << itB->first << "(" << BuddyByAddTimeDataNPtr::dynamicCast( itB->second )->friends.size() << ")";
      std::pair<std::map<long, Ice::ObjectPtr>::iterator,bool> ret;
      ret = objs->data.insert(*itB);
      if(!ret.second){
        MCE_WARN("[SetDataHelper::SetDataThread::run] setData: " << itB->first << " insert "
            << BuddyByAddTimeDataPtr::dynamicCast( itB->second )->friends.size() << " failed.");
        objs->data[itB->first] = itB->second;
      }
      /*
         idstr << " " << itB->first << "(" << BuddyByAddTimeDataPtr::dynamicCast( itB->second )->friends.size() << ")";
         if( itT!=times_to_be_set_.end() ){
         BuddyByAddTimeCacheAdapter::instance().load(
         itB->first,
         BuddyByAddTimeDataPtr::dynamicCast( itB->second ),
         itT->second );
         ++itT;
         }
       */
      ++itB;
    }
    MCE_INFO("[SetDataHelper::SetDataThread::run] setData:" << idstr.str() );
    BuddyByAddTimeCacheAdapter::instance().setData(objs);
  }
}

/*****************************************************************************/

void ReloadTask::handle(){
  MyUtil::IntSeq buddies;
  try {
    buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc( _id, -1 );
  } catch( Ice::Exception e ){
    MCE_WARN("[ReloadTask] id=" << _id << " found Ice::Exception : " << e.what() );
    return;
  } catch( ... ){
    MCE_WARN("[ReloadTask] id=" << _id << " found Ice::Exception" );
    return;
  }
  multimap<int, int> orderFriend;
  try{
    Statement sql;
    sql	<< "SELECT guest, UNIX_TIMESTAMP(time) AS time FROM relation_"
      << (_id % 100)
      << " WHERE host="
      << _id;

    ostringstream pattern;
    pattern << "relation_" << (_id % 100);

    QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
        sql, OrderedListHandler( buddies, orderFriend, "time", "guest" ) );
  }catch(std::exception& e){
    MCE_WARN( "[ReloadTask] handle " << _id << " std::exception " << e.what() );
    return;
  }catch(...){
    MCE_WARN( "[ReloadTask] handle " << _id << " unknown exception." );
    return;
  }

  int time_now = (int)time(NULL);
  MyUtil::IntSeq times;
  ostringstream timestr;
  for( MyUtil::IntSeq::iterator itB=buddies.begin(); itB!=buddies.end(); ++itB ){
    times.push_back( time_now );
    timestr << time_now << " ";
  }

  for (multimap<int, int>::reverse_iterator itMap = orderFriend.rbegin(); itMap
      != orderFriend.rend(); ++itMap ){
    buddies.push_back(itMap->second);
    times.push_back(itMap->first);
    timestr << itMap->first << " ";
  }

  BuddyByAddTimeDataNPtr obj = new BuddyByAddTimeDataN;
  buddies.swap(obj->friends);
  times.swap(obj->times);

  MCE_INFO("[ReloadTask] id=" << _id << " friends.size()=" << obj->friends.size() << " times=" << timestr.str() );
  SetDataHelper::instance().invokeDelay(_id,obj,timestr.str());
}

void LoadTask::handle(){
  int time_now = (int)time(NULL);
  for( MyUtil::Int2IntSeqMap::const_iterator it = _lists.begin(); it != _lists.end(); ++it ){
    MyUtil::IntSeq buddies = it->second;
    multimap<int, int> orderFriend;
    try{
      Statement sql;
      sql	<< "SELECT guest, UNIX_TIMESTAMP(time) AS time FROM relation_"
        << (it->first % 100)
        << " WHERE host="
        << it->first;
      ostringstream pattern;
      pattern << "relation_" << (it->first % 100);
      QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
          sql, OrderedListHandler( buddies, orderFriend, "time", "guest" ));
    }catch(std::exception& e){
      MCE_WARN( "[LoadTask] handle " << it->first << " std::exception " << e.what());
    }catch(...){
      MCE_WARN( "[LoadTask] handle " << it->first << " unknown exception.");
    }

    MyUtil::IntSeq times;
    ostringstream timestr;
    for( MyUtil::IntSeq::iterator itB=buddies.begin(); itB!=buddies.end(); ++itB ){
      times.push_back( time_now );
      timestr << time_now << " ";
    }
    for (multimap<int, int>::reverse_iterator itMap = orderFriend.rbegin(); itMap
        != orderFriend.rend(); ++itMap ){
      buddies.push_back(itMap->second);
      times.push_back(itMap->first);
      timestr << itMap->first << " ";
    }
    BuddyByAddTimeDataNPtr obj = new BuddyByAddTimeDataN;
    buddies.swap(obj->friends);
    times.swap(obj->times);

    MCE_INFO("[LoadTask] id=" << it->first << " friends.size()=" << obj->friends.size() );
    SetDataHelper::instance().invokeDelay(it->first,obj,timestr.str());
  }
  SetDataHelper::instance().invokeNow();
}

/*****************************************************************************/

bool OrderedListHandler::handle(mysqlpp::Row& res) const {
  int value = (int) res[_valueField.c_str()];
  MyUtil::IntSeq::iterator it = lower_bound( _buddies.begin(), _buddies.end(), value);
  if( it!=_buddies.end() && (*it)==value ){
    _buddies.erase(it);
    _data.insert( make_pair( (int) res[_keyField.c_str()], value ) );
  }
  return true;
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_BYADDTIME_LOADER_LOAD, ThreadPoolConfig(1,5));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_BYADDTIME_LOADER_RELOAD, ThreadPoolConfig(1,5));
}

