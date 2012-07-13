#include "BuddyGroupI.h"
#include "QueryRunner.h"
//#include "RelationAdapter.h"
#include "BuddyCoreAdapter.h"
//#include "TalkIqAdapter.h"
#include "IqMsgHandlerAdapter.h"
#include <boost/lexical_cast.hpp>
using namespace com::xiaonei::model;
using namespace com::xiaonei::xce;
using namespace talk::adapter;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::service::buddy;

bool isUtf8(const string& str) {
	MCE_DEBUG("check:"<<str);
	int c = 0;
	for (size_t i = str.length() - 1; i >=0; --i) {
		unsigned char ch = (unsigned char)str[i];
		//		MCE_DEBUG("ch:"<<(int)ch);
		//		MCE_DEBUG("& = "<< (int)(ch & (unsigned char) 0x80) << "  0x80:"<<0x80);
		if ((!(((int)ch & 0x80) == 0x80)) && (c==0)) {
			//			MCE_DEBUG("return:"<< (int)(((int)ch & 0x80) == 0x80));
			return true;
		}
		//		MCE_DEBUG("ch:"<<(int)ch << " 0xc0:"<<0xc0<< "  &:"<<(int)((int)ch & (unsigned char) 0xc0) << "  0x80:"<<0x80);
		if (((int)ch & 0xc0) == 0x80) {
			c ++;
			//			MCE_DEBUG("C="<<c);
		} else {
			switch (c) {
			case 1:
				//				MCE_DEBUG("case 1");
				if (((int)ch & 0xe0) == 0xc0) {
					return true;
				} else {
					return false;
				}
			case 2:
				//				MCE_DEBUG("case 2");
				if (((int)ch & 0xf0) == 0xe0) {
					return true;
				} else {
					return false;
				}
			case 3:
				//				MCE_DEBUG("case 3");
				if (((int)ch & 0xf8) == 0xf0) {
					return true;
				} else {
					return false;
				}
			default:
				//				MCE_DEBUG("return false")
				return false;
			}
		}
	}
}

void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(0, "BG", new BuddyGroupFactoryI);
	service.getAdapter()->add(&BuddyGroupManagerI::instance(), service.createIdentity("M",
			""));
	//BuddyCoreAdapter::instance().initializeWith(service.getAdapter());
	bool notifyIm = (bool) service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Service."+ServiceI::instance().getName()+".NotifyIM", 1);
	BuddyGroupManagerI::instance().notifyIm(notifyIm);
}

Ice::ObjectPtr BuddyGroupFactoryI::create(Ice::Int id) {
	MCE_DEBUG("Loading "<<id);

	GroupMapWrapperPtr res = new GroupMapWrapper();

	QueryRunner conn("buddygroup", CDbRServer,
			BuddyGroupManagerI::getTableName("relation_group", id));
	Statement sql;
	sql
			<< "select a.groupid gid, a.name gname, if(b.guest is null, 0, b.guest) guest"
			<< " from "<<BuddyGroupManagerI::getTableName("relation_group", id)
			<<" a left join "<<BuddyGroupManagerI::getTableName(
			"relation_group_item", id)<<" b "
			<< "on (a.host=b.host and a.groupid = b.groupid) where a.host ="
			<< boost::lexical_cast<string>(id);
	//	DefResSetPtr drs = DefResSetPtr::dynamicCast(conn.doSelect(sql.str()));
	//	mysqlpp::StoreQueryResult res = conn.store(Statement(sql.str()));

#ifndef NEWARCH
	mysqlpp::Result dataRes = conn.store(Statement(sql.str()));
#else
	mysqlpp::StoreQueryResult dataRes = conn.store(sql);
#endif
	map<int,string> illegalGroups;

	if (dataRes) {

#ifndef NEWARCH
		for (size_t i = 0; i<dataRes.size(); ++i)
#else
			for (size_t i = 0; i<dataRes.num_rows(); ++i) {
#endif
			mysqlpp::Row row = dataRes.at(i);
		int groupId = (int)row["gid"];
#ifndef NEWARCH

		string groupName = row["gname"].get_string();
#else

		string groupName = row["gname"].c_str();
#endif

		int guest = (int)row["guest"];

		if (!isUtf8(groupName)) {
			illegalGroups[groupId] = groupName;
			continue;
		}

		BuddyGroupMap::iterator it = res->value.find(groupName);
		if (it == res->value.end()) {
			BuddyGroupPtr ptr = new BuddyGroup();
			ptr->groupId = groupId;
			if (guest)
				ptr->buddyIds.insert(guest);
			res->value[groupName] = ptr;
		} else {
			if (guest)
				it->second->buddyIds.insert(guest);
		}
	}
}

if (!illegalGroups.empty()) {
	TaskManager::instance().execute(new CleanIllegalDataTask(id,illegalGroups));
}
return res;

}

void BuddyGroupManagerI::addGroup(Ice::Int host, const string& groupName,
	const Ice::Current&) {

if (!isUtf8(groupName)) {
	MCE_WARN("BuddyGroupManagerI::addGroup --> Illegal GroupName:"
			<<groupName<<"  host:"<<host);
	return;
}

//	QueryRunner conn("buddygroup", CDbWServer,
//			BuddyGroupManagerI::getTableName("relation_group", host));
int groupId = 0;
{
	ConnectionHolder conn("buddygroup", CDbWServer,
			BuddyGroupManagerI::getTableName("relation_group", host));
	mysqlpp::Query query = conn.query();
	//	ostringstream sql;
	query<<"insert into "<<BuddyGroupManagerI::getTableName("relation_group",
			host)<<" (host, name) values ("<<host<<","<<mysqlpp::quote
	<<groupName<<")";
	//	int groupId = conn.execute(Statement(sql.str())).insert_id();
	groupId = query.execute().insert_id();
}
if (ServiceI::instance().hasObject(0, host)) {
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (wptr) {
		//GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

		IceUtil::Mutex::Lock lock(*wptr);

		BuddyGroupPtr gptr = new BuddyGroup();
		gptr->groupId = groupId;
		wptr->value[groupName] = gptr;
	}
}

}

