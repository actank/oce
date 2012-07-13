#include "RelationI.h"
#include "RelationServiceI.h"
#include "RelationLogicAdapter.h"
#include "RelationAdapter.h"
#include "MainAdapter.h"
#include "RelationCommon.h"

#include <boost/lexical_cast.hpp>

using namespace mop::hi::svc::relation;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;


//---------------------------------------------------------------------------
void
RelationManagerI::addRequest(::Ice::Int fromId,
						     ::Ice::Int toId,
						     const ::MyUtil::Str2StrMap& constProps,
						     const Ice::Current& current)
{
	sendRequest(toId, fromId, constProps, current);
}

void
RelationManagerI::sendRequest(::Ice::Int toId,
						     ::Ice::Int fromId,
						     const ::MyUtil::Str2StrMap& constProps,
						     const Ice::Current& current)
{
	MCE_DEBUG("addRequest.");
	if(fromId == toId) {
		MCE_WARN("can not add oneself as a friend!!!" << fromId);
		return;
	}

	MyUtil::Str2StrMap& props = const_cast<MyUtil::Str2StrMap&>(constProps);
	RelationServiceI& service = RelationServiceI::instance();
	DataHandleSetEIPtr reqGuest = service.getRequestList(toId);
	// do some check locally;
	if(reqGuest->contains(fromId)) {
		MCE_DEBUG("addRequest found existing REQUEST. return.");
		return;
	};

	DataHandleSetMIPtr buddies = service.getBuddyList(toId);
	if(buddies->contains(fromId)) {
		MCE_DEBUG("addRequest found existing BUDDY. return.");
		return;
	};

	// do some check remotely;
	InternalManagerPrx peerInternalManager = RelationAdapter::instance().getInternalManager(fromId);
	if(peerInternalManager->containsRequest(toId, fromId)) {
		MCE_DEBUG("addRequst found existing REVERSE REQUST. confirmRequest");
		RelationAdapter::instance().getRelationManager(fromId)->confirmRequest(toId, fromId);
		return;
	};

	if(peerInternalManager->containsBlock(fromId, toId)) {
		MCE_DEBUG("addRequst found existing BLOCK. RemoveBlock first.");
		RelationAdapter::instance().getRelationManager(fromId)->removeBlock(fromId, toId);
	}
	props[CRQAPPLICANT] = boost::lexical_cast<string>(fromId);
	props[CRQACCEPTER] = boost::lexical_cast<string>(toId);
	ConnectionQuery(service.getFbRequest(), CDbWServer).doInsertIgnore(TABLE_REQUEST, props);

	//reqGuest->add(fromId, props);
	DataHandlePtr ent = new RequestHandle;
	ent->value(new RequestData);
	ent->setProperties(props);
	reqGuest->add(ent);

	MCE_DEBUG("call RelationLogic to addapply.");
	RelationLogicAdapter::instance().addApply(fromId, toId, current.ctx);
}

void
RelationManagerI::confirmRequest(::Ice::Int fromId,
							 ::Ice::Int toId,
							 const Ice::Current& current)
{
	acceptRequest(toId, fromId, current);	
}

void
RelationManagerI::acceptRequest(::Ice::Int toId,
							 ::Ice::Int fromId,
							 const Ice::Current& current)
{
    MCE_TRACE("confirmRequest");
	if(fromId == toId) {
		MCE_DEBUG("confirmRequest fromId == toId : " << fromId);
		return;
	};
	RelationServiceI& service = RelationServiceI::instance();
	DataHandleSetEIPtr reqGuest = service.getRequestList(toId);
    if( ! reqGuest->contains(fromId)) {
		MCE_DEBUG("confirmRequest no request to confirm. fromId="<<fromId<<" toId="<<toId);
        return;
    }

	Str2StrMap filter;
	filter[CRQAPPLICANT]=boost::lexical_cast<string>(fromId);
	filter[CRQACCEPTER]=boost::lexical_cast<string>(toId);
	ConnectionQuery(service.getFbRequest(), CDbWServer).doDelete(TABLE_REQUEST, filter);

	reqGuest->remove(fromId);
    
    //RelationAdapter::instance().getRelationManager(fromId)->addBuddy(fromId, toId);
	addBuddy(toId, fromId);
	
    RelationLogicAdapter::instance().acceptRequest(toId, fromId, current.ctx);
}

void
RelationManagerI::rejectRequest(::Ice::Int fromId,
							::Ice::Int toId,
							const Ice::Current& current)
{
	denyRequest(toId, fromId, current);
}

