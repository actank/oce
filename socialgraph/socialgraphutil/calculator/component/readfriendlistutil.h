#ifndef _READ_FRIENDLIST_UTIL_H_
#define _READ_FRIENDLIST_UTIL_H_

#include "DbCxxPool/src/QueryRunner.h"
#include "UtilCxx/src/TaskManager.h"

#include "socialgraph/socialgraphutil/calculator/component.h"

namespace readfriend {

const int TASK_LEVEL_READ_FRIEND = 11;

//---------------------------------------------------------------

class ReadFriendListUtil : public xce::socialgraph::Component {
public:
	ReadFriendListUtil() { com::xiaonei::xce::ConnectionPoolManager::instance().initialize(); }
	~ReadFriendListUtil() {}
	void GetFriendList(const std::vector<int>& idList, std::map<int, std::vector<int> >& result);
	void GetFriendListSynchronism(const std::vector<int>& idList, std::map<int, std::vector<int> >& result);

	void Load() {}
};

//---------------------------------------------------------------
//批量获取用户好友列表
class ReadFriendTask : public MyUtil::Task {
public:
	ReadFriendTask(int userId, std::vector<int>& friendList, ::Ice::Int level = TASK_LEVEL_READ_FRIEND):
		MyUtil::Task(level), _userId(userId), _friendList(friendList) {
	}
	virtual void handle();

private:
	int _userId;
	std::vector<int>& _friendList;
};

//---------------------------------------------------------------

class FriendRelationReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	FriendRelationReadHandler(std::vector<int>& friendList) : _friendList(friendList) { }
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector<int>& _friendList;
};

//---------------------------------------------------------------
//buddyrelation表名
class TableName {
public:
	static std::string RelationTableName(int id) {
		ostringstream oss;
		oss << "relation_" << id % 100;
		return oss.str();
	}

	static std::string RelationPattern(int id) {
		ostringstream oss;
		oss << "relation_" << id % 100;
		return oss.str();
	}
};

}

#endif