void BuddyGroupManagerI::renameGroup(Ice::Int host, const string& oldName,
		const string& newName, const Ice::Current&) {

	//if newName exist
	if (!(isUtf8(oldName) && isUtf8(newName))) {
		MCE_WARN("BuddyGroupManagerI::renameGroup --> Illegal OldGroupName:"
				<<oldName<<"  NewGroupName:"<<newName<<"  host:"<<host);
		return;
	}

	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return;
	}

	IceUtil::Mutex::Lock lock(*wptr);

	BuddyGroupMap::iterator nit = wptr->value.find(newName);

	if (nit != wptr->value.end()) {

		BuddyGroupMap::iterator oit = wptr->value.find(oldName);
		MCE_DEBUG(__LINE__ << "  -->");
		if (oit != wptr->value.end()) {

			for (set<int>::iterator bit = oit->second->buddyIds.begin(); bit
					!= oit->second->buddyIds.end(); ++bit) {
				nit->second->buddyIds.insert(*bit);
			}

			oit->second->buddyIds.clear();

			_removeGroup(host, oldName);

		}
	} else {

		//		QueryRunner conn("buddygroup", CDbWServer,
		//				BuddyGroupManagerI::getTableName("relation_group", host));
		{
			ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();

		//		ostringstream sql;
		query<<"update " <<BuddyGroupManagerI::getTableName("relation_group",
				host) <<" set name="<<mysqlpp::quote<<newName<<" where host="
				<<host <<" and name="<<mysqlpp::quote<<oldName;
		//		conn.execute(sql.str());
		query.execute();
		}

		//if (ServiceI::instance().hasObject(0, host)) {
		//GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
		if (wptr) {
			//GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
			BuddyGroupMap::iterator it = wptr->value.find(oldName);

			if (it != wptr->value.end()) {
				wptr->value[newName] = it->second;
				wptr->value.erase(it);
			}

		}
		//}
	}
	//notify im
	//GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);

	if (_notifyIm) {
		BuddyItemPtr bi = new BuddyItem();
		bi->host = host;

		if (wptr) {
			BuddyGroupMap::iterator it = wptr->value.find(newName);
			if (it != wptr->value.end()) {
				for (set<int>::iterator bit = it->second->buddyIds.begin(); bit
						!= it->second->buddyIds.end(); ++bit) {
					bi->items[*bit] = _getBuddyGroups(host, *bit);
				}
			}
		}
		if (bi->items.empty()) {
			return;
		}
		MCE_DEBUG("BuddyGroupManagerI::renameGroup 3-->"<< bi->items.size());
		IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
	}
}
void BuddyGroupManagerI::removeGroup(Ice::Int host, const string& groupName,
		const Ice::Current&) {
	MCE_DEBUG("BuddyGroupManagerI::removeGroup");

	if (!isUtf8(groupName)) {
		MCE_WARN("BuddyGroupManagerI::removeGroup --> Illegal GroupName:"
				<<groupName<<"  host:"<<host);
		return;
	}

	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return;
	}
	IceUtil::Mutex::Lock lock(*wptr);

	int groupId = -1;
	set<int> buddyIds;

	BuddyGroupMap::iterator it;
	if (wptr) {
		//	GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


		it = wptr->value.find(groupName);

		if (it != wptr->value.end()) {
			buddyIds = it->second->buddyIds;
			if (!it->second->buddyIds.empty()) {
				groupId = it->second->groupId;
			}
		}
	}
	//	QueryRunner conn("buddygroup", CDbWServer,
	//			BuddyGroupManagerI::getTableName("relation_group", host));
	//	ostringstream sql;
	{
	ConnectionHolder conn("buddygroup", CDbWServer,
			BuddyGroupManagerI::getTableName("relation_group", host));
	mysqlpp::Query query = conn.query();

	query<<"delete from "<<BuddyGroupManagerI::getTableName("relation_group",
			host)<<"  where host="<<host <<" and name="<<mysqlpp::quote
			<<groupName;
	query.execute();
	}
	if (it != wptr->value.end()) {

		wptr->value.erase(it);
	}

	MCE_DEBUG("BuddyGroupManagerI::removeGroup1 -->"<< buddyIds.size());

	if (groupId == -1) {
		return;
	}

	MCE_DEBUG("BuddyGroupManagerI::removeGroup2 -->"<< buddyIds.size());
	{
	ConnectionHolder conn1("buddygroup", CDbWServer,
			BuddyGroupManagerI::getTableName("relation_group", host));
	mysqlpp::Query query = conn1.query();
	//	sql.str("");
	query<<"delete from "<<BuddyGroupManagerI::getTableName(
			"relation_group_item", host)<<" where groupid="<<groupId
			<< " and host="<< host;
	query.execute();
	}
	//notify im
	if (_notifyIm) {
		if (buddyIds.empty()) {
			return;
		}
		BuddyItemPtr bi = new BuddyItem();
		bi->host = host;
		if (wptr) {
			for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
				bi->items[*it] = _getBuddyGroups(host, *it);
			}
		}
		if (bi->items.empty()) {
			return;
		}

		MCE_DEBUG("BuddyGroupManagerI::removeGroup 3-->"<< buddyIds.size());
		IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
	}
	//---------
}

