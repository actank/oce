#include "BuddyRelationCacheI.h"
#include "ServiceI.h"
#include "BuddyRelationReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "util/cpp/InvokeMonitor.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::tokyo;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BuddyRelationCacheManagerI::instance(),
			service.createIdentity("M", ""));

	service.registerObjectCache(BUDDY_RELATION_CACHE, "BRC", &BuddyRelationCacheCaller::instance(), false);	

	// 向Controller注册
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyRelationCache.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationCache.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerBuddyRelationCache",
			&BuddyRelationCacheManagerI::instance(), mod, interval);

	TaskManager::instance().config(TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
	
	service.registerXceObserver(new ServiceSchemaI);

  //----------------------------Monitor-----------------------------------------
  MyUtil::ServiceMonitorManager::instance().start();

	if (IS_USE_TT) {
		//利用TT进行加载，但是要保证服务对应的preloader关闭，不然会同时加载,后面preloader要改成向TT加载
		MCE_INFO("BuddyRelationCache use TokyoTyrant to load!");
		TokyoTyrantTranslatePtr translate = new TokyoTyrantRelationTranslate;
		TokyoTyrantLoaderPtr loader = new TokyoTyrantRelationLoader(translate);
		loader->load();
	} else {
		MCE_INFO("BuddyRelationCache use Preloader to load!");
	}
}

/*****************************************************************************/
BuddyDesc BuddyRelationCacheManagerI::getRelation(const Relationship& relation,
    const Ice::Current& current){
  ostringstream msg;
  msg << relation.from << "/" << relation.to ;
  InvokeClient client = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  
  return BuddyRelationCacheLogic::instance().getRelation(relation);
}	

void BuddyRelationCacheManagerI::setRelation(const Relationship& relationship,  BuddyDesc buddydesc, 
                                             const Ice::Current& current){
	ostringstream msg;
  msg << relationship.from << ":" << relationship.to << " desc " << buddydesc ;
  InvokeClient client = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
  BuddyRelationCacheLogic::instance().setRelation(relationship, buddydesc);
}

RelationshipEntryList BuddyRelationCacheManagerI::getRelationBatch(const MyUtil::Int2IntMap & ids,	const Ice::Current& current){
	ostringstream msg;
  msg << ids.size() ;
  InvokeClient client = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
  RelationshipEntryList list;
	for(MyUtil::Int2IntMap::const_iterator it=ids.begin(); it!=ids.end(); ++it)
	{
		Relationship relation ;
		relation.from = it->first;
		relation.to = it->second;

	    BuddyDesc desc=BuddyRelationCacheLogic::instance().getRelation(relation);

		RelationshipEntry entry ;
		entry.relation = relation;
		entry.desc = desc;
		list.push_back(entry);
	}

	return list;
}


void BuddyRelationCacheManagerI::addBuddyRelationData(Ice::Int id, const BuddyRelationDataPtr& data, 
		const Ice::Current& current) {
	ostringstream msg;
  msg << id << " " << data->list.size() ;
  InvokeClient client = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
  BuddyRelationCacheLogic::instance().addBuddyRelationData(id, data);

	//MCE_INFO("BuddyRelationCacheManagerI::addBuddyRelationData done. id / ObjectCache size : " << id << " / " << ServiceI::instance().getObjectCache()->locateCategory(BUDDY_RELATION_CACHE)->size());
}

void BuddyRelationCacheManagerI::setData(const ::MyUtil::ObjectResultPtr& buddyData, const Ice::Current& current) {
	ostringstream msg;
  msg << buddyData->data.size() ;
  InvokeClient client = InvokeClient::create(current, msg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	BuddyRelationCacheLogic::instance().setData(buddyData);
        //TaskManager::instance().execute(new FillTask(buddyData));
}

bool BuddyRelationCacheManagerI::isValid(const Ice::Current&) {
        MCE_INFO( "[BuddyRelationCacheManagerI::isValid] " << _isValid );
        IceUtil::RWRecMutex::RLock lock(_validMutex);
        return _isValid;
}

void BuddyRelationCacheManagerI::setValid(bool valid,const Ice::Current&) {
        MCE_INFO( "[BuddyRelationCacheManagerI::setValid] " << valid );
        IceUtil::RWRecMutex::WLock lock(_validMutex);
        _isValid = valid;
        if( valid ){
                ServerStateSubscriber::instance().setStatus(1);
        } else{
                ServerStateSubscriber::instance().setStatus(0);
        }
}

/************logic **********************************************************/
EntryListHolderPtr BuddyRelationCacheLogic::readObject(uint32_t id) {
	return ServiceI::instance().locateObject<EntryListHolderPtr> (BUDDY_RELATION_CACHE, id);
}

void BuddyRelationCacheLogic::writeObject(uint32_t id, const EntryListHolderPtr& obj) {
	ServiceI::instance().getObjectCache()->addObject(BUDDY_RELATION_CACHE, id, obj);
}

void BuddyRelationCacheLogic::writeObjects(const MyUtil::ObjectResultPtr& result) {
	ServiceI::instance().getObjectCache()->addObjects(BUDDY_RELATION_CACHE, result->data);
	MCE_INFO("BuddyRelationCacheManagerI::writeObjects done. size / ObjectCache size : " << result->data.size() << " / " << ServiceI::instance().getObjectCache()->locateCategory(BUDDY_RELATION_CACHE)->size());
}
/*****************************************************************************/
Ice::ObjectPtr BuddyRelationCacheCaller::create(Ice::Int id) {
	MCE_INFO("BuddyRelationCacheCaller::create, id " << id);
	BuddyRelationLoaderAdapter::instance().reload(id);
	return new EntryListHolder(id);
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
}
/*****************************************************************************/
void TokyoTyrantRelationLoader::addObject(uint32_t id, Ice::ObjectPtr obj) {
	//MCE_DEBUG("TokyoTyrantRelationLoader::addObject, id " << id);
	ServiceI::instance().getObjectCache()->addObject(BUDDY_RELATION_CACHE, id, obj);
}