void
RelationManagerI::denyRequest(::Ice::Int toId,
							::Ice::Int fromId,
							const Ice::Current& current)
{
    MCE_TRACE("rejectRequest");
	if(fromId == toId)
	{
		MCE_DEBUG("rejectRequest fromId == toId : " << fromId);
		return;
	};

	RelationServiceI& service = RelationServiceI::instance();
	DataHandleSetEIPtr reqGuest = service.getRequestList(toId);
	if( ! reqGuest->contains(fromId))
	{
		MCE_DEBUG("rejectRequest no request to reject. fromId="<<fromId<<" toId="<<toId);
		return;
	};

    MyUtil::Str2StrMap filter;
    filter[CRQAPPLICANT] = boost::lexical_cast<string>(fromId);
    filter[CRQACCEPTER] = boost::lexical_cast<string>(toId);
	ConnectionQuery(service.getFbRequest(), CDbWServer).doDelete(TABLE_REQUEST, filter);

	reqGuest->remove(fromId);
	MCE_DEBUG("notifyRejectRequest fromId:" << fromId << " -> " << " guest:" << toId);
	MCE_DEBUG("call relationLogic to denyrequest");
	RelationLogicAdapter::instance().denyRequest(toId, fromId, current.ctx);
}

void
RelationManagerI::rejectAllRequest(::Ice::Int toId,
							   const Ice::Current& current)
{
	denyAllRequest(toId, current);	
}

void
RelationManagerI::denyAllRequest(::Ice::Int toId,
							   const Ice::Current& current)
{
    MCE_TRACE("rejectAllRequest");

	RelationServiceI& service = RelationServiceI::instance();
	DataHandleSetEIPtr reqGuest = service.getRequestList(toId);
	
    MyUtil::Str2StrMap filter;
    filter[CRQACCEPTER] = boost::lexical_cast<string>(toId);
    ConnectionQuery(service.getFbRequest(), CDbWServer).doDelete(TABLE_REQUEST, filter);    	

	DataHandleSeq seq = reqGuest->getAll(0, -1);
	// TODO: This interface need proformance.
    for(DataHandleSeq::iterator it = seq.begin();
			it != seq.end();
			++it)
    {
		DataHandlePtr& ent = *it;
		int fromId = boost::lexical_cast<int>(ent->value<RequestDataPtr>()->applicant);
		MCE_DEBUG("rejectAllRequest reject: "<<fromId);
		reqGuest->remove(fromId);
        RelationLogicAdapter::instance()
			.denyRequest(toId, fromId, current.ctx);
    }
}

::Ice::Int
RelationManagerI::getRequestCount(::Ice::Int toId,
							  const Ice::Current& current)
{
    return RelationServiceI::instance().getRequestList(toId)->size();
}

::MyUtil::Str2StrMapSeq
RelationManagerI::getRequestList(::Ice::Int toId,
							 ::Ice::Int offset,
							 ::Ice::Int limit,
							 const Ice::Current& current)
{
	DataHandleSeq resultAll = RelationServiceI::instance().getRequestList(toId)
		->getAll(offset, limit);
	Str2StrMapSeq result;
	for(DataHandleSeq::iterator it = resultAll.begin();
			it != resultAll.end();
			++it)
	{
		result.push_back((*it)->getProperties());
	};
	return result;
}


MyUtil::RowSetPtr
RelationManagerI::getRequestRowSet(Ice::Int toId,
                            Ice::Int offset,
                            Ice::Int limit,
                            const Ice::Current& current)
{
    MCE_DEBUG("=======call getRequestRowSet() =========");
    DataHandleSeq ents = RelationServiceI::instance().getRequestList(toId)->getAll(offset, limit);

    if(ents.empty())
    {
        return NULL;
    }
    RowSetPtr res = RowSetHelper::boxing(ents);

    MCE_DEBUG("=======call getRequestRowSet() ========= return size = "<< res->rows.size() );
    return res;
}

mop::hi::svc::relation::RequestDataSeq
RelationManagerI::getRequestSeq(Ice::Int toId,
                            Ice::Int offset,
                            Ice::Int limit,
                            const Ice::Current& current)
{
    MCE_DEBUG("=======call getRequestRowSet() =========");
    DataHandleSeq ents = RelationServiceI::instance().getRequestList(toId)
    	->getAll(offset, limit);

    mop::hi::svc::relation::RequestDataSeq result;
	for(DataHandleSeq::iterator it = ents.begin();
			it != ents.end();
			++it)
	{
		result.push_back((*it)->value<RequestDataPtr>());
	};
	return result;
}

::MyUtil::Str2StrMap
RelationManagerI::getRequest(::Ice::Int fromId,
						     ::Ice::Int toId,
						     const Ice::Current& current)
{
    DataHandlePtr ent = RelationServiceI::instance().getRequestList(toId)->get(fromId);
	if(!ent)
	{
		return Str2StrMap();
	}
	return ent->getProperties();
}

void
RelationManagerI::addBlock(::Ice::Int sourceId,
						   ::Ice::Int targetId,
						   const Ice::Current& current)
{
	block(sourceId, targetId, current);	
}

