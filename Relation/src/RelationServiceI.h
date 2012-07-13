#include <IceBox/IceBox.h>
#include "ServiceI.h"
#include "Channel.h"
#include "Singleton.h"
#include "Relation.h"
#include "DataSet.h"

#ifndef RELATION2_API
#   define RELATION2_API
#endif

extern "C"
{
	    RELATION2_API ::IceBox::Service*
			    create(::Ice::CommunicatorPtr communicator);
}

namespace mop
{
namespace hi
{
namespace svc
{
namespace relation
{
using namespace MyUtil;

const Ice::Int BUDDYLIST = 0;
const Ice::Int REQUESTLIST = 1;
const Ice::Int BLOCKLIST = 2;
const Ice::Int BLOCKEDLIST = 3;
const Ice::Int GROUPSET = 4;

const string RELATION_MANAGER = "RM";
const string INTERNAL_MANAGER = "IM";
const string GROUP_MANAGER = "GM";

const string TABLE_BUDDY_PREFIX = "relation_";
const int TABLE_BUDDY_COUNT = 100;
const string TABLE_REQUEST = "buddyapply";
const string TABLE_BLOCK = "relation_block";
const string TABLE_RELATION_GROUP = "relation_group";


//const static int GROUP_OTHER = 2;
//const static int GROUP_DEFAULT = 0;
//const static int GROUP_ALL = -1;

const static int UU_BLOCKED = 100;
const static int UU_BLOCK = 90;
const static int UU_REQUEST_GUEST = 4;
const static int UU_GUEST_REQUEST = 1;
const static int UU_FRIEND = 0;

class RELATION2_API RelationServiceI :
	public MyUtil::Singleton<RelationServiceI>
{
public:

	DataHandleSetMIPtr getBuddyList(int guestId);
	DataHandleSetEIPtr getRequestList(int guestId);
	DataHandleSetMIPtr getBlockList(int hostId);
	DataHandleSetMIPtr getBlockedList(int guestId);
	DataHandleSetMIPtr getRelationGroupSet(int ownerId);

	bool hasObject(int category, int id);
	Ice::ObjectPtr findObject(int category, int id);
	void removeObject(int category, int id);
	
	string getBuddyTable(int id);
	string getFbBuddy();
	string getFbRequest();
	string getFbBlock();
	string getFbGroup();

};

class RequestListFactory : public MyUtil::ServantFactory
{
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class BuddyListFactory : public MyUtil::ServantFactory
{
public:
	Ice::ObjectPtr create(Ice::Int id);
};

class BlockListFactory : public MyUtil::ServantFactory
{
public:
	Ice::ObjectPtr create(Ice::Int id);
};

class BlockedListFactory : public MyUtil::ServantFactory
{
public:
	Ice::ObjectPtr create(Ice::Int id);
};

class GroupListFactory : public MyUtil::ServantFactory
{
public:
	Ice::ObjectPtr create(Ice::Int id);
};

};
};
};
};

