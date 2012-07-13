#include "BuddyByNameLoaderI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace xce::usercache;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::buddybynameloader;
using namespace xce::buddybyname;

void MyUtil::initialize(){
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&BuddyByNameLoaderI::instance(), service.createIdentity("L", ""));

  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYNAME_LOADER_LOAD, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYNAME_LOADER_RELOAD, ThreadPoolConfig(1, 5));
  TaskManager::instance().config(TASK_LEVEL_BUDDY_BYNAME_LOADER_RENAME, ThreadPoolConfig(1, 1));
  service.registerXceObserver(new ServiceSchemaI);

  SetDataHelper::instance().initialize();

  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyByNameLoader.Mod");
  int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyByNameLoader.Interval", 5);
  // 向Controller注册
  ServerStateSubscriber::instance().initialize("ControllerBuddyByNameLoader",
      &BuddyByNameLoaderI::instance(), mod, sInterval);

}

/*****************************************************************************/

void BuddyByNameLoaderI::reload(::Ice::Int hostId, const Ice::Current& current ){
  MCE_INFO( "[BuddyByNameLoaderI::reload] id=" << hostId );
  TaskManager::instance().execute( new ReloadTask( hostId ) );
}

void BuddyByNameLoaderI::load(const MyUtil::Int2IntSeqMap& lists,
    const Ice::Current& current) {
  //MCE_INFO("[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [BuddyByNameLoaderI::load] lists.size()=" << lists.size() );
  ostringstream idstr;
  idstr << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [BuddyByNameLoaderI::load] ";
  for(MyUtil::Int2IntSeqMap::const_iterator it = lists.begin(); it != lists.end(); ++it){
    idstr << it->first << "(" << it->second.size()<<")";
  }
  MCE_INFO( idstr.str() );
  TaskManager::instance().execute(new LoadTask(lists));
}

void BuddyByNameLoaderI::rename(::Ice::Int theRenameOne, const std::string& name, const Ice::Current& current) {
  MCE_INFO("[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] [BuddyByNameLoaderI::rename] " << theRenameOne << " rename: " << name);
  MyUtil::IntSeq buddies;
  try{
    buddies = BuddyByIdCacheAdapter::instance().getFriendListAsc( theRenameOne, -1 );
  }catch(Ice::Exception& e){
    MCE_WARN( "rename getFriendList handle " << theRenameOne << " Ice::Exception " << e.what());
  }catch(std::exception& e){
    MCE_WARN( "rename getFriendList handle " << theRenameOne << " std::exception " << e.what());
  }catch(...){
    MCE_WARN( "rename getFriendList handle " << theRenameOne << " unknown exception.");
  }

  TaskManager::instance().execute( new RenameTask( buddies, theRenameOne, name ) );
}

/*****************************************************************************/
void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
  MCE_INFO("ServiceSchemaI::configure");
  TaskManager::instance().config(props, "Load", TASK_LEVEL_BUDDY_BYNAME_LOADER_LOAD, ThreadPoolConfig(1,5));
  TaskManager::instance().config(props, "Reload", TASK_LEVEL_BUDDY_BYNAME_LOADER_RELOAD, ThreadPoolConfig(1,5));
  TaskManager::instance().config(props, "Rename", TASK_LEVEL_BUDDY_BYNAME_LOADER_RENAME, ThreadPoolConfig(1,1));

  string loaderMod = props->getProperty("Service.BuddyByNameLoader.LoaderMod");
  if ("UserCache" == loaderMod) {
    MCE_DEBUG("loaderMod is UserCache");
    BuddyByNameFactory::instance().changeLoaderMod(BuddyByNameFactory::USERCACHE);
  } else if ("Db" == loaderMod) {
    MCE_DEBUG("loaderMod is Db");
    BuddyByNameFactory::instance().changeLoaderMod(BuddyByNameFactory::DB);
  }
}