void
RelationManagerI::block(::Ice::Int sourceId,
						   ::Ice::Int targetId,
						   const Ice::Current& current)
{
	if(sourceId == targetId)
	{
		MCE_WARN("sourceId==targetId==" << sourceId << " nothing to say.");
		return;
	};
	MCE_DEBUG("addBlock begin.");
	RelationServiceI& service = RelationServiceI::instance();
	DataHandleSetMIPtr block = service.getBlockList(sourceId);
	if( block->contains(targetId) )
	{
		MCE_DEBUG("addBlock already block");
		return;
	}

	DataHandleSetMIPtr buddy = service.getBuddyList(sourceId);
	if( buddy->contains(targetId) )
	{
		MCE_DEBUG("addBlock existing BUDDY. remove it first.");
		removeBuddy(sourceId, targetId, current);
	}

	InternalManagerPrx guestInternalManager = RelationAdapter::instance().getInternalManager(targetId);
	if( guestInternalManager->containsRequest(sourceId, targetId) )
	{
		MCE_DEBUG("addBlock existing REQUEST. remove it first.");
		RelationAdapter::instance().getRelationManager(targetId)->rejectRequest(sourceId, targetId);
	}

	MyUtil::Date now(time(NULL));

	Str2StrMap props;
	props[CBHOST]=boost::lexical_cast<string>(sourceId);
	props[CBGUEST]=boost::lexical_cast<string>(targetId);
	props[CBTIME]=now.str();

	ConnectionQuery(service.getFbBlock(), CDbWServer).doInsert(TABLE_BLOCK, props);

	//ostringstream sql;
	//sql << "INSERT INTO " << TABLE_BLOCK
	//	<< " (" << CBHOST << ", " << CBGUEST << ", " << CBTIME
	//	<< ") VALUES ("
	//	<< sourceId << ", " << targetId << ", '" << now.str() << "')";
	//DatabaseManager::instance().getDatabase(service.getFbBlock())->doInsert(sql.str());

	DataHandlePtr d = new BlockHandle;
	d->value(new BlockData);
	d->setProperties(props);;
	block->add(d);

	guestInternalManager->addBlocked(targetId, props);

	RelationLogicAdapter::instance().addBlock(sourceId, targetId, current.ctx);

	MCE_DEBUG("addBlock done.");
}

void
RelationManagerI::removeBlock(::Ice::Int sourceId,
						      ::Ice::Int targetId,
						      const Ice::Current& current)
{
	unblock(sourceId, targetId, current);	
}

void
RelationManagerI::unblock(::Ice::Int sourceId,
						      ::Ice::Int targetId,
						      const Ice::Current& current)
{
	if(sourceId == targetId)
	{
		MCE_WARN("sourceId==targetId==" << sourceId << " nothing to say.");
		return;
	};
	RelationServiceI& service = RelationServiceI::instance();

	DataHandleSetMIPtr block = service.getBlockList(sourceId);
	if( ! block->contains(targetId) )
	{
		MCE_DEBUG("remove Blockee. no BLOCK exists. return. sourceId="<<sourceId<<" targetId="<<targetId);
		return;
	}

	ostringstream sql;
	sql << "DELETE FROM " << TABLE_BLOCK
		<< " WHERE (" << CBHOST << "=" << sourceId
		<< " AND " << CBGUEST << "=" << targetId
		<< ")";
	ConnectionQuery(service.getFbBlock(), CDbWServer).doDelete(sql.str());

	block->remove(targetId);
	RelationAdapter::instance().getInternalManager(targetId)->removeBlocked(sourceId, targetId);

	RelationLogicAdapter::instance().removeBlock(sourceId, targetId, current.ctx);
}

::Ice::Int
RelationManagerI::getBlockCount(::Ice::Int sourceId,
							const Ice::Current& current)
{
    return RelationServiceI::instance().getBlockList(sourceId)->size();
}

::MyUtil::Str2StrMap
RelationManagerI::getBlock(::Ice::Int sourceId,
						   ::Ice::Int targetId,
						   const Ice::Current& current)
{
    DataHandlePtr ent = RelationServiceI::instance().getBlockList(targetId)->get(sourceId);
	if(!ent)
	{
		return Str2StrMap();
	}
	return ent->getProperties();
}

::MyUtil::Str2StrMapSeq
RelationManagerI::getBlockList(::Ice::Int sourceId,
						       ::Ice::Int begin,
						       ::Ice::Int limit,
						       const Ice::Current& current)
{
    DataHandleSeq resultAll = RelationServiceI::instance().getBlockList(sourceId)->getAll(begin, limit);
	Str2StrMapSeq result;
	for(DataHandleSeq::iterator it = resultAll.begin();
			it != resultAll.end();
			++it)
	{
		result.push_back((*it)->getProperties());
	};
	return result;
}

MyUtil::RowSetPtr
RelationManagerI::getBlockRowSet(Ice::Int sourceId,
                            Ice::Int begin,
                            Ice::Int limit,
                            const Ice::Current& current)
{

    MCE_DEBUG("=======call getBlockRowSet() =========");

    DataHandleSeq ents = RelationServiceI::instance().getBlockList(sourceId)->getAll(begin, limit);


    if(ents.empty())
    {
        return NULL;
    }

    RowSetPtr res = RowSetHelper::boxing(ents);
    MCE_DEBUG("=======call getBlockRowSet() ========= return size = "<< res->rows.size() );
    return res;
}

BlockDataSeq
RelationManagerI::getBlockSeq(
	Ice::Int fromId,
	Ice::Int begin,
	Ice::Int limit,
	const Ice::Current&)
{
	return BlockDataSeq();
}