void BuddyGroupManagerI::_removeGroup(Ice::Int host, const string& groupName) {
	MCE_DEBUG("BuddyGroupManagerI::_removeGroup");
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return;
	}

	int groupId = -1;
	set<int> buddyIds;

	BuddyGroupMap::iterator it;
	if (wptr) {
		//	GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


		it = wptr->value.find(groupName);

		if (it != wptr->value.end()) {
			buddyIds = it->second->buddyIds;
			if (!it->second->buddyIds.empty()) {
				groupId = it->second->groupId;
			}
		}
	}
	//	QueryRunner conn("buddygroup", CDbWServer,
	//			BuddyGroupManagerI::getTableName("relation_group", host));
	//	ostringstream sql;
	{
	ConnectionHolder conn("buddygroup", CDbWServer,
			BuddyGroupManagerI::getTableName("relation_group", host));
	mysqlpp::Query query = conn.query();
	query<<"delete from "<<BuddyGroupManagerI::getTableName("relation_group",
			host)<<"  where host="<<host <<" and name="<<mysqlpp::quote
			<<groupName;
	//	conn.execute(sql.str());
	query.execute();
	}
	if (it != wptr->value.end()) {

		wptr->value.erase(it);
	}

	MCE_DEBUG("BuddyGroupManagerI::_removeGroup1 -->"<< buddyIds.size());

	if (groupId == -1) {
		return;
	}

	MCE_DEBUG("BuddyGroupManagerI::_removeGroup2 -->"<< buddyIds.size());

	//	sql.str("");
	//	sql<<"delete from "<<BuddyGroupManagerI::getTableName(
	//			"relation_group_item", host)<<" where groupid="<<groupId
	//			<< " and host="<< host;
	//	conn.execute(sql.str());
	{
	ConnectionHolder conn1("buddygroup", CDbWServer,
			BuddyGroupManagerI::getTableName("relation_group", host));
	mysqlpp::Query query = conn1.query();
	query <<"delete from "<<BuddyGroupManagerI::getTableName(
			"relation_group_item", host)<<" where groupid="<<groupId
			<< " and host="<< host;
	query.execute();
	}
	//notify im
	if (_notifyIm) {
		if (buddyIds.empty()) {
			return;
		}
		BuddyItemPtr bi = new BuddyItem();
		bi->host = host;
		if (wptr) {
			for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
				bi->items[*it] = _getBuddyGroups(host, *it);
			}
		}
		if (bi->items.empty()) {
			return;
		}

		MCE_DEBUG("BuddyGroupManagerI::_removeGroup 3-->"<< buddyIds.size());
		IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
	}
	//---------
}

void BuddyGroupManagerI::cleanGroup(Ice::Int host, const string& groupName,
		const Ice::Current&) {

	if (!isUtf8(groupName)) {
		MCE_WARN("BuddyGroupManagerI::cleanGroup --> Illegal GroupName:"
				<<groupName<<"  host:"<<host);
		return;
	}

	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return;
	}
	IceUtil::Mutex::Lock lock(*wptr);

	int groupId = -1;
	set<int> buddyIds;

	BuddyGroupMap::iterator it;
	if (wptr) {
		it = wptr->value.find(groupName);
		if (it != wptr->value.end()) {
			if (it->second->buddyIds.empty()) {
				//groupId = it->second->groupId;
				return;
			}
			buddyIds = it->second->buddyIds;
			groupId = it->second->groupId;
			it->second->buddyIds.clear();
		}
	}
	if (groupId >= 0) {
		//		QueryRunner conn("buddygroup", CDbWServer,
		//				BuddyGroupManagerI::getTableName("relation_group", host));
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		//		ostringstream sql;
		query<<"delete from "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host)<<" where groupid="<<groupId
				<<" and host="<<host;
		query.execute();
	}

	//notify im
	if (_notifyIm) {
		if (buddyIds.empty()) {
			return;
		}
		BuddyItemPtr bi = new BuddyItem();
		bi->host = host;
		if (wptr) {
			for (set<int>::iterator it = buddyIds.begin(); it != buddyIds.end(); ++it) {
				bi->items[*it] = _getBuddyGroups(host, *it);
			}
		}
		if (bi->items.empty()) {
			return;
		}
		IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
	}
	//---------

}

