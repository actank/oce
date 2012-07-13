#ifndef _DB_HELPER_H_
#define _DB_HELPER_H_

#include "QueryRunner.h"

namespace xce {
namespace dbhelper {

class DBHelper {
public:
	DBHelper();
	virtual ~DBHelper() {}
	void GetBirthdayRecommendList(int t_month, int t_day, int m_month, int m_day, std::vector<int>& birthdayRecommendList);
	void ReadFriendList(int userId, std::vector<int>& friends);
	void FliterRecommendBlock(int userId, std::vector<int>& birthdayRecommendList);
};

//--------------------------------------------------------------------------------------------

class TableName {
public:
	static std::string BirthdayBlockTable(int id) {
		ostringstream oss;
		oss << "birthdayrecommend_block_" << (id % 10);
		return oss.str();
	}
};

//--------------------------------------------------------------------------------------------

class BirthdayBatchReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	BirthdayBatchReadHandler(int& last, int t_month, int t_day, int m_month, int m_day, std::vector<int>& birthdayRecommendList) : 
		_last(last), _t_month(t_month), _t_day(t_day), _m_month(m_month), _m_day(m_day), _birthdayRecommendList(birthdayRecommendList) {
		}
	virtual bool handle(mysqlpp::Row& row) const;
	
private:
	int& _last;
	int _t_month;
	int _t_day;
	int _m_month;
	int _m_day;
	std::vector<int>& _birthdayRecommendList;
};

//--------------------------------------------------------------------------------------------

class FriendReadHandler : public com::xiaonei::xce::ResultHandler {
public:
	FriendReadHandler(std::vector<int>& friends) : _friends(friends) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector<int>& _friends;
};

//--------------------------------------------------------------------------------------------

class BirthdayRecommendBlockHandler : public com::xiaonei::xce::ResultHandler {
public:
	BirthdayRecommendBlockHandler(std::vector<int>& recommendBlockList) : _recommendBlockList(recommendBlockList) {}
	virtual bool handle(mysqlpp::Row& row) const;

private:
	std::vector<int>& _recommendBlockList;
};

}
}
#endif