::Ice::Int
RelationManagerI::getBlockedCount(::Ice::Int targetId,
							const Ice::Current& current)
{
    return RelationServiceI::instance().getBlockedList(targetId)->size();
}

::MyUtil::Str2StrMapSeq
RelationManagerI::getBlockedList(::Ice::Int targetId,
						       ::Ice::Int begin,
						       ::Ice::Int limit,
						       const Ice::Current& current)
{
    DataHandleSeq resultAll = RelationServiceI::instance().getBlockedList(targetId)->getAll(begin, limit);
	Str2StrMapSeq result;
	for(DataHandleSeq::iterator it = resultAll.begin();
			it != resultAll.end();
			++it)
	{
		result.push_back((*it)->getProperties());
	};
	return result;
}

MyUtil::RowSetPtr
RelationManagerI::getBlockedRowSet(Ice::Int targetId,
                            Ice::Int begin,
                            Ice::Int limit,
                            const Ice::Current& current)
{
    MCE_DEBUG("=======call getBlockedRowSet() ========= ");
    DataHandleSeq ents = RelationServiceI::instance().getBlockedList(targetId)->getAll(begin, limit);

    if(ents.empty())
    {
        return NULL;
    }

    RowSetPtr res = RowSetHelper::boxing(ents);
    MCE_DEBUG("=======call getBlockedRowSet() ========= return size = "<< res->rows.size() );
    return res;
}


void
RelationManagerI::addBuddy(::Ice::Int hostId,
						   ::Ice::Int buddyId,
						   const Ice::Current& current)
{
	// This is an internal only interface.
	// no check needed.
	if(hostId == buddyId)
	{
		MCE_WARN("addBuddy hostId==buddyId==" << hostId << " nothing to say.");
		return;
	};
	RelationServiceI& service = RelationServiceI::instance();
	DataHandleSetMIPtr buddy = service.getBuddyList(hostId);

	MyUtil::Date now(time(NULL));
	//ostringstream sqlHost;
	//sqlHost << "INSERT INTO " << service.getBuddyTable(hostId)
	//	<< " ("<<CRHOST<<","<<CRGUEST<<","<<CRTIME<<")"
	//	<< " VALUES (" << hostId << "," << buddyId << ",'" << now.str() << "')";
	//ostringstream sqlGuest;
	//sqlGuest << "INSERT INTO " << service.getBuddyTable(buddyId)
	//	<< " ("<<CRHOST<<","<<CRGUEST<<","<<CRTIME<<")"
	//	<< " VALUES (" << buddyId << "," << hostId << ",'" << now.str() << "')";

	//DatabaseManager::instance().getDatabase(service.getFbBuddy())->doInsert(sqlHost.str());
	//DatabaseManager::instance().getDatabase(service.getFbBuddy())->doInsert(sqlGuest.str());

	Str2StrMap propHost;
	propHost[CRHOST] = boost::lexical_cast<string>(hostId);
	propHost[CRGUEST] = boost::lexical_cast<string>(buddyId);
	propHost[CRGROUPID] = "0";
	propHost[CRTIME] = now.str();
	ostringstream sqlHost;
	sqlHost << "REPLACE INTO " << service.getBuddyTable(hostId) 
		<< " ("<<CRHOST<<","<<CRGUEST<<","<<CRGROUPID<<","<<CRTIME<<")"
		<< " VALUES ('"<<hostId<<"','"<<buddyId<<"','0','"<<now.str()<<"')";
	MCE_DEBUG(sqlHost.str());

	Str2StrMap propGuest;
	propGuest[CRHOST] = boost::lexical_cast<string>(buddyId);
	propGuest[CRGUEST] = boost::lexical_cast<string>(hostId);
	propGuest[CRGROUPID] = "0";
	propGuest[CRTIME] = now.str();
	ostringstream sqlGuest;
	sqlGuest << "REPLACE INTO " << service.getBuddyTable(buddyId) 
		<< " ("<<CRHOST<<","<<CRGUEST<<","<<CRGROUPID<<","<<CRTIME<<")"
		<< " VALUES ('"<<buddyId<<"','"<<hostId<<"','0','"<<now.str()<<"')";
	MCE_DEBUG(sqlGuest.str());

	ConnectionQuery(service.getFbBuddy(), CDbWServer, service.getBuddyTable(hostId)).doInsert(sqlHost.str());
	ConnectionQuery(service.getFbBuddy(), CDbWServer, service.getBuddyTable(buddyId)).doInsert(sqlGuest.str());

	DataHandlePtr entHost = new BuddyHandle;
	entHost->value(new BuddyData);
	entHost->setProperties(propHost);

	buddy->add(entHost);
	RelationAdapter::instance().getInternalManager(buddyId)->addBuddy(buddyId, propGuest);
}

