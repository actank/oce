#include "UserFilter.h"
#include "util/cpp/String.h"
#include "UserStateAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace xce::userfilter;
using namespace com::xiaonei::xce;
using namespace xce::adapter::userstate;

void UserLoginInfoLoader::loadFromDB(LoginInfoMap& loginInfoMap, time_t now, unsigned min, unsigned max) {
	string TableName = "user_login_";
	char year[10];
	char month[10];
	char date[10];

	strftime(year, sizeof(year), "%Y", localtime(&now));
	strftime(month, sizeof(month), "%m", localtime(&now));
	strftime(date, sizeof(date), "%d", localtime(&now));

	string currentFile = TableName + string(year) + string(month);

	time_t last = now - (atoi(date) + 3) * 24 * 3600;        //上个月份数据表
	strftime(year, sizeof(year), "%Y", localtime(&last));
	strftime(month, sizeof(month), "%m", localtime(&last));
	strftime(date, sizeof(date), "%d", localtime(&last));

	string lastFile = TableName + string(year) + string(month);

	cout << "[LoginAnalyzer] analyze firstFile(" << currentFile << ") secondFile(" << lastFile << ")" << endl;

	loadCurrentTable( currentFile, min, max );
	loadLastTable( lastFile, min, max );
	buildBitMap(loginInfoMap);               //拼接最近30天数据
}

void UserLoginInfoLoader::loadCurrentTable(const std::string& tableName, unsigned min, unsigned max) {
	cout << "[UserLoginInfoLoader] loadCurrentTable(" << tableName << ")" << endl;
	int lowboundary = min;
	int last = 0;
	int highboundary = max;

	do {
		size_t queryCount = 0;
		Statement sql; //查数据库
		sql << "select id, CAST(`login_days` AS signed) as frequence from "
				<< tableName << " where id > " << lowboundary << " && id < " << highboundary << " limit 100000";
		try {
			queryCount
					= QueryRunner("friendfinder_userlogin", CDbRServer).query(
							sql, CurrentLoginResultHandler(last, loginInfoMapOf2Month));
		} catch (Ice::Exception& e) {
			cout << time(NULL) << "[UserLoginInfoLoader] loadCurrentTable "
					<< tableName << " Connection Exception : " << e.what()
					<< "\n" << endl;
		} catch (...) {
			cout << time(NULL)
					<< "[UserLoginInfoLoader] loadCurrentTable DB unknow Exception"
					<< endl;
		}
		if (last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			cout << "[UserLoginInfoLoader] loadCurrentTable lowboundary("
					<< lowboundary << ") at end of " << tableName << endl;
			break;
		}
	} while (true);
	cout << "1 loginInfoMapOf2Month size : " << loginInfoMapOf2Month.size() << endl;
}

void UserLoginInfoLoader::loadLastTable(const std::string& tableName, unsigned min, unsigned max) {
	cout << "[UserLoginInfoLoader] loadLastTable(" << tableName << ")" << endl;
	int lowboundary = min;
	int last = 0;
	int highboundary = max;

	do {
		size_t queryCount = 0;
		Statement sql; //查数据库
		sql << "select id, CAST(`login_days` AS signed) as frequence from "
			<< tableName << " where id > " << lowboundary << " && id < " << highboundary << " limit 100000";
		try {
			queryCount
					= QueryRunner("friendfinder_userlogin", CDbRServer).query(
							sql, LastLoginResultHandler(last, loginInfoMapOf2Month));
		} catch (Ice::Exception& e) {
			cout << time(NULL) << "[UserLoginInfoLoader] loadLastTable " << tableName
					<< " Connection Exception : " << e.what() << endl;
		} catch (...) {
			cout << time(NULL)
					<< "[UserLoginInfoLoader] loadLastTable DB unknow Exception"
					<< endl;
		}

		if (last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			cout << "[UserLoginInfoLoader] loadLastTable lowboundary(" << lowboundary
					<< ") at end of " << tableName << endl;
			break;
		}
	} while (true);
	cout << "2 loginInfoMapOf2Month size : " << loginInfoMapOf2Month.size() << endl;
}

