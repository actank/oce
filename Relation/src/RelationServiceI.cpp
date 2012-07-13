#include "Relation.h"
#include "RelationServiceI.h"
#include "RelationI.h"
#include "RelationLogicAdapter.h"
#include "TaskManager.h"
#include "RelationAdapter.h"
#include "MainAdapter.h"
#include "RelationCommon.h"
#include "ConnectionQuery.h"
#include "DataSet.h"

using namespace com::xiaonei::xce;
using namespace mop::hi::svc::relation;
using namespace mop::hi::svc::adapter;
using namespace MyUtil;

//---------------------------------------------------------------------------
void
MyUtil::initialize()
{
	ServiceI& service = ServiceI::instance();

	MCE_DEBUG("RelationServiceI initialize.");
	service.registerObjectCache(BUDDYLIST, "BUL", new BuddyListFactory, true);
	service.registerObjectCache(REQUESTLIST, "REL", new RequestListFactory, true);
	service.registerObjectCache(BLOCKLIST, "BLL", new BlockListFactory, true);
	service.registerObjectCache(BLOCKEDLIST, "BEL", new BlockedListFactory, true);
	service.registerObjectCache(GROUPSET, "GRS", new GroupListFactory, true);

	service.getAdapter()->add(new RelationManagerI,
			service.getCommunicator()->stringToIdentity(RELATION_MANAGER));
	service.getAdapter()->add(new InternalManagerI,
			service.getCommunicator()->stringToIdentity(INTERNAL_MANAGER));
	service.getAdapter()->add(new RelationGroupManagerI,
			service.getCommunicator()->stringToIdentity(GROUP_MANAGER));
}

bool
RelationServiceI::hasObject(int category, int id) {
	return ServiceI::instance().hasObject(category, id);
}

Ice::ObjectPtr
RelationServiceI::findObject(int category, int id) {
	return ServiceI::instance().locateObject(category, id);
}

void
RelationServiceI::removeObject(int category, int id) {
	ServiceI::instance().removeObject(category, id);
}

DataHandleSetMIPtr
RelationServiceI::getBuddyList(int guestId)
{
    return DataHandleSetMIPtr::dynamicCast(findObject(BUDDYLIST, guestId));
}

DataHandleSetEIPtr
RelationServiceI::getRequestList(int guestId)
{
    return DataHandleSetEIPtr::dynamicCast(findObject(REQUESTLIST, guestId));
}

DataHandleSetMIPtr
RelationServiceI::getBlockList(int hostId)
{
    return DataHandleSetMIPtr::dynamicCast(findObject(BLOCKLIST, hostId));
}

DataHandleSetMIPtr
RelationServiceI::getBlockedList(int guestId)
{
    return DataHandleSetMIPtr::dynamicCast(findObject(BLOCKEDLIST, guestId));
}

DataHandleSetMIPtr
RelationServiceI::getRelationGroupSet(int hostId)
{
    return DataHandleSetMIPtr::dynamicCast(findObject(GROUPSET, hostId));
}

string
RelationServiceI::getBuddyTable(int id)
{
	ostringstream result;
	result << TABLE_BUDDY_PREFIX << (id % TABLE_BUDDY_COUNT);
	MCE_DEBUG("getRelationTable : " << result.str());
	return result.str();
}

string
RelationServiceI::getFbBuddy()
{
	return "relation";
	//return EntityHelper::instance().getEntitySchema(getBuddyTable(id))->database;
}

string
RelationServiceI::getFbRequest()
{
	return "relation_info";
	//return EntityHelper::instance().getEntitySchema(TABLE_REQUEST)->database;
}

string
RelationServiceI::getFbBlock()
{
	return "relation_info";
	//return EntityHelper::instance().getEntitySchema(TABLE_BLOCK)->database;
}

