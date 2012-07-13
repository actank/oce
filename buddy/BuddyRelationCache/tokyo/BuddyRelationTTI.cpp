#include "BuddyRelationTTI.h"
#include "util/cpp/TimeCost.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::buddy;
using namespace xce::tokyo;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BuddyRelationTTManagerI::instance(),
			service.createIdentity("M", ""));

	// 向Controller注册
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("BuddyRelationTT.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyRelationTT.Interval", 5);
	ServerStateSubscriber::instance().initialize("ControllerBuddyRelationTT",
			&BuddyRelationTTManagerI::instance(), mod, interval);

	service.registerXceObserver(new ServiceSchemaI);

	//
}

/*****************************************************************************/
BuddyDesc BuddyRelationTTManagerI::getRelation(const Relationship& relation,
		const Ice::Current& current){
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] BuddyRelationTTManagerI::getRelation " << relation.from << "/" << relation.to ;
	TimeCost tc = TimeCost::create(msg.str());

	return BuddyRelationTTLogic::instance().getRelation(relation);
}	

void BuddyRelationTTManagerI::setRelation(const Relationship& relationship,  BuddyDesc buddydesc, 
                                             const Ice::Current& current){
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] BuddyRelationTTManagerI::setRelation, relation[" << relationship.from << ":" << relationship.to << "] desc " << buddydesc ;
	TimeCost tc = TimeCost::create(msg.str());

	BuddyRelationTTLogic::instance().setRelation(relationship, buddydesc);
}

void BuddyRelationTTManagerI::addBuddyRelationData(Ice::Int id, const BuddyRelationDataPtr& data, 
		const Ice::Current& current) {
	BuddyRelationTTLogic::instance().addBuddyRelationData(id, data);
}

bool BuddyRelationTTManagerI::isValid(const Ice::Current&) {
        MCE_INFO( "[BuddyRelationTTManagerI::isValid] " << _isValid );
        IceUtil::RWRecMutex::RLock lock(_validMutex);
        return _isValid;
}

void BuddyRelationTTManagerI::setValid(bool valid,const Ice::Current&) {
        MCE_INFO( "[BuddyRelationTTManagerI::setValid] " << valid );
        IceUtil::RWRecMutex::WLock lock(_validMutex);
        _isValid = valid;
        if( valid ){
                ServerStateSubscriber::instance().setStatus(1);
        } else{
                ServerStateSubscriber::instance().setStatus(0);
        }
}

void BuddyRelationTTManagerI::setData(const ::MyUtil::ObjectResultPtr& buddyData, const Ice::Current& current) {
	ostringstream msg;
        msg << "[" << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] BuddyRelationTTManagerI::setData, size: " << buddyData->data.size() ;
	TimeCost tc = TimeCost::create(msg.str());
        //TaskManager::instance().execute(new FillTask(buddyData));
}

/************logic **********************************************************/
void BuddyRelationTTLogic::initialize() {
	MCE_INFO("BuddyRelationTTLogic::initialize()");
	_translate = new TokyoTyrantRelationTranslate;
	_helper = new TokyoTyrantHelper(CACHE_CLUSTER, _translate);
}

EntryListHolderPtr BuddyRelationTTLogic::readObject(uint32_t id) {
	return EntryListHolderPtr::dynamicCast(_helper->read(id));
}

void BuddyRelationTTLogic::writeObject(uint32_t id, const EntryListHolderPtr& obj) {
	_helper->write(id, obj);
}

void BuddyRelationTTLogic::writeObjects(const MyUtil::ObjectResultPtr& result) {
	for (std::map<long, Ice::ObjectPtr>::const_iterator it = result->data.begin();
			it != result->data.end(); ++it) {
		writeObject((uint32_t)it->first, EntryListHolderPtr::dynamicCast(it->second));
	}
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
}