void UserLoginInfoLoader::buildBitMap(LoginInfoMap& loginInfoMap) {
	cout << "[UserLoginInfoLoader] buildBitMap" << endl;

	char date[10];
	time_t t = time(0);
	strftime(date, sizeof(date), "%d", localtime(&t));
	int today = atoi(date);

	for (map<int, vector<int> >::iterator iter = loginInfoMapOf2Month.begin(); iter
			!= loginInfoMapOf2Month.end(); ++iter) {
		if (iter->second.size() < 2) {
			continue;
		}

		bitset < 32 > bitvec1(iter->second.at(0));
		bitset < 32 > bitvec2(iter->second.at(1));

		bitset < 32 > result;
		result.reset();

		int pos = 0;
		for (int i = today - 1; i >= 0; --i) {
			if (bitvec1.test(i)) {
				result.set(pos);
			} else {
				result.reset(pos);
			}
			++pos;
		}

		int endTime = pos;
		for (int j = 31; j > endTime; --j) {
			if (bitvec2.test(j)) {
				result.set(pos);
			} else {
				result.reset(pos);
			}
			++pos;
		}

		if(result.any()){
			loginInfoMap[iter->first] = result;
		}
	}
}

bool AllUserResultHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int) row["id"] ? _last : (int) row["id"];
	int id = (int) row["id"];
	_userBitMap->set(id);
	return true;
}
//--------------------------------------------------------------
bool CurrentLoginResultHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int) row["id"] ? _last : (int) row["id"];
	int id = (int) row["id"];
	int frequence = (int) row["frequence"];
	_userLoginMap[id].push_back(frequence);
	_userLoginMap[id].push_back(0);
	return true;
}

//--------------------------------------------------------------
bool LastLoginResultHandler::handle(mysqlpp::Row& row) const {
	_last = _last > (int) row["id"] ? _last : (int) row["id"];
	int id = (int) row["id"];
	int frequence = (int) row["frequence"];
	map<int, vector<int> >::iterator fIt = _userLoginMap.find(id);
	if (fIt != _userLoginMap.end()) {
		fIt->second.at(1) = frequence;
	} else {
		_userLoginMap[id].push_back(0); //default
		_userLoginMap[id].push_back(frequence);
	}
	return true;
}

//--------------------------------------------------------------
bool ActiveChecker::checkID(int id, LoginInfo loginInfo){
	LoginInfo li;
	li |= mask;
	li &= loginInfo;
	if(li.count() >= _loginDays){
		return true;
	}else{
		return false;
	}
}

//--------------------------------------------------------------
void UserLoginFilter::getLoginUserIDs(UserBitMap* userBitMap, LoginInfoMap& loginInfoMap, CheckerPtr& checker){
	for(LoginInfoMap::iterator iter=loginInfoMap.begin(); iter!=loginInfoMap.end(); ++iter) {
		if(checker->checkID(iter->first, iter->second)){
			userBitMap->set(iter->first);
		}
	}
}

void AllUserFilter::getAllUserIDs(UserBitMap * userBitMap, unsigned min, unsigned max){
	cout << "[AllUserFilter] loadTable( user_passport )" << endl;
	int lowboundary = min;
	int last = 0;
	int highboundary = max;

	do {
		size_t queryCount = 0;
		Statement sql; //查数据库
		sql << "SELECT id FROM user_passport WHERE id > " << lowboundary << " && id < " << highboundary << " ORDER BY id limit 100000";
		AllUserResultHandler handler(last, userBitMap);
		try {
			queryCount = QueryRunner("user_passport", CDbRServer).query(sql, handler);
		} catch (Ice::Exception& e) {
			cout << time(NULL) << "[UserLoginInfoLoader] loadLastTable user_passport Connection Exception : " << e.what() << endl;
		} catch (...) {
			cout << time(NULL) << "[UserLoginInfoLoader] loadLastTable DB unknow Exception"	<< endl;
		}

		if (last > lowboundary) {
			lowboundary = last;
		} else if (queryCount <= 0) {
			cout << "[UserLoginInfoLoader] loadLastTable lowboundary(" << lowboundary << ") at end of user_passport " << endl;
			break;
		}
	} while (true);
}