string
RelationServiceI::getFbGroup()
{
	return "relation_info";
	//return EntityHelper::instance().getEntitySchema(TABLE_RELATION_GROUP)->database;
}
//---------------------------------------------------------------------------
Ice::ObjectPtr
BuddyListFactory::create(Ice::Int id)
{
    MCE_DEBUG( "Loading " << id );
    if(id == 200618204) return 0;
	RelationServiceI& service = RelationServiceI::instance();
	Str2StrMap filter;
	filter[CRHOST]=boost::lexical_cast<string>(id);
	filter["DISC"]="0";
	string buddyTable = service.getBuddyTable(id);
	DataResSetPtr rs = DataResSetPtr::dynamicCast(
		ConnectionQuery(service.getFbBuddy(), CDbRServer, buddyTable).doSelect(
			buddyTable,
			filter,
			new GeneralResSetFactory<DataResSetI<BuddyHandle> >));
	DataHandleSetMIPtr result = new DataHandleSetMI(CRGUEST);
	result->addAll(rs->rows);
    MCE_DEBUG( "Loading " << id << " done.");
    return result;
}

Ice::ObjectPtr
BlockListFactory::create(Ice::Int id)
{
    MCE_DEBUG( "Loading " << id );
	RelationServiceI& service = RelationServiceI::instance();
	Str2StrMap filter;
	filter[CBHOST]=boost::lexical_cast<string>(id);
    DataResSetPtr rs = DataResSetPtr::dynamicCast(
    	ConnectionQuery(service.getFbBlock()).doSelect(
    		TABLE_BLOCK,
    		filter,
    		new GeneralResSetFactory<DataResSetI<BlockHandle> >));
    DataHandleSetMIPtr result = new DataHandleSetMI(CBGUEST);
    result->addAll(rs->rows);
    MCE_DEBUG( "Loading " << id << " done.");
    return result;
}

Ice::ObjectPtr
BlockedListFactory::create(Ice::Int id)
{
    MCE_DEBUG( "Loading " << id );
    RelationServiceI& service = RelationServiceI::instance();
	Str2StrMap filter;
	filter[CBGUEST]=boost::lexical_cast<string>(id);
    DataResSetPtr rs = DataResSetPtr::dynamicCast(
    	ConnectionQuery(service.getFbBlock()).doSelect(
    		TABLE_BLOCK,
    		filter,
    		new GeneralResSetFactory<DataResSetI<BlockedHandle> >));
    DataHandleSetMIPtr result = new DataHandleSetMI(CBHOST);
    result->addAll(rs->rows);
	MCE_DEBUG( "Loading " << id << " done.");
    return result;
}

Ice::ObjectPtr
RequestListFactory::create(Ice::Int id)
{
    MCE_DEBUG( "Loading " << id );
    RelationServiceI& service = RelationServiceI::instance();
	ostringstream sql;
	sql << "select * from " << TABLE_REQUEST 
		<< " where " << CRQACCEPTER << "=" << id
		<< " order by time";
	DataResSetPtr rs = DataResSetPtr::dynamicCast(
    	ConnectionQuery(service.getFbRequest()).doSelect(
    		sql.str(),
    		new GeneralResSetFactory<DataResSetI<RequestHandle> >));
    DataHandleSetEIPtr result = new DataHandleSetEI(CRQAPPLICANT);
    result->addAll(rs->rows);
	MCE_DEBUG( "Loading " << id << " done.");
    return result;
}

Ice::ObjectPtr
GroupListFactory::create(Ice::Int id)
{
    MCE_DEBUG( "Loading " << id );
    RelationServiceI& service = RelationServiceI::instance();
	Str2StrMap filter;
	filter[HOST_COL]=boost::lexical_cast<string>(id);
    DataResSetPtr rs = DataResSetPtr::dynamicCast(
    	ConnectionQuery(service.getFbGroup()).doSelect(
    		TABLE_RELATION_GROUP,
    		filter,
    		new GeneralResSetFactory<DataResSetI<GroupHandle> >));
    DataHandleSetMIPtr result = new DataHandleSetMI(GROUP_COL);
    result->addAll(rs->rows);
    MCE_DEBUG( "Loading " << id << " done.");
    return result;
}

/* vim: set ts=4 shiftwidth=4 softtabstop=4 : */

