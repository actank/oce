
#include "UseredTribeI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceBox/IceBox.h>
#include <map>
#include <boost/lexical_cast.hpp>
#include "ConnectionQuery.h"
using namespace mop::hi::svc::model;
using namespace MyUtil;
using namespace std;
using namespace com::xiaonei::xce;

extern "C"
{

//
// Factory function
//
USERED_TRIBE_API ::IceBox::Service*
create(Ice::CommunicatorPtr communicator)
{
    return &UseredTribeServiceI::instance();
}

}

//---------------------------------------------------------------------------
void
UseredTribeServiceI::initialize()
{
    addServantCache<Ice::Int>(ADMINED_TRIBE_SET, "UA", new SyncServantFactory<Ice::Int>(new AdminedTribeSetFactoryI));
    addServantCache<Ice::Int>(MEMBERED_TRIBE_SET, "UM", new SyncServantFactory<Ice::Int>(new MemberedTribeSetFactoryI));
	addServantCache<Ice::Int>(TRIBE_TINY_VIEW, "T", new SyncServantFactory<Ice::Int>(new TribeTinyViewFactoryI));
	
	getAdapter()->add(new UseredTribeManagerI,
		createIdentity(USERED_TRIBE_MANAGER,""));


	ConnectionPoolManager::instance().initialize(getAdapter());
	/*
    string strStorm = getCommunicator()->getProperties()
        ->getPropertyWithDefault(
                "Service."+getName()+".IceStorm",
                "TribeStorm/TopicManager@TribeStorm.TopicManager");
    Ice::ObjectPrx obj = getCommunicator()->stringToProxy(strStorm);
    IceStorm::TopicManagerPrx topic= IceStorm::TopicManagerPrx::uncheckedCast(obj);
	MyUtil::IceStormFacadePtr _storm = new MyUtil::IceStormFacade(topic);
    
    Ice::ObjectPrx publisher = _storm->getPublisher("UseredTribe");
    _TribeTopic = TribeTopicPrx::uncheckedCast(publisher);
    	
    Ice::ObjectPrx tribe = 
        getAdapter()->addWithUUID(new UseredTribeTopicI);
    _storm->subscribe("TribeTopic", tribe, IceStorm::QoS());
	*/
	
}

//TribeTopicPrx
//TribeServiceI::getTribeTopic()
//{
//    return _TribeTopic;
//};


UseredTribeSetPtr
UseredTribeServiceI::getAdminedTribeSet(Ice::Int tribeId)
{
	return UseredTribeSetPtr::dynamicCast(findObject(ADMINED_TRIBE_SET, tribeId));
}

UseredTribeSetPtr
UseredTribeServiceI::getMemberedTribeSet(Ice::Int tribeId)
{
	return UseredTribeSetPtr::dynamicCast(findObject(MEMBERED_TRIBE_SET, tribeId));
}

TribeTinyViewPtr
UseredTribeServiceI::getTribeTinyView(Ice::Int tribeId)
{
	return TribeTinyViewPtr::dynamicCast(findObject(TRIBE_TINY_VIEW, tribeId));
}

//---------------------------------------------------------------------------
Ice::ObjectPtr
AdminedTribeSetFactoryI::create(const Ice::Int& id)
{
	MCE_DEBUG("Loading object: " << id);
	
	UseredTribeSetPtr result = new UseredTribeSetI;	
	Str2StrMap filter;
	filter["usr"] = boost::lexical_cast<string>(id);
	
	ConnectionQuery conn("tribe",CDbRServer);
	
	ResSetPtr res = conn.doSelect(
		"tribe_admin", filter);
	DefResSetPtr dr = DefResSetPtr::dynamicCast(res);
	
	for(int i = 0 ; i <dr->size(); i++)
	{
		result->tribeIds.push_back((int)dr->getCol(i,"tribe"));
	}
	
	MCE_DEBUG("Loading object: " << id << " done.");
	return result;
}

