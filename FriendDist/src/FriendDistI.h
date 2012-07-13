#ifndef __FRIEND_DISTI_H__
#define __FRIEND_DISTI_H__

#include "FriendDist.h"
#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "QueryRunner.h"
#include "TaskManager.h"
#include "ServiceI.h"

namespace mop {
namespace hi {
namespace svc {
namespace model {
using namespace MyUtil;

const static Ice::Int FRIEND_DIST = 0;

const string FRIEND_DIST_MANAGER = "FDM";
const static int TASK_LEVEL_REBUILD = 1;
const static int TASK_LEVEL_REBUILD_DIFF = 2;
const static string TABLE_FRIEND_DIST = "friend_dist";

const static size_t TOPN=10;

class FriendDistFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

typedef map<int, int> Univ2CountMap;
typedef multimap<int, int> Count2UnivMap;
class FriendDist : public Ice::Object, public IceUtil::Mutex {
public:
	FriendDist(Univ2CountMap map, Count2UnivMap reverseMap, int total);
	MyUtil::Int2IntMap getFriendDist();
	FriendDistWithTotalPtr getFriendDistWithTotal();
	Ice::Int getFriendDistByUniv(::Ice::Int);
	Ice::Int getTotalCount();

	// For depracated interface in FriendDistManager.
	FriendCountSeq getFriendDistSeq(int begin, int limit);
private:
	Univ2CountMap _map; // count to networkid
	FriendCountSeq _seq;
	FriendDistWithTotalPtr _top;
};
typedef IceUtil::Handle<FriendDist> FriendDistPtr;

class FriendDistManagerI : virtual public FriendDistManager {
public:
	virtual Int2IntMap getFriendDist(::Ice::Int, const Ice::Current& = Ice::Current());
	virtual FriendDistWithTotalPtr getFriendDistWithTotal(Ice::Int, const Ice::Current& = Ice::Current());
	virtual Ice::Int getFriendDistByUniv(::Ice::Int, ::Ice::Int, const Ice::Current& = Ice::Current());
	virtual void rebuildFriendDist(::Ice::Int userId, const Ice::Current& = Ice::Current());
	virtual Ice::Int getTotalCount(::Ice::Int userId, const Ice::Current& = Ice::Current());
private:
	FriendDistPtr findFriendDist(int userId);
};

class FriendDistRebuildTask : public MyUtil::Task {
public:
	FriendDistRebuildTask(int id);
	virtual void handle();
private:
	int _id;
};

class FriendDistIHandler : public com::xiaonei::xce::ResultHandler {
public:
	FriendDistIHandler(Univ2CountMap& map, int& total, const std::string& univField, const std::string& statusField);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	Univ2CountMap& _map;
	int& _total;

	std::string _univField;
	std::string _statusField;
};

class TotalCountHandler : public com::xiaonei::xce::ResultHandler {
public:
	TotalCountHandler(int& total, const std::string& countField);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	int& _total;
	std::string _countField;
};

class FriendDistServiceSchemaI : public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

}
}
}
}
#endif
