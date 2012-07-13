#include "DBHelper.h"
using namespace com::xiaonei::xce; 
using namespace MyUtil; 
using namespace xce::dbhelper; 

DBHelper::DBHelper() {
	ConnectionPoolManager::instance().initialize();
}

void DBHelper::GetBirthdayRecommendList(int t_month, int t_day, int m_month, int m_day, std::vector<int>& birthdayRecommendList) {
	cout << "[DBHelper] GetBirthdayRecommendList t_month(" << t_month << ") t_day(" << 
		t_day << ") m_month(" << m_month << ") m_day(" << m_day << ")" << endl;
	string tableName = "user_born";
	int lowboundary = 0;
	int last = 0;

	do {
		size_t queryCount = 0;
		
		Statement sql;
		sql
			<< "SELECT * FROM " << tableName
			<< " WHERE id>" << lowboundary
			<< " limit 10000";

		try {
			queryCount = QueryRunner("user_born_friendfinder", CDbRServer).query(sql, BirthdayBatchReadHandler(last, t_month, t_day, 
																						m_month, m_day, birthdayRecommendList));
		} catch (Ice::Exception& e) {
			cout << "[DBHelper] GetBirthdayRecommendList " << tableName << " Connection Exception:" << e.what() << endl;
		} catch (...) {
			cout << "[DBHelper] GetBirthdayRecommendList " << tableName << " DB unknow Exception" << endl;
		}

		//if ((int)birthdayRecommendList.size() % 10 == 0) {
		//	fprintf(stderr, "birthdayRecommendList size=%ld, last=%ld \r", birthdayRecommendList.size(), last);
		//}
		//cout << "last bb : " << last << endl;
		//fprintf(stderr, "birthdayRecommendList size=%ld, last=%d \n", birthdayRecommendList.size(), last);

		if (last > lowboundary) {
			lowboundary = last;
		} else if(queryCount <= 0) {
			cout << "[DBHelper] GetBirthdayRecommendList lowboundary(" << lowboundary << ") at end of " << tableName << endl;
			break;
		}
	} while(true);
}

void DBHelper::ReadFriendList(int userId, std::vector<int>& friends) {
	//cout << "[DBHelper] ReadFriendList userId(" << userId << ")" << endl;	
	ostringstream pattern;
	pattern << "relation_" << userId % 100;	

	Statement sql;
	sql
		<< "SELECT * FROM relation_" << userId % 100
		<< " WHERE host=" << userId;

	try {
		QueryRunner("relation_friendrank", CDbRServer, pattern.str()).query(sql, FriendReadHandler(friends));	
	} catch (Ice::Exception& e) {
		cout << "[DBHelper] ReadFriendList Connection Exception:" << e.what() << endl;
	} catch (...) {
		cout << "[DBHelper] ReadFriendList DB unknow Exception" << endl;
	}
}

void DBHelper::FliterRecommendBlock(int userId, std::vector<int>& birthdayRecommendList) {
	string tableName = TableName::BirthdayBlockTable(userId);
	//cout << "[DBHelper] FliterRecommendBlock userId(" << userId << ") birthdayRecommendList size(" << 
	//	birthdayRecommendList.size() << ") tableName(" << tableName << ")" << endl;

	vector<int> blockList;
		
	Statement sql;
	sql
		<< "SELECT * FROM " << tableName 
		<< " WHERE owner_id=" << userId;

	try {
		QueryRunner("birthday_recommend", CDbRServer).query(sql, BirthdayRecommendBlockHandler(blockList));
	} catch (Ice::Exception& e) {
		cout << "[DBHelper] FliterRecommendBlock Connection Exception:" << e.what() << endl;
	} catch (std::exception& e) {
		cout << "[DBHelper] FliterRecommendBlock std Exception:" << e.what() << endl;
	} catch (...) {
		cout << "[DBHelper] FliterRecommendBlock unknown exception" << endl;
	}

	//cout << "blockList size : " << blockList.size() << endl;
	//for (vector<int>::iterator iter = blockList.begin();
	//		iter != blockList.end(); ++iter) {
	//	cout << *iter << "  ";
	//}
	//cout << endl;

	set<int> recommendSet(birthdayRecommendList.begin(), birthdayRecommendList.end());
	//cout << "recommendSet size(" << recommendSet.size() << ")" << endl;

	for (vector<int>::iterator iter = blockList.begin();
			iter != blockList.end(); ++iter) {
		set<int>::iterator fIt = recommendSet.find(*iter);
		if (fIt != recommendSet.end()) {
			recommendSet.erase(fIt);
		}
	}

	birthdayRecommendList.clear();
	for (set<int>::iterator iter = recommendSet.begin();
			iter != recommendSet.end(); ++iter) {
		birthdayRecommendList.push_back(*iter);
	}
}

//--------------------------------------------------------------------------------------------

bool BirthdayBatchReadHandler::handle(mysqlpp::Row& row) const {
	if (row["id"]) { 
		_last = _last > (int)row["id"] ? _last : (int)row["id"];	

		if (((int)row["birth_month"] == _t_month && (int)row["birth_day"] == _t_day)) {
			//fprintf(stderr, "BirthdayBatchReadHandler %d\t%d\t%d\n", (int)row["id"], (int)row["birth_month"], (int)row["birth_day"]);
			_birthdayRecommendList.push_back((int)row["id"]);
		}
		//if (((int)row["birth_month"] == _t_month && (int)row["birth_day"] == _t_day) ||
		//	((int)row["birth_month"] == _m_month && (int)row["birth_day"] == _m_day)) {
		//	//fprintf(stderr, "%d\t%d\t%d\n", (int)row["id"], (int)row["birth_month"], (int)row["birth_day"]);
		//
		//	_birthdayRecommendList.push_back((int)row["id"]);
		//}
	}
	return true;
}

//--------------------------------------------------------------------------------------------

bool FriendReadHandler::handle(mysqlpp::Row& row) const {
	if ((int)_friends.size() < 2000) {
		_friends.push_back((int)row["guest"]);
	}
	return true;
}

//--------------------------------------------------------------------------------------------

bool BirthdayRecommendBlockHandler::handle(mysqlpp::Row& row) const {
	_recommendBlockList.push_back((int)row["friend_id"]);

	return true;
}