void
RelationManagerI::removeBuddy(::Ice::Int hostId,
						      ::Ice::Int buddyId,
						      const Ice::Current& current)
{
	if(hostId == buddyId)
	{
		MCE_WARN("hostId==buddyId==" << hostId << " nothing to say.");
		return;
	};
	RelationServiceI& service = RelationServiceI::instance();
	DataHandleSetMIPtr buddy = service.getBuddyList(hostId);

	if( ! buddy->contains(buddyId) )
	{
		MCE_DEBUG("removeBuddy no such buddy. return.");
		return;
	}

	ostringstream sqlHost;
	sqlHost << "DELETE FROM " << service.getBuddyTable(hostId)
			<< " WHERE (" << CRHOST << "=" << hostId
				<< " AND " << CRGUEST << "=" << buddyId
				<< ")";
	ostringstream sqlGuest;
	sqlGuest << "DELETE FROM " << service.getBuddyTable(buddyId)
			<< " WHERE (" << CRHOST << "=" << buddyId
				<< " AND " << CRGUEST << "=" << hostId
				<< ")";

	ConnectionQuery(service.getFbBuddy(), CDbWServer, service.getBuddyTable(hostId)).doDelete(sqlHost.str());
	ConnectionQuery(service.getFbBuddy(), CDbWServer, service.getBuddyTable(buddyId)).doDelete(sqlGuest.str());
			
	buddy->remove(buddyId);
	RelationAdapter::instance().getInternalManager(buddyId)->removeBuddy(buddyId, hostId);

	RelationLogicAdapter::instance().removeBuddy(hostId, buddyId, current.ctx);
}

::Ice::Int
RelationManagerI::getBuddyCount(::Ice::Int hostId,
							const Ice::Current& current)
{
    return RelationServiceI::instance().getBuddyList(hostId)->size();
}

::MyUtil::Str2StrMapSeq
RelationManagerI::getBuddyList(::Ice::Int hostId,
						       ::Ice::Int begin,
						       ::Ice::Int limit,
						       const Ice::Current& current)
{
    DataHandleSeq resultAll = RelationServiceI::instance().getBuddyList(hostId)->getAll(begin, limit);
	Str2StrMapSeq result;
	for(DataHandleSeq::iterator it = resultAll.begin();
			it != resultAll.end();
			++it)
	{
		MCE_DEBUG("getBuddyList begin="<<begin << " limit=" << limit);
		result.push_back((*it)->getProperties());
	};
	return result;
}

MyUtil::RowSetPtr
RelationManagerI::getBuddyRowSet(Ice::Int hostId,
                                Ice::Int begin,
                                Ice::Int limit,
                                const Ice::Current& current)
{
    MCE_DEBUG("=======call getBuddyRowSet() ========= ");
    DataHandleSeq resultAll = RelationServiceI::instance().getBuddyList(hostId)->getAll(begin, limit);
    if(resultAll.empty())
    {
        return NULL;
    }
    RowSetPtr res = RowSetHelper::boxing(resultAll);
    MCE_DEBUG("=======call getBuddyRowSet() ========= return size = "<< res->rows.size() );
   return res;
}


::MyUtil::Str2StrMap
RelationManagerI::getBuddy(::Ice::Int hostId,
						   ::Ice::Int buddyId,
						   const Ice::Current& current)
{
    DataHandlePtr ent = RelationServiceI::instance().getBuddyList(hostId)->get(buddyId);
	if (!ent) {
		return Str2StrMap();
	};
	return ent->getProperties();
}


BuddyDataSeq
RelationManagerI::getBuddySeq(
	Ice::Int hostId,
	Ice::Int begin,
	Ice::Int limit,
	const Ice::Current&)
{
	return BuddyDataSeq();
}


::MyUtil::Str2StrMap
RelationManagerI::getRelationA(::Ice::Int visitorId,
                              ::Ice::Int targetId,
                              const Ice::Current& current)
{
    string strTargetId = boost::lexical_cast<string>(targetId);
    string strVisitorId = boost::lexical_cast<string>(visitorId);

    RelationServiceI& service = RelationServiceI::instance();
    DataHandleSetMIPtr buddy = service.getBuddyList(visitorId);
    DataHandleSetMIPtr blocked = service.getBlockedList(visitorId);
    DataHandleSetEIPtr request = service.getRequestList(visitorId);

    if(request->contains(targetId))
    {
        // the visitor received BuddyRequest from target,
        // so set disc=UU_GUEST_REQUEST
        Str2StrMap result = request->get(targetId)->getProperties();
        result["HOST"]=strVisitorId;
        result["GUEST"]=strTargetId;
        result["DISC"]=boost::lexical_cast<string>(UU_GUEST_REQUEST);
        return result;
    }
    if(buddy->contains(targetId))
    {
        Str2StrMap result = buddy->get(targetId)->getProperties();
        result["HOST"]=strVisitorId;
        result["GUEST"]=strTargetId;
        result["DISC"]=boost::lexical_cast<string>(UU_FRIEND);
        return result;
    }
    if(blocked->contains(targetId))
    {
        // the visitor was blocked by target,
        // so set disc = UU_BLOCKED
        Str2StrMap result = blocked->get(targetId)->getProperties();
        result["HOST"]=strVisitorId;
        result["GUEST"]=strTargetId;
        result["DISC"]=boost::lexical_cast<string>(UU_BLOCKED);
        return result;
    }
    RelationNotExistException ex;
    ex.hostId = targetId;
    ex.guestId = visitorId;
    ex.message = "getRelation no such exception.;";
    throw ex;
}
	