Ice::ObjectPtr
MemberedTribeSetFactoryI::create(const Ice::Int& id)
{
	MCE_DEBUG("Loading object: " << id);
	
	UseredTribeSetPtr result = new UseredTribeSetI;
	ostringstream sql;
	sql<< "select * from tribe_member where member = " << id << " and disc < 2";
	//Str2StrMap filter;
	//filter["member"] = boost::lexical_cast<string>(id);
	ConnectionQuery conn("tribe",CDbRServer);
	
	ResSetPtr res = conn.doSelect(sql.str());
	DefResSetPtr dr = DefResSetPtr::dynamicCast(res);
	
	for(int i = 0 ;i <dr->size(); i++)
	{
		result->tribeIds.push_back((int)dr->getCol(i,"tribe"));
	}

	MCE_DEBUG("Loading object: " << id << " done.");
	return result;
}

Ice::ObjectPtr
TribeTinyViewFactoryI::create(const Ice::Int& id)
{
	MCE_DEBUG("Loading object: " << id);
	ostringstream sql;
	sql << "select id, name, head_url, usercount from tribe where id=" << id;
	
	ConnectionQuery conn("tribe",CDbRServer);
	DefResSetPtr rs = DefResSetPtr::dynamicCast(
		conn.doSelect( sql.str()));
	if (rs->empty()) {
		return 0;
	}
	TribeTinyViewPtr result = new TribeTinyView;
	result->id = id;
	result->name = rs->getCol(0, "name").get_string();
	result->headurl = rs->getCol(0, "head_url").get_string();
	result->usercount =(int) rs->getCol(0, "usercount");
	
	MCE_DEBUG("Loading object: " << id << " done.");
	return result;
}

//---------------------------------------------------------------------------
::Ice::Int               
mop::hi::svc::model::UseredTribeManagerI::getAdminedTribeCount(::Ice::Int userId,
                               const Ice::Current& current)
{
	UseredTribeServiceI& service = UseredTribeServiceI::instance();
	return service.getAdminedTribeSet(userId)->size();
}

::Ice::Int               
mop::hi::svc::model::UseredTribeManagerI::getMemberedTribeCount(::Ice::Int userId,
                               const Ice::Current& current)
{
	UseredTribeServiceI& service = UseredTribeServiceI::instance();
	return service.getMemberedTribeSet(userId)->size();
}

MyUtil::IntSeq
mop::hi::svc::model::UseredTribeManagerI::getAdminedTribes(::Ice::Int userId,
                               ::Ice::Int offset,
                               ::Ice::Int limit,
                               const Ice::Current& current)
{
	UseredTribeServiceI& service = UseredTribeServiceI::instance();
    return service.getAdminedTribeSet(userId)
    		->getAll(offset, limit);
}     

MyUtil::IntSeq
mop::hi::svc::model::UseredTribeManagerI::getMemberedTribes(::Ice::Int userId,
                               ::Ice::Int offset,
                               ::Ice::Int limit,
                               const Ice::Current& current)
{
	UseredTribeServiceI& service = UseredTribeServiceI::instance();
    return service.getMemberedTribeSet(userId)
    		->getAll(offset, limit);
}   

TribeTinyViewSeq
mop::hi::svc::model::UseredTribeManagerI::getAdminedTribeTinyViews(::Ice::Int userId,
                               ::Ice::Int offset,
                               ::Ice::Int limit,
                               const Ice::Current& current)
{
	UseredTribeServiceI& service = UseredTribeServiceI::instance();
    IntSeq adminedTribeIds = service.getAdminedTribeSet(userId)
    		->getAll(offset, limit);
   	TribeTinyViewSeq result;
    if (offset <= 0) {
    	offset = 0;
    }
    if (limit <= 0) {
    	limit = adminedTribeIds.size();
    }
    for (int i = 0; i < adminedTribeIds.size(); ++i) {
    	if (i < offset) {
    		continue;
    	}
    	if (result.size() >= limit) {
    		break;
    	}
    	try {
    		result.push_back(service.getTribeTinyView(adminedTribeIds.at(i)));
    	} catch (Ice::Exception& e) {
    	
    	}
    }
    return result;    
}     