void BuddyGroupManagerI::addBuddyToGroup(Ice::Int host,
		const string& groupName, Ice::Int buddy, const Ice::Current&) {

	if (!isUtf8(groupName)) {
		MCE_WARN("BuddyGroupManagerI::addBuddyToGroup --> Illegal GroupName:"
				<<groupName<<"  host:"<<host);
		return;
	}

	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return;
	}
	IceUtil::Mutex::Lock lock(*wptr);

	//GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


	BuddyGroupMap::iterator it = wptr->value.find(groupName);
	if (it != wptr->value.end()) {
		if (it->second->buddyIds.find(buddy) != it->second->buddyIds.end()) {
			return;
		}
		it->second->buddyIds.insert(buddy);
		//		QueryRunner conn("buddygroup", CDbWServer,
		//				BuddyGroupManagerI::getTableName("relation_group", host));
		//		ostringstream sql;
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query <<"insert into  "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host) <<" (host,guest,groupid) values ("
				<<host<<","<<buddy<<"," <<it->second->groupId<<")";
		query.execute();

	} else {

		//		QueryRunner conn("buddygroup", CDbWServer,
		//				BuddyGroupManagerI::getTableName("relation_group", host));
		//		ostringstream sql;
		int groupId =0;
		{
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();

		query << "insert into "<<BuddyGroupManagerI::getTableName(
				"relation_group", host)<<" (host,name) values ("<< host <<","
				<<mysqlpp::quote <<groupName<<")";
		groupId = query.execute().insert_id();
		}
		//		sql.str("");
		{
		ConnectionHolder conn1("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn1.query();
		query << "insert into  "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host) <<" (host,guest,groupid) values ("
				<<host<<","<<buddy<<"," <<groupId<<")";
		query.execute();
		}
		//yuanfei
		BuddyGroupPtr bgPtr = new BuddyGroup();
		bgPtr->groupId = groupId;
		bgPtr->buddyIds.insert(buddy);
		wptr->value[groupName] = bgPtr;
		//
	}

	//notify im
	if (_notifyIm) {
		BuddyItemPtr bi = new BuddyItem();
		bi->host = host;
		bi->items[buddy] = _getBuddyGroups(host, buddy);

		IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
	}
	//---------
}

void BuddyGroupManagerI::addBuddiesToGroup(Ice::Int host,
		const string& groupName, const MyUtil::IntSeq & buddies,
		const Ice::Current&) {

	if (!isUtf8(groupName)) {
		MCE_WARN("BuddyGroupManagerI::addBuddiesToGroup --> Illegal GroupName:"
				<<groupName<<"  host:"<<host);
		return;
	}

	if (buddies.empty()) {
		return;
	}
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return;
	}
	IceUtil::Mutex::Lock lock(*wptr);

	//GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


	BuddyGroupMap::iterator it = wptr->value.find(groupName);
	if (it != wptr->value.end()) {
		MyUtil::IntSeq insertIds;
		for (size_t i = 0; i < buddies.size(); ++i) {
			//it->second->buddyIds.insert(buddies.at(i));
			if (it->second->buddyIds.find(buddies.at(i))
					!= it->second->buddyIds.end()) {
				continue;
			}
			insertIds.push_back(buddies.at(i));
			it->second->buddyIds.insert(buddies.at(i));
		}
		//		QueryRunner conn("buddygroup", CDbWServer,
		//				BuddyGroupManagerI::getTableName("relation_group", host));
		//		ostringstream sql;
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query <<"insert into  "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host) <<" (host,guest,groupid) values ";
		for (size_t i = 0; i < insertIds.size(); ++i) {
			if (i) {
				query <<",";
			}
			query <<"("<<host<<","<<insertIds.at(i)<<","<<it->second->groupId
					<<")";

		}
		query.execute();

	} else {

		//		QueryRunner conn("buddygroup", CDbWServer,
		//				BuddyGroupManagerI::getTableName("relation_group", host));
		//		ostringstream sql;
		int groupId = 0;
		{
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query << "insert into "<<BuddyGroupManagerI::getTableName(
				"relation_group", host)<<" (host,name) values ("<< host <<","
				<<mysqlpp::quote <<groupName<<")";
		groupId = query.execute().insert_id();
		}
		//		sql.str("");
		{
		ConnectionHolder conn1("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn1.query();
		query << "insert into  "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host) <<" (host,guest,groupid) values ";
		for (size_t i = 0; i < buddies.size(); ++i) {
			if (i) {
				query <<",";
			}

			query <<"("<<host<<","<<buddies.at(i)<<","<<groupId<<")";
		};
		query.execute();
		}
		//yuanfei
		BuddyGroupPtr bgPtr = new BuddyGroup();
		bgPtr->groupId = groupId;
		for (size_t i = 0; i < buddies.size(); ++i) {
			bgPtr->buddyIds.insert(buddies.at(i));
		}
		wptr->value[groupName] = bgPtr;
		//
	}

	//notify im
	if (_notifyIm) {
		if (buddies.empty()) {
			return;
		}
		BuddyItemPtr bi = new BuddyItem();
		bi->host = host;

		for (size_t i = 0; i < buddies.size(); ++i) {
			bi->items[buddies.at(i)] = _getBuddyGroups(host, buddies.at(i));
		}

		IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
	}
	//---------
}

void BuddyGroupManagerI::removeBuddyFromGroup(Ice::Int host,
		const string& groupName, Ice::Int buddy, const Ice::Current&) {

	if (!isUtf8(groupName)) {
		MCE_WARN("BuddyGroupManagerI::removeBuddyFromGroup --> Illegal GroupName:"
				<<groupName<<"  host:"<<host);
		return;
	}

	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return;
	}
	IceUtil::Mutex::Lock lock(*wptr);

	//GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);


	BuddyGroupMap::iterator it = wptr->value.find(groupName);
	if (it != wptr->value.end()) {
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query << "delete from "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host)<<" where host = " << host
				<< " and guest = "<<buddy <<" and  groupid = "
				<< it->second->groupId;
		query.execute();
		it->second->buddyIds.erase(buddy);
	}

	//notify im
	if (_notifyIm) {
		BuddyItemPtr bi = new BuddyItem();
		bi->host = host;
		bi->items[buddy] = _getBuddyGroups(host, buddy);

		IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
	}
	//---------
}