::MyUtil::Str2StrMap
RelationManagerI::getRelation(::Ice::Int targetId,
						      ::Ice::Int visitorId,
						      const Ice::Current& current)
{
	string strTargetId = boost::lexical_cast<string>(targetId);
	string strVisitorId = boost::lexical_cast<string>(visitorId);

	RelationServiceI& service = RelationServiceI::instance();
	DataHandleSetMIPtr buddy = service.getBuddyList(visitorId);
	DataHandleSetMIPtr blocked = service.getBlockedList(visitorId);
	DataHandleSetEIPtr request = service.getRequestList(visitorId);

	if(request->contains(targetId))
	{
		// the visitor received BuddyRequest from target, 
		// so set disc=UU_GUEST_REQUEST
		Str2StrMap result = request->get(targetId)->getProperties();
		result["HOST"]=strTargetId;
		result["GUEST"]=strVisitorId;
		result["DISC"]=boost::lexical_cast<string>(UU_GUEST_REQUEST);
		return result;
	}
	if(buddy->contains(targetId))
	{
		Str2StrMap result = buddy->get(targetId)->getProperties();
		result["HOST"]=strTargetId;
		result["GUEST"]=strVisitorId;
		result["DISC"]=boost::lexical_cast<string>(UU_FRIEND);
		return result;
	}
	if(blocked->contains(targetId))
	{
		// the visitor was blocked by target,
		// so set disc = UU_BLOCKED
		Str2StrMap result = blocked->get(targetId)->getProperties();
		result["HOST"]=strTargetId;
		result["GUEST"]=strVisitorId;
		result["DISC"]=boost::lexical_cast<string>(UU_BLOCKED);
		return result;
	}
	RelationNotExistException ex;
	ex.hostId = targetId;
	ex.guestId = visitorId;
	ex.message = "getRelation no such exception.;";
	throw ex;
}

::Ice::Int
RelationManagerI::getRelationCount(::Ice::Int userId,
							   ::Ice::Int disc,
							   const Ice::Current& current)
{
	int buddyCount = getBuddyCount(userId);
	int blockedCount = getBlockedCount(userId);
	int requestCount = getRequestCount(userId);
	MCE_DEBUG("buddyCount=" << buddyCount << " blockCount=" << blockedCount << " requestCount=" << requestCount);
    switch(disc)
    {
    	case -1:
    	    return buddyCount + blockedCount + blockedCount;
    	case UU_FRIEND:
    	    return buddyCount;
    	case UU_GUEST_REQUEST:
    	    return requestCount;
    	default:
    		return -1;
    }
}


//----------------------------------------------------------------------------
//
// Relation Group
//
//----------------------------------------------------------
MyUtil::Str2StrMap
RelationGroupManagerI::addRelationGroup(::Ice::Int hostId,
								const ::std::string& name,
								const Ice::Current& current)
{
	//MCE_DEBUG("HOST"<<hostId<<"aaaa"<<props[GROUP_COL]);
    RelationServiceI& service = RelationServiceI::instance();

    Str2StrMap props;
    props[CGNAME] = name;
    props[CRHOST] = boost::lexical_cast<string>(hostId);
 	Ice::Int groupId = ConnectionQuery(service.getFbGroup(), CDbWServer).doInsert(TABLE_RELATION_GROUP, props);
	MCE_DEBUG("---relstion group: "<< groupId );
	props[CGGROUP] = boost::lexical_cast<string>(groupId);

	// Sync memory
	if(service.hasObject(GROUPSET, hostId))
	{
		DataHandlePtr ent1 = new GroupHandle;
		ent1->value(new GroupData);
		ent1->setProperties(props);
		ent1->setProperty(CGGROUP, props[CGGROUP]);
		service.getRelationGroupSet(hostId)->add(ent1);
	}
	// Sync MainServer
    MainAdapter::instance().addRelationGroupSet(hostId, groupId, props[CGNAME]);

    return props;
}

void
RelationGroupManagerI::removeRelationGroup(::Ice::Int hostId,
								   ::Ice::Int groupId,
								   const Ice::Current& current)
{
	RelationServiceI& service = RelationServiceI::instance();
	Str2StrMap filter;
    filter[CGHOST] = boost::lexical_cast<string>(hostId);
    filter[CGGROUP] = boost::lexical_cast<string>(groupId);
    ConnectionQuery(service.getFbGroup(), CDbWServer).doDelete(TABLE_RELATION_GROUP, filter);

	//if(service.hasObject(service.getRelationGroupSet(hostId)->ice_getIdentity()))
	if(service.hasObject(GROUPSET, hostId))
    {
		service.getRelationGroupSet(hostId)
			->remove(groupId);
	}
    MainAdapter::instance().removeRelationGroupSet(hostId, groupId);
}

