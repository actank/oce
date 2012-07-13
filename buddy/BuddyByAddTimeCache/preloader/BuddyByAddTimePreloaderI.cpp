#include "BuddyByAddTimePreloaderI.h"
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
using namespace xce::cacheloader;

void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.registerXceObserver(new ServiceSchemaI);

  string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyByAddTimeCache.Controller", "ControllerBuddyByAddTimeCache");
  int cInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 60);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByAddTimeCache.Proxy.Timeout", 300);
  int produceBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByAddTimePreloader.ProduceBatchSize", 10000);
  int consumeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByAddTimePreloader.ConsumeBatchSize", 3000);
  int writeBatchSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByAddTimePreloader.WriteBatchSize", 1000);
  int threadSize = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByAddTimePreloader.ThreadSize", 5);

  CacheLoader<BuddyByAddTimeCacheManagerPrx>::instance().initialize( 
      new BatchUsersIdProducer,
      &BuddyByAddTimePreloaderFactory::instance(),
      ctlEndpoint,
      cInterval,
      timeout,
      produceBatchSize,
      consumeBatchSize,
      writeBatchSize,
      threadSize );
}

/*****************************************************************************/

MyUtil::ObjectResultPtr BuddyByAddTimePreloaderFactory::create(const MyUtil::IntSeq& ids) {
  MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult();
  MyUtil::Int2IntSeqMap buddymap = BuddyByIdCacheAdapter::instance().getFriendLists( ids );

  for (MyUtil::IntSeq::const_iterator idIt = ids.begin(); idIt != ids.end(); ++idIt ){ 
    MyUtil::IntSeq buddies;
    MyUtil::Int2IntSeqMap::iterator it = buddymap.find( *idIt );
    if( it != buddymap.end() ){
      buddies = it->second;
    } else{
      res->data[*idIt] = 0;
      continue;
    }

    BuddyByAddTimeDataNPtr obj = new BuddyByAddTimeDataN;
    std::multimap<int, int> orderFriend;
    try{
      Statement sql;
      sql	<< "SELECT guest, UNIX_TIMESTAMP(time) AS time FROM relation_"
        << (*idIt % 100)
        << " WHERE host="
        << *idIt;

      ostringstream pattern;
      pattern << "relation_" << (*idIt % 100);

      QueryRunner("relation_buddycache", CDbRServer, pattern.str()).query(
          sql, OrderedListHandler( buddies, orderFriend, "time", "guest" ) );
    }catch(std::exception& e){
      MCE_WARN( "Create::handle " << *idIt << " std::exception " << e.what() );
      res->data[*idIt] = 0;
      continue;
    }catch(...){
      MCE_WARN( "Create::handle " << *idIt << " unknown exception." );
      res->data[*idIt] = 0;
      continue;
    }

    int time_now = (int)time(NULL);
    MyUtil::IntSeq times;
    for( MyUtil::IntSeq::iterator itB=buddies.begin(); itB!=buddies.end(); ++itB ){
      times.push_back( time_now );
    }
    for (multimap<int, int>::reverse_iterator itMap = orderFriend.rbegin(); itMap
        != orderFriend.rend(); ++itMap ){
      buddies.push_back(itMap->second);
      times.push_back(itMap->first);
    }
    buddies.swap(obj->friends);
    times.swap(obj->times);
    res->data[*idIt] = obj;

    MCE_DEBUG("[BuddyByAddTimePreloaderFactory::create] userId=" << *idIt << " friends.size()=" << obj->friends.size() );
  }
  return res;
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
}

