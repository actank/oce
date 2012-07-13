#ifndef BUDDYGROUPI_H_
#define BUDDYGROUPI_H_

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include "BuddyGroup.h"
#include "ServiceI.h"
#include <set>
#include <map>
#include <IceUtil/Mutex.h>
#include <Singleton.h>

namespace com {
namespace xiaonei {
namespace model {
using namespace std;
using namespace MyUtil;



class BuddyGroup : public Ice::Object {
public:
	int groupId;
	set<int> buddyIds;
};
typedef IceUtil::Handle<BuddyGroup> BuddyGroupPtr;

typedef map<string,BuddyGroupPtr> BuddyGroupMap;

class GroupMapWrapper : public Ice::Object, public IceUtil::Mutex {
public:
	BuddyGroupMap value;
};
typedef IceUtil::Handle<GroupMapWrapper> GroupMapWrapperPtr;

class BuddyGroupManagerI : public BuddyGroupManager,
	public Singleton<BuddyGroupManagerI> {
public:
	virtual void addGroup(Ice::Int host, const string& groupName,
			const Ice::Current& = Ice::Current());
	virtual void renameGroup(Ice::Int host, const string& oldName,
			const string& newName, const Ice::Current& = Ice::Current());
	virtual void removeGroup(Ice::Int host, const string& groupName,
			const Ice::Current& = Ice::Current());
	virtual void cleanGroup(Ice::Int host, const string& groupName,
			const Ice::Current& = Ice::Current());

	virtual void addBuddyToGroup(Ice::Int host, const string& groupName,
			Ice::Int buddy, const Ice::Current& = Ice::Current());

	virtual void addBuddiesToGroup(Ice::Int host, const string& groupName,
			const MyUtil::IntSeq & buddies, const Ice::Current& = Ice::Current());
	virtual void removeBuddyFromGroup(Ice::Int host, const string& groupName,
			Ice::Int buddy, const Ice::Current& = Ice::Current());
	virtual void removeBuddy(Ice::Int host, Ice::Int buddy,
			const Ice::Current& = Ice::Current());
	virtual int changeBuddyGroups(Ice::Int host, Ice::Int buddy,
			const MyUtil::StrSeq& nameSeq, const Ice::Current& = Ice::Current());
	virtual void changeBuddyGroupsForWeb(Ice::Int host, Ice::Int buddy,
			const StrSeq& nameSeq, const Ice::Current& = Ice::Current());
	//	virtual int buddyChangeGroups(Ice::Int host, Ice::Int buddy,
	//			const MyUtil::StrSeq& nameSeq, const Ice::Current& = Ice::Current());
	virtual BuddiedGroupsMap getBuddiedGroupsMap(Ice::Int host,
			const Ice::Current& = Ice::Current());

	virtual MyUtil::StrSeq getGroupList(Ice::Int host,
			const Ice::Current& = Ice::Current());
	virtual MyUtil::IntSeq getBuddyList(Ice::Int host, const string& groupName,
			Ice::Int begin, Ice::Int limit, const Ice::Current& = Ice::Current());
	virtual Ice::Int getCountByGroup(Ice::Int host, const string& groupName,
			const Ice::Current& = Ice::Current());

	virtual MyUtil::StrSeq getBuddyGroups(Ice::Int host, Ice::Int guest,
			const Ice::Current& = Ice::Current());

	MyUtil::Str2IntMap getAllGroupsBuddyCount(int host,
			const Ice::Current& = Ice::Current());

	static string getTableName(const string & table, int host) {
		ostringstream tableName;
		tableName<<table<<"_"<<(host%100);
		return tableName.str();
	}

	void notifyIm(bool notifyIm) {
		_notifyIm = notifyIm;
	}
protected:
	//	IceUtil::Mutex _changeGroupLock;
	void _removeGroup(Ice::Int host, const string& groupName);
	MyUtil::StrSeq _getBuddyGroups(Ice::Int host, Ice::Int guest);
	bool _notifyIm;

};

class BuddyGroupFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class CleanIllegalDataTask : public MyUtil::Task {
	int _host;
	map<int,string> _groups;
public:
	CleanIllegalDataTask(int host, map<int,string> groups) :
		_host(host) {
		_groups.swap(groups);
	}
	;
	virtual void handle();

};

}
;
}
;
}
;

#endif /*BUDDYGROUPI_H_*/