void BuddyGroupManagerI::removeBuddy(Ice::Int host, Ice::Int buddy,
		const Ice::Current&) {
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (wptr) {
		IceUtil::Mutex::Lock lock(*wptr);
		for (BuddyGroupMap::iterator it = wptr->value.begin(); it
				!= wptr->value.end(); ++it) {
			ConnectionHolder conn("buddygroup", CDbWServer,
					BuddyGroupManagerI::getTableName("relation_group", host));
			mysqlpp::Query query = conn.query();
			query << "delete from "<<BuddyGroupManagerI::getTableName(
					"relation_group_item", host)<<" where host = " << host
					<< " and guest = "<<buddy << " and groupid = "
					<< it->second->groupId;
			query.execute();
			it->second->buddyIds.erase(buddy);
		}
	}

	//notify im

	//	BuddyItemPtr bi = new BuddyItem();
	//	bi->host = host;
	//	bi->items[buddy] = StrSeq();
	//
	//	IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
	//---------
}

void BuddyGroupManagerI::changeBuddyGroupsForWeb(Ice::Int host, Ice::Int buddy,
		const StrSeq& nameSeq, const Ice::Current& current) {
	//changeBuddyGroups(host, buddy, nameSeq, current);
	for (size_t i = 0; i < nameSeq.size(); ++i) {
		if (!isUtf8(nameSeq.at(i))) {

			MCE_WARN("BuddyGroupManagerI::changeBuddyGroups --> Illegal GroupName:"
					<<nameSeq.at(i)<<"  host:"<<host);
			return;
		}
	}

	//	IceUtil::Mutex::Lock lock(_changeGroupLock);
	MCE_DEBUG("call BuddyGroupManagerI::changeBuddyGroups  ->  host:" <<host
			<<"   buddy:"<<buddy);

	Relationship r;
	r.from = host;
	r.to = buddy;

/*	BuddyDesc desc = BuddyCoreAdapter::instance().getRelation(r);

	MCE_DEBUG("desc:"<<desc);
	if (desc != Friend) {
		MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroups --> host:"<<host
				<< "   desc:"<<desc<<"  not firend");
		return desc;
	}
*/
	set<string> nameSet;
	for (size_t i = 0; i<nameSeq.size(); ++i) {
		nameSet.insert(nameSeq.at(i));
	}
	MyUtil::IntSeq addToGroupItem;
	MyUtil::IntSeq deleteGroupId;
	MyUtil::StrSeq addGroup;

	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroups --> host:"<<host
				<<" locate object err and desc = -1");
		return;
	}

	IceUtil::Mutex::Lock lock(*wptr);
	//GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

	set<string>::iterator it1 = nameSet.begin();
	BuddyGroupMap::iterator it2 = wptr->value.begin();

	vector<string> removeGroup;

	while ((it1 != nameSet.end()) && (it2 != wptr->value.end())) {
		if (*it1 < it2->first) {
			MCE_DEBUG("it1:"<<*it1<<"    it2:"<<it2->first);
			addGroup.push_back(*it1);
			it1++;
		} else if (*it1 > it2->first) {
			if (it2->second->buddyIds.find(buddy)
					!= it2->second->buddyIds.end()) {
				deleteGroupId.push_back(it2->second->groupId);
				it2->second->buddyIds.erase(buddy);
				if (it2->second->buddyIds.empty()) {
					removeGroup.push_back(it2->first);
				}
			}
			it2++;
		} else {
			if (it2->second->buddyIds.insert(buddy).second) {
				addToGroupItem.push_back(it2->second->groupId);
			}
			it1++;
			it2++;
		}
	}
	while (it1 != nameSet.end()) {
		if (wptr->value.find(*it1) == wptr->value.end()) {
			addGroup.push_back(*it1);
		}
		it1++;
	}
	while (it2 != wptr->value.end()) {
		if (it2->second->buddyIds.find(buddy) != it2->second->buddyIds.end()) {
			deleteGroupId.push_back(it2->second->groupId);
			it2->second->buddyIds.erase(buddy);
			if (it2->second->buddyIds.empty()) {
				removeGroup.push_back(it2->first);
			}
		}
		it2++;
	}

	MCE_DEBUG("host: "<<host<<"  addToGroupItem.size:"<<addToGroupItem.size()
			<<"    deleteGroupId.size:"<<deleteGroupId.size()
			<<"   addGroup.size:"<<addGroup.size());
	if (addToGroupItem.empty() && deleteGroupId.empty() && addGroup.empty()) {
		MCE_DEBUG("host:"<<host<<"   return -1");
		return;
	}

	//	QueryRunner conn("buddygroup", CDbWServer,
	//			BuddyGroupManagerI::getTableName("relation_group", host));

	if (!addToGroupItem.empty()) {
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		//		ostringstream sql;
		query <<"insert into  "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host)<<" (host,guest,groupid) values ";
		for (size_t i = 0; i < addToGroupItem.size(); ++i) {
			if (i) {
				query<<",";
			}
			query<<"("<<host<<","<<buddy<<"," <<addToGroupItem.at(i)<<")";
		}
		query.execute();
	}

	if (!deleteGroupId.empty()) {
		//QueryRunner conn("buddygroup", CDbWServer);

		//		ostringstream sql;
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query << "delete from "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host)<<" where host = " << host
				<< " and guest = "<<buddy <<" and  groupid in (";
		for (size_t i = 0; i < deleteGroupId.size(); ++i) {
			if (i) {
				query<<",";
			}
			query<<deleteGroupId.at(i);
		}
		query << ")";
		query.execute();
	}

	if (!addGroup.empty()) {
		//QueryRunner conn("buddygroup", CDbWServer);
		//		ostringstream sql;
		int groupId = -1;
		{
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query<<"insert into "<<BuddyGroupManagerI::getTableName(
				"relation_group", host)<<" (host,name) values ";

		for (size_t i = 0; i < addGroup.size(); ++i) {
			if (i) {
				query<<",";
			}
			query<<"("<<host<<","<<mysqlpp::quote<<addGroup.at(i)<<")";
		}
		groupId = query.execute().insert_id();
		}
		if (groupId != -1) {
			{
			ConnectionHolder conn("buddygroup", CDbWServer,
					BuddyGroupManagerI::getTableName("relation_group", host));
			mysqlpp::Query query = conn.query();
			//			sql.str("");
			query <<"insert into "<<BuddyGroupManagerI::getTableName(
					"relation_group_item", host)
					<<" (host,guest,groupid) values ";
			for (size_t i = 0; i < addGroup.size(); ++i) {
				if (i) {
					query<<",";
				}
				query<<"("<<host<<","<<buddy<<","<<groupId+i<<")";
			}
			query.execute();
			}
			//------------------------------------
			for (size_t i = 0; i < addGroup.size(); ++i) {
				BuddyGroupPtr bgPtr = new BuddyGroup();
				bgPtr->groupId = groupId + i;
				bgPtr->buddyIds.insert(buddy);
				wptr->value[addGroup.at(i)] = bgPtr;
			}
		}
	}

	//delete empty group
	if (!removeGroup.empty()) {
		//		ostringstream sql;
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query<<"delete from "<<BuddyGroupManagerI::getTableName(
				"relation_group", host)<<"  where host="<<host
				<<" and name in (";

		for (size_t i = 0; i < removeGroup.size(); ++i) {
			BuddyGroupMap::iterator it = wptr->value.find(removeGroup.at(i));
			if (it != wptr->value.end()) {
				wptr->value.erase(it);
			}
			if (i) {
				query <<",";
			}
			query <<mysqlpp::quote<< removeGroup.at(i);
		}
		query << ")";
		//		QueryRunner conn("buddygroup", CDbWServer,
		//				BuddyGroupManagerI::getTableName("relation_group", host));
		query.execute();
	}
	//notify im
	if (_notifyIm) {
		if (current.ctx.find("FROM") != current.ctx.end()) {
			BuddyItemPtr bi = new BuddyItem();
			bi->host = host;
			bi->items[buddy] = nameSeq;

			IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
		}
	}
	//---------
	//MCE_DEBUG("host:"<<host<<"  return desc:"<<desc);
}