TribeTinyViewSeq
mop::hi::svc::model::UseredTribeManagerI::getMemberedTribeTinyViews(::Ice::Int userId,
                               ::Ice::Int offset,
                               ::Ice::Int limit,
                               const Ice::Current& current)
{
	UseredTribeServiceI& service = UseredTribeServiceI::instance();
    IntSeq memberedTribeIds = service.getMemberedTribeSet(userId)
    		->getAll(offset, limit);
   	TribeTinyViewSeq result;
    if (offset <= 0) {
    	offset = 0;
    }
    if (limit <= 0) {
    	limit = memberedTribeIds.size();
    }
    for (int i = 0; i < memberedTribeIds.size(); ++i) {
    	if (i < offset) {
    		continue;
    	}
    	if (result.size() >= limit) {
    		break;
    	}
    	try {
    		result.push_back(service.getTribeTinyView(memberedTribeIds.at(i)));
    	} catch (Ice::Exception& e) {
    	
    	}
    }
    return result;    
}   
 

void 
mop::hi::svc::model::UseredTribeManagerI::addMemberedTribe(::Ice::Int userId, ::Ice::Int tribeId, const Ice::Current&)
{
	UseredTribeServiceI::instance().getMemberedTribeSet(userId)->add(tribeId);
}

void 
mop::hi::svc::model::UseredTribeManagerI::removeMemberedTribe(::Ice::Int userId, ::Ice::Int tribeId, const Ice::Current&)
{
	UseredTribeServiceI::instance().getMemberedTribeSet(userId)->remove(tribeId);
}

void 
mop::hi::svc::model::UseredTribeManagerI::addAdminedTribe(::Ice::Int userId, ::Ice::Int tribeId, const Ice::Current&)
{
	UseredTribeServiceI::instance().getAdminedTribeSet(userId)->add(tribeId);
}

void 
mop::hi::svc::model::UseredTribeManagerI::removeAdminedTribe(::Ice::Int userId, ::Ice::Int tribeId, const Ice::Current&)
{
	UseredTribeServiceI::instance().getAdminedTribeSet(userId)->remove(tribeId);
}


void
mop::hi::svc::model::UseredTribeManagerI::reloadTribeTinyView(::Ice::Int tribeId, const Ice::Current&)
{
	UseredTribeServiceI& service = UseredTribeServiceI::instance();
	service.removeObject(TRIBE_TINY_VIEW, tribeId);
}

//---------------------------------------------------------------------------
::Ice::Int
mop::hi::svc::model::UseredTribeSetI::size(const Ice::Current& current)
{
	IceUtil::Mutex::Lock lock(*this);
	return tribeIds.size();
}

void
mop::hi::svc::model::UseredTribeSetI::add(Ice::Int tribeId, const Ice::Current& current)
{
	MCE_DEBUG(current.id.name << " : UseredTribeSetI::add("<<tribeId<<")");
	IceUtil::Mutex::Lock lock(*this);
	if (find(tribeIds.begin(), tribeIds.end(), tribeId) != tribeIds.end()) {
		return;
	}
	tribeIds.push_back(tribeId);
}

void
mop::hi::svc::model::UseredTribeSetI::remove(Ice::Int tribeId, const Ice::Current& current)
{
	MCE_DEBUG(current.id.name << " : UseredTribeSetI::remove("<<tribeId<<")");
	IceUtil::Mutex::Lock lock(*this);
	tribeIds.erase(std::remove(tribeIds.begin(), tribeIds.end(), tribeId), 
		tribeIds.end());
}

::MyUtil::IntSeq
mop::hi::svc::model::UseredTribeSetI::getAll(::Ice::Int begin,
                         ::Ice::Int limit,
                         const Ice::Current& current)
{
	if (begin < 0) {
		begin = 0;
	}
	IceUtil::Mutex::Lock lock(*this);
	IntSeq result;
	for(IntSeq::iterator it = tribeIds.begin();
		it != tribeIds.end();
		++it)
	{
		if(begin-- > 0)	continue;
		result.push_back(*it);
		if(limit > 0 && result.size() >= limit) break;
	}
    return result;
}


//---------------------------------------------------------------------------