void
RelationGroupManagerI::renameRelationGroup(::Ice::Int hostId,
								   ::Ice::Int groupId,
								   const ::std::string& name,
								   const Ice::Current& current)
{
	RelationServiceI& service = RelationServiceI::instance();
    Str2StrMap filter, props;
    filter[CGHOST] = boost::lexical_cast<string>(hostId);
    filter[CGGROUP] = boost::lexical_cast<string>(groupId);
	props[NAME_COL] = name;
	ConnectionQuery(service.getFbGroup(), CDbWServer).doUpdate(TABLE_RELATION_GROUP, filter, props);

	// sync memory
	//if(service.hasObject(service.getRelationGroupSet(hostId)->ice_getIdentity()))
	if(service.hasObject(GROUPSET, hostId))
    {
		DataHandlePtr group = service.getRelationGroupSet(hostId)->get(groupId);
		if (group) {
			group->setProperties(props);
		}
	}
	// sync MainServer
	MainAdapter::instance().updateRelationGroupSet(hostId, groupId, name);
}

::Ice::Int
RelationGroupManagerI::getRelationGroupCount(::Ice::Int hostId,
								     const Ice::Current& current)
{
    return RelationServiceI::instance()
		.getRelationGroupSet(hostId)->size();
}

::MyUtil::Str2StrMapSeq
RelationGroupManagerI::getRelationGroups(::Ice::Int hostId,
								 ::Ice::Int begin,
								 ::Ice::Int limit,
								 const Ice::Current& current)
{
	MCE_DEBUG("call RelationManagerI::getBuddyGroups");
	Str2StrMapSeq result;
	DataHandleSeq ents = RelationServiceI::instance()
		.getRelationGroupSet(hostId)->getAll(begin, limit);
	MCE_DEBUG("ents.size()="<<ents.size());
	for (size_t i = 0; i < ents.size(); ++i) {
		result.push_back(ents.at(i)->getProperties());
	}
	MCE_DEBUG("finished getBuddyGroups");
	return result;
}

GroupDataSeq
RelationGroupManagerI::getRelationGroupSeq(Ice::Int hostId,
                                    Ice::Int begin,
                                    Ice::Int limit,
                                    const Ice::Current& current)
{
	DataHandleSeq ents = RelationServiceI::instance()
                .getRelationGroupSet(hostId)->getAll(begin, limit);
	GroupDataSeq result;
	for (uint i = 0; i < ents.size(); ++i) {
		result.push_back(ents.at(i)->value<GroupDataPtr>());
	}
	return result;
}

MyUtil::RowSetPtr
RelationGroupManagerI::getRelationGroupRowSet(Ice::Int hostId,
                                    Ice::Int begin,
                                    Ice::Int limit,
                                    const Ice::Current& current)
{
    MCE_DEBUG("=======call getRelationGroupRowSet() =========" );
    DataHandleSeq ents = RelationServiceI::instance()
                .getRelationGroupSet(hostId)->getAll(begin, limit);

    MCE_DEBUG("ents.size()="<<ents.size());
    if(ents.empty())
    {
        return NULL;
    }
    RowSetPtr res = RowSetHelper::boxing(ents);
    MCE_DEBUG("=======call getRelationGroupRowSet() ========= return size = "<< res->rows.size() );
    return res;

}


::MyUtil::Str2StrMap
RelationGroupManagerI::getRelationGroup(::Ice::Int hostId,
								::Ice::Int groupId,
								const Ice::Current& current)
{
    DataHandlePtr res = RelationServiceI::instance().getRelationGroupSet(hostId)
        ->get(groupId);
    if (res) {
    	return res->getProperties();
    }
    return Str2StrMap();
}

void
RelationGroupManagerI::moveBuddy(::Ice::Int hostId,
							 ::Ice::Int guestId,
							 ::Ice::Int groupId,
							 const Ice::Current& current)
{
	RelationServiceI& service = RelationServiceI::instance();

	Str2StrMap filter, props;
    filter[CRHOST] = boost::lexical_cast<string>(hostId);
    filter[CRGUEST] = boost::lexical_cast<string>(guestId);
	props[CRGROUPID] = boost::lexical_cast<string>(groupId);;
	ConnectionQuery(service.getFbBuddy(), CDbWServer, service.getBuddyTable(hostId)).doUpdate(service.getBuddyTable(hostId), filter, props);

	string strGuest = boost::lexical_cast<string>(guestId);
	DataHandlePtr ent = service.getBuddyList(hostId)->get(guestId);
	ent->setProperty(CRGROUPID, boost::lexical_cast<string>(groupId));

	// sync MainServer
	MainAdapter::instance().changeBuddyGroupId(hostId, guestId, groupId);
}

//============================================================================
//
// Internal Manager
//
//============================================================================

void
InternalManagerI::reloadRequest(::Ice::Int guestId,
							const Ice::Current& current)
{
	RelationServiceI::instance().removeObject(
                REQUESTLIST, guestId);
		//RelationServiceI::instance().getRequestList(guestId)->ice_getIdentity()

}

void
InternalManagerI::reloadBlock(::Ice::Int hostId,
						      const Ice::Current& current)
{
	RelationServiceI::instance().removeObject(
				BLOCKLIST, hostId
	);
}