int BuddyGroupManagerI::changeBuddyGroups(Ice::Int host, Ice::Int buddy,
		const MyUtil::StrSeq& nameSeq, const Ice::Current& current) {

	//delete illegal groups

	for (size_t i = 0; i < nameSeq.size(); ++i) {
		if (!isUtf8(nameSeq.at(i))) {

			MCE_WARN("BuddyGroupManagerI::changeBuddyGroups --> Illegal GroupName:"
					<<nameSeq.at(i)<<"  host:"<<host);
			return -1;
		}
	}

	//	IceUtil::Mutex::Lock lock(_changeGroupLock);
	MCE_DEBUG("call BuddyGroupManagerI::changeBuddyGroups  ->  host:" <<host
			<<"   buddy:"<<buddy);

	Relationship r;
	r.from = host;
	r.to = buddy;

	BuddyDesc desc = BuddyCoreAdapter::instance().getRelation(r);

	MCE_DEBUG("desc:"<<desc);
	if (desc != Friend) {
		MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroups --> host:"<<host
				<< "   desc:"<<desc<<"  not firend");
		return desc;
	}

	set<string> nameSet;
	for (size_t i = 0; i<nameSeq.size(); ++i) {
		nameSet.insert(nameSeq.at(i));
	}
	MyUtil::IntSeq addToGroupItem;
	MyUtil::IntSeq deleteGroupId;
	MyUtil::StrSeq addGroup;

	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		MCE_DEBUG("BuddyGroupManagerI::changeBuddyGroups --> host:"<<host
				<<" locate object err and desc = -1");
		return -1;
	}

	IceUtil::Mutex::Lock lock(*wptr);
	//GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);

	set<string>::iterator it1 = nameSet.begin();
	BuddyGroupMap::iterator it2 = wptr->value.begin();

	vector<string> removeGroup;

	while ((it1 != nameSet.end()) && (it2 != wptr->value.end())) {
		if (*it1 < it2->first) {
			MCE_DEBUG("it1:"<<*it1<<"    it2:"<<it2->first);
			addGroup.push_back(*it1);
			it1++;
		} else if (*it1 > it2->first) {
			if (it2->second->buddyIds.find(buddy)
					!= it2->second->buddyIds.end()) {
				deleteGroupId.push_back(it2->second->groupId);
				it2->second->buddyIds.erase(buddy);
				if (it2->second->buddyIds.empty()) {
					removeGroup.push_back(it2->first);
				}
			}
			it2++;
		} else {
			if (it2->second->buddyIds.insert(buddy).second) {
				addToGroupItem.push_back(it2->second->groupId);
			}
			it1++;
			it2++;
		}
	}
	while (it1 != nameSet.end()) {
		if (wptr->value.find(*it1) == wptr->value.end()) {
			addGroup.push_back(*it1);
		}
		it1++;
	}
	while (it2 != wptr->value.end()) {
		if (it2->second->buddyIds.find(buddy) != it2->second->buddyIds.end()) {
			deleteGroupId.push_back(it2->second->groupId);
			it2->second->buddyIds.erase(buddy);
			if (it2->second->buddyIds.empty()) {
				removeGroup.push_back(it2->first);
			}
		}
		it2++;
	}

	MCE_DEBUG("host: "<<host<<"  addToGroupItem.size:"<<addToGroupItem.size()
			<<"    deleteGroupId.size:"<<deleteGroupId.size()
			<<"   addGroup.size:"<<addGroup.size());
	if (addToGroupItem.empty() && deleteGroupId.empty() && addGroup.empty()) {
		MCE_DEBUG("host:"<<host<<"   return -1");
		return -1;
	}

	//	QueryRunner conn("buddygroup", CDbWServer,
	//			BuddyGroupManagerI::getTableName("relation_group", host));

	if (!addToGroupItem.empty()) {
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		//		ostringstream sql;
		query <<"insert into  "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host)<<" (host,guest,groupid) values ";
		for (size_t i = 0; i < addToGroupItem.size(); ++i) {
			if (i) {
				query<<",";
			}
			query<<"("<<host<<","<<buddy<<"," <<addToGroupItem.at(i)<<")";
		}
		query.execute();
	}

	if (!deleteGroupId.empty()) {
		//QueryRunner conn("buddygroup", CDbWServer);

		//		ostringstream sql;
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query << "delete from "<<BuddyGroupManagerI::getTableName(
				"relation_group_item", host)<<" where host = " << host
				<< " and guest = "<<buddy <<" and  groupid in (";
		for (size_t i = 0; i < deleteGroupId.size(); ++i) {
			if (i) {
				query<<",";
			}
			query<<deleteGroupId.at(i);
		}
		query << ")";
		query.execute();
	}

	if (!addGroup.empty()) {
		//QueryRunner conn("buddygroup", CDbWServer);
		//		ostringstream sql;
		int groupId = -1;
		{
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query<<"insert into "<<BuddyGroupManagerI::getTableName(
				"relation_group", host)<<" (host,name) values ";

		for (size_t i = 0; i < addGroup.size(); ++i) {
			if (i) {
				query<<",";
			}
			query<<"("<<host<<","<<mysqlpp::quote<<addGroup.at(i)<<")";
		}
		groupId = query.execute().insert_id();
		}
		if (groupId != -1) {
			{
			ConnectionHolder conn("buddygroup", CDbWServer,
					BuddyGroupManagerI::getTableName("relation_group", host));
			mysqlpp::Query query = conn.query();
			//			sql.str("");
			query <<"insert into "<<BuddyGroupManagerI::getTableName(
					"relation_group_item", host)
					<<" (host,guest,groupid) values ";
			for (size_t i = 0; i < addGroup.size(); ++i) {
				if (i) {
					query<<",";
				}
				query<<"("<<host<<","<<buddy<<","<<groupId+i<<")";
			}
			query.execute();
			}
			//------------------------------------
			for (size_t i = 0; i < addGroup.size(); ++i) {
				BuddyGroupPtr bgPtr = new BuddyGroup();
				bgPtr->groupId = groupId + i;
				bgPtr->buddyIds.insert(buddy);
				wptr->value[addGroup.at(i)] = bgPtr;
			}
		}
	}

	//delete empty group
	if (!removeGroup.empty()) {
		//		ostringstream sql;
		ConnectionHolder conn("buddygroup", CDbWServer,
				BuddyGroupManagerI::getTableName("relation_group", host));
		mysqlpp::Query query = conn.query();
		query<<"delete from "<<BuddyGroupManagerI::getTableName(
				"relation_group", host)<<"  where host="<<host
				<<" and name in (";

		for (size_t i = 0; i < removeGroup.size(); ++i) {
			BuddyGroupMap::iterator it = wptr->value.find(removeGroup.at(i));
			if (it != wptr->value.end()) {
				wptr->value.erase(it);
			}
			if (i) {
				query <<",";
			}
			query <<mysqlpp::quote<< removeGroup.at(i);
		}
		query << ")";
		//		QueryRunner conn("buddygroup", CDbWServer,
		//				BuddyGroupManagerI::getTableName("relation_group", host));
		query.execute();
	}
	//notify im
	if (_notifyIm) {
		if (current.ctx.find("FROM") != current.ctx.end()) {
			BuddyItemPtr bi = new BuddyItem();
			bi->host = host;
			bi->items[buddy] = nameSeq;

			IqMsgHandlerAdapter::instance().changeBuddyGroupNotify(bi);
		}
	}
	//---------
	MCE_DEBUG("host:"<<host<<"  return desc:"<<desc);
	return desc;

}