void
InternalManagerI::reloadBlocked(::Ice::Int guestId,
						      const Ice::Current& current)
{
	RelationServiceI::instance().removeObject(
				BLOCKEDLIST, guestId
	);
}

void
InternalManagerI::reloadBuddy(::Ice::Int hostId,
						      const Ice::Current& current)
{
	RelationServiceI::instance().removeObject(
				BUDDYLIST, hostId
	);
}

void
InternalManagerI::reloadRelationGroup(::Ice::Int ownerId,
							      const Ice::Current& current)
{
	RelationServiceI::instance().removeObject(
				GROUPSET, ownerId
	);
}

void
mop::hi::svc::relation::InternalManagerI::addRequest(::Ice::Int guestId,
						     const ::MyUtil::Str2StrMap& props,
						     const Ice::Current& current)
{
	DataHandlePtr ent = new RequestHandle;
	ent->value(new RequestData);
	ent->setProperties(props);
	RelationServiceI::instance().getRequestList(guestId)->add(ent);
}

void
mop::hi::svc::relation::InternalManagerI::addBuddy(::Ice::Int hostId,
						   const ::MyUtil::Str2StrMap& props,
						   const Ice::Current& current)
{
	DataHandlePtr d = new BuddyHandle;
	d->value(new BuddyData);
	d->setProperties(props);
	RelationServiceI::instance().getBuddyList(hostId)->add(d);
}

void
mop::hi::svc::relation::InternalManagerI::addBlock(::Ice::Int hostId,
						   const ::MyUtil::Str2StrMap& props,
						   const Ice::Current& current)
{
	//EntityPtr ent = new EntityI( EntityHelper::instance().getEntitySchema(TABLE_BLOCK) );
	DataHandlePtr d = new BlockHandle;
	d->value(new BlockData);
	d->setProperties(props);
	RelationServiceI::instance().getBlockList(hostId)->add(d);
}

void
mop::hi::svc::relation::InternalManagerI::addBlocked(::Ice::Int guestId,
						     const ::MyUtil::Str2StrMap& props,
						     const Ice::Current& current)
{
	DataHandlePtr d = new BlockedHandle;
	d->value(new BlockedData);
	d->setProperties(props);
	RelationServiceI::instance().getBlockedList(guestId)->add(d);
}

void
mop::hi::svc::relation::InternalManagerI::removeRequest(::Ice::Int hostId,
							::Ice::Int guestId,
							const Ice::Current& current)
{
	RelationServiceI::instance().getRequestList(guestId)->remove(hostId);
}

void
mop::hi::svc::relation::InternalManagerI::removeBuddy(::Ice::Int hostId,
						      ::Ice::Int guestId,
						      const Ice::Current& current)
{
	RelationServiceI::instance().getBuddyList(hostId)->remove(guestId);
}
/*
void
mop::hi::svc::relation::InternalManagerI::removeBlock(::Ice::Int hostId,
						      ::Ice::Int guestId,
						      const Ice::Current& current)
{
	RelationServiceI::instance().getBlockList(hostId)->remove(boost::lexical_cast<string>(guestId));
}
*/

void
mop::hi::svc::relation::InternalManagerI::removeBlock(::Ice::Int hostId,
						      ::Ice::Int guestId,
						      const Ice::Current& current)
{
	RelationServiceI::instance().getBlockList(hostId)->remove(guestId);
}

void
mop::hi::svc::relation::InternalManagerI::removeBlocked(::Ice::Int hostId,
							::Ice::Int guestId,
							const Ice::Current& current)
{
	RelationServiceI::instance().getBlockedList(guestId)->remove(hostId);
}

bool
mop::hi::svc::relation::InternalManagerI::containsRequest(::Ice::Int hostId,
							  ::Ice::Int guestId,
							  const Ice::Current& current)
{
    return RelationServiceI::instance().getRequestList(guestId)->contains(hostId);
}

bool
mop::hi::svc::relation::InternalManagerI::containsBuddy(::Ice::Int hostId,
							::Ice::Int guestId,
							const Ice::Current& current)
{
    return RelationServiceI::instance().getBuddyList(hostId)->contains(guestId);
}
/*
bool
mop::hi::svc::relation::InternalManagerI::containsBlock(::Ice::Int hostId,
							::Ice::Int guestId,
							const Ice::Current& current)
{
    return RelationServiceI::instance().getBlockList(hostId)->contains(boost::lexical_cast<string>(guestId));
}
*/

bool
mop::hi::svc::relation::InternalManagerI::containsBlock(::Ice::Int hostId,
							::Ice::Int guestId,
							const Ice::Current& current)
{
    return RelationServiceI::instance().getBlockList(hostId)->contains(guestId);
}

bool
mop::hi::svc::relation::InternalManagerI::containsBlocked(::Ice::Int hostId,
							  ::Ice::Int guestId,
							  const Ice::Current& current)
{
    return RelationServiceI::instance().getBlockedList(guestId)->contains(hostId);
}
/* vim: set ts=4 softtabstop=4 shiftwidth=4 ff=unix: */