//int BuddyGroupManagerI::buddyChangeGroups(Ice::Int host, Ice::Int buddy,
//		const MyUtil::StrSeq& nameSeq, const Ice::Current&) {
//	return changeBuddyGroups(host, buddy, nameSeq);
//}


//---------------------------------

BuddiedGroupsMap BuddyGroupManagerI::getBuddiedGroupsMap(Ice::Int host,
		const Ice::Current&) {
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	BuddiedGroupsMap res;
	if (wptr) {
		IceUtil::Mutex::Lock lock(*wptr);
		//GroupMapWrapperPtr wptr = GroupMapWrapperPtr::dynamicCast(ptr);
		for (BuddyGroupMap::iterator it = wptr->value.begin(); it
				!= wptr->value.end(); ++it) {
			for (set<int>::iterator iit = it->second->buddyIds.begin(); iit
					!= it->second->buddyIds.end() ; ++iit) {
				res[*iit].push_back(it->first);
			}
		}

	}
	return res;
}

MyUtil::StrSeq BuddyGroupManagerI::getGroupList(Ice::Int host,
		const Ice::Current&) {
	MCE_DEBUG("BuddyGroupManagerI::getGroupList --> host:" <<host);
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	MyUtil::StrSeq res;
	if (wptr) {
		IceUtil::Mutex::Lock lock(*wptr);
		for (BuddyGroupMap::iterator it = wptr->value.begin(); it
				!= wptr->value.end(); ++it) {
			res.push_back(it->first);
		}
	}

	return res;
}

MyUtil::IntSeq BuddyGroupManagerI::getBuddyList(Ice::Int host,
		const string& groupName, Ice::Int begin, Ice::Int limit,
		const Ice::Current&) {
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	MyUtil::IntSeq ids;
	if (wptr) {
		IceUtil::Mutex::Lock lock(*wptr);
		map<string,BuddyGroupPtr>::iterator it = wptr->value.find(groupName);
		if (it != wptr->value.end()) {
			if (begin < 0) {
				begin = 0;
			}
			int index = 0;
			for (set<int>::iterator bit = it->second->buddyIds.begin(); bit
					!= it->second->buddyIds.end(); ++bit) {
				if (index < begin) {
					index ++;
					continue;
				}
				if (index >= begin+limit && limit >= 0) {
					break;
				}
				ids.push_back(*bit);
				index ++;
			}
		}

	}

	return ids;

}

MyUtil::StrSeq BuddyGroupManagerI::getBuddyGroups(Ice::Int host,
		Ice::Int guest, const Ice::Current&) {
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return MyUtil::StrSeq();
	}
	IceUtil::Mutex::Lock lock(*wptr);
	MyUtil::StrSeq res;
	for (map<string,BuddyGroupPtr>::iterator it = wptr->value.begin(); it
			!= wptr->value.end(); ++it) {
		if (it->second->buddyIds.find(guest) != it->second->buddyIds.end()) {
			res.push_back(it->first);
		}
	}
	return res;
}
MyUtil::StrSeq BuddyGroupManagerI::_getBuddyGroups(Ice::Int host, Ice::Int guest) {
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	if (!wptr) {
		return MyUtil::StrSeq();
	}

	MyUtil::StrSeq res;
	for (map<string,BuddyGroupPtr>::iterator it = wptr->value.begin(); it
			!= wptr->value.end(); ++it) {
		if (it->second->buddyIds.find(guest) != it->second->buddyIds.end()) {
			res.push_back(it->first);
		}
	}
	return res;
}

Ice::Int BuddyGroupManagerI::getCountByGroup(Ice::Int host,
		const string& groupName, const Ice::Current&) {
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);

	if (wptr) {
		IceUtil::Mutex::Lock lock(*wptr);
		map<string,BuddyGroupPtr>::iterator it = wptr->value.find(groupName);
		if (it != wptr->value.end()) {
			return it->second->buddyIds.size();
		}
	}
	return 0;
}

MyUtil::Str2IntMap BuddyGroupManagerI::getAllGroupsBuddyCount(int host,
		const Ice::Current&) {
	GroupMapWrapperPtr wptr = ServiceI::instance().locateObject<GroupMapWrapperPtr>(0, host);
	MyUtil::Str2IntMap counts;
	if (wptr) {
		IceUtil::Mutex::Lock lock(*wptr);
		map<string,BuddyGroupPtr>::iterator it = wptr->value.begin();
		for (; it!=wptr->value.end(); ++it) {
			counts[it->first] = it->second->buddyIds.size();
		}
	}
	return counts;
}

//----------------------------
void CleanIllegalDataTask::handle() {

	if (_groups.empty()) {
		return;
	}
	//	QueryRunner conn("buddygroup", CDbWServer,
	//			BuddyGroupManagerI::getTableName("relation_group", _host));
	{
	ConnectionHolder conn("buddygroup", CDbWServer,
			BuddyGroupManagerI::getTableName("relation_group", _host));
	mysqlpp::Query query = conn.query();
	//	ostringstream sql;
	query<<"delete from "<<BuddyGroupManagerI::getTableName("relation_group",
			_host)<<"  where host="<<_host <<" and name in (";

	for (map<int,string>::iterator it = _groups.begin(); it != _groups.end(); ++it) {
		MCE_INFO("Illegal GroupName:"<< it->second<< " GroupId:"<<it->first
				<< " host:"<<_host);

		if (it != _groups.begin()) {
			query<<",";
		}
		query<<mysqlpp::quote <<it->second;
	}
	query<<")";

	query.execute();
	}
	//	sql.str("");
	{
	ConnectionHolder conn1("buddygroup", CDbWServer,
			BuddyGroupManagerI::getTableName("relation_group", _host));
	mysqlpp::Query query = conn1.query();
	query<<"delete from "<<BuddyGroupManagerI::getTableName(
			"relation_group_item", _host)<<" where groupid in (";

	for (map<int,string>::iterator it = _groups.begin(); it != _groups.end(); ++it) {
		if (it != _groups.begin()) {
			query<<",";
		}
		query<<it->first;
	}

	query<< ") and host="<< _host;
	query.execute();
	}
}

