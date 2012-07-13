#include "NetworkCollector.h"
#include <boost/algorithm/string.hpp>

using namespace com::xiaonei::xce;
using namespace xce::friendfinder;
using namespace std;
using namespace boost;

NetworkCollector::NetworkCollector() {
}

NetworkCollector::~NetworkCollector() {
}

void NetworkCollector::getUnivInfo(int userId, vector< vector<string> >& univInfo) {
	MCE_INFO("[NetworkCollector] getUnivInfo userId(" << userId << ")");
	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	string tableName = "university_info";

	Statement sql;       // 查数据库
	sql	
		<< "SELECT * FROM " << tableName
		<< " where userid=" << userId;

	try {
		QueryRunner("university_info_friendfinder", CDbRServer).query(sql, UnivReadHandler(univInfo));
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << " [NetworkCollector] DB Connection Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[NetworkCollector] DB Unknow Exception");
	}

	//for (vector< vector<string> >::iterator iter = univInfo.begin();
	//		iter != univInfo.end(); ++iter) {
	//	ostringstream tr;
	//	for (vector<string>::iterator it = iter->begin();
	//			it != iter->end(); ++it) {
	//		tr << *it << "  ";	
	//	}
	//	MCE_INFO(tr.str());
	//}

    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;
	MCE_INFO("getUnivInfo from DB run tim (gettimeofday) : " << lTime << " second!");
}

void NetworkCollector::getHighschoolInfo(int userId, vector< vector<string> >& highschoolInfo) {
	MCE_INFO("[NetworkCollector] getHighschoolInfo userId(" << userId << ")");
	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	string tableName = "high_school_info";

	Statement sql;
	sql
 		<< "SELECT * FROM " << tableName
		<< " where userid="	<< userId;
	try {
		QueryRunner("high_school_info_friendfinder", CDbRServer).query(sql, HighschoolReadHandler(highschoolInfo));
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << " [NetworkCollector] DB Connection Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[NetworkCollector] DB Unknow Exception");
	}

	//for (vector< vector<string> >::iterator iter = highschoolInfo.begin();
	//		iter != highschoolInfo.end(); ++iter) {
	//	ostringstream tr;
	//	for (vector<string>::iterator it = iter->begin();
	//			it != iter->end(); ++it) {
	//		tr << *it << "  ";	
	//	}
	//	MCE_INFO(tr.str());
	//}
 
    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;
	MCE_INFO("getHighschoolInfo from DB run time (gettimeofday) : " << lTime << " second!");
}

void NetworkCollector::getJuniorschoolInfo(int userId, vector< vector<string> >& juniorschoolInfo) {
	MCE_INFO("[NetworkCollector] getJuniorschoolInfo userId(" << userId << ")");
	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	string tableName = "junior_high_school_info";
	Statement sql;
	sql
 		<< "SELECT * FROM " << tableName
		<< " where userid="	<< userId;
	try {
		QueryRunner("junior_high_school_info_friendfinder", CDbRServer).query(sql, JuniorschoolReadHandler(juniorschoolInfo));
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << " [NetworkCollector] DB Connection Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[NetworkCollector] DB Unknow Exception");
	}
 
	//for (vector< vector<string> >::iterator iter = juniorschoolInfo.begin();
	//		iter != juniorschoolInfo.end(); ++iter) {
	//	ostringstream tr;
	//	for (vector<string>::iterator it = iter->begin();
	//			it != iter->end(); ++it) {
	//		tr << *it << "  ";	
	//	}
	//	MCE_INFO(tr.str());
	//}

    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;
	MCE_INFO("getJuniorschoolInfo from DB run time (gettimeofday) : " << lTime << " second!");
}

void NetworkCollector::getElementaryschoolInfo(int userId, vector< vector<string> >& elementaryschoolInfo) {
	MCE_INFO("[NetworkCollector] getElementaryschoolInfo userId(" << userId << ")");
	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	string tableName = "elementary_school_info";
	Statement sql;
	sql
 		<< "SELECT * FROM " << tableName
		<< " where userid="	<< userId;
	try {
		QueryRunner("elementary_school_info_friendfinder", CDbRServer).query(sql, ElementaryschoolReadHandler(elementaryschoolInfo));
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << " [NetworkCollector] DB Connection Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[NetworkCollector] DB Unknow Exception");
	}
 
	//for (vector< vector<string> >::iterator iter = elementaryschoolInfo.begin();
	//		iter != elementaryschoolInfo.end(); ++iter) {
	//	ostringstream tr;
	//	for (vector<string>::iterator it = iter->begin();
	//			it != iter->end(); ++it) {
	//		tr << *it << "  ";	
	//	}
	//	MCE_INFO(tr.str());
	//}

    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;
	MCE_INFO("getElementaryschoolInfo from DB run time (gettimeofday) : " << lTime << " second!");
}

void NetworkCollector::getWorkplaceInfo(int userId, vector< vector<string> >& workplaceInfo) {
	MCE_INFO("[NetworkCollector] getWorkplaceInfo userId(" << userId << ")");
	string tableName = "workplace_info";
	Statement sql;
	sql
 		<< "SELECT * FROM " << tableName
		<< " where userid="	<< userId;
	try {
		QueryRunner("workplace_info_friendfinder", CDbRServer).query(sql, WorkplaceReadHandler(workplaceInfo));
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << " [NetworkCollector] DB Connection Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[NetworkCollector] DB Unknow Exception");
	}
}

void NetworkCollector::getUserAge(int userId, int& age) {
	MCE_INFO("[NetworkCollector] getUserAge userId(" << userId << ")");
	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	string tableName = "user_born";
	Statement sql;
	sql
 		<< "SELECT * FROM " << tableName
		<< " where id="	<< userId;
	try {
		QueryRunner("user_born_friendfinder", CDbRServer).query(sql, UserAgeReadHandler(age));
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << " [NetworkCollector] DB Connection Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[NetworkCollector] DB Unknow Exception");
	}
 
    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;
	MCE_INFO("getUserAge from DB run time (gettimeofday) : " << lTime << " second!");
}

void NetworkCollector::getUserAgeBatch(const MyUtil::IntSeq& userIds, MyUtil::Int2IntMap& userAgeMap) {
	MCE_INFO("[NetworkCollector] getUserAgeBatch");
	struct timeval tvStart,tvEnd;
    double linStart = 0,linEnd = 0,lTime = 0;
    gettimeofday(&tvStart, NULL);

	string tableName = "user_born";

	MyUtil::IntSeq::const_iterator subIt = userIds.begin();

	Statement sql;
	sql
		<< "SELECT * FROM " << tableName
		<< " WHERE id IN (" << *subIt;
	for(; subIt!=userIds.end(); ++subIt) {
		sql << ", " << *subIt;
	}
	sql << ") LIMIT " << userIds.size() * 10;

	try {
		QueryRunner("user_born_friendfinder", CDbRServer).query(sql, UserAgeBatchReadHandler(userAgeMap));
	} catch ( Ice::Exception& e ) {
		MCE_WARN(time(NULL) << " [NetworkCollector] DB Connection Exception : " << e.what() << "\n");
	} catch ( ... ) {
		MCE_WARN(time(NULL) << "[NetworkCollector] DB Unknow Exception");
	}
 
    gettimeofday(&tvEnd, NULL);
    linStart = ((double)tvStart.tv_sec * 1000000 + (double)tvStart.tv_usec);  //unit S
    linEnd = ((double)tvEnd.tv_sec * 1000000 + (double)tvEnd.tv_usec);        //unit S
    lTime = linEnd - linStart;
    lTime = lTime/1000000;
	MCE_INFO("getUserAgeBatch from DB run time (gettimeofday) : " << lTime << " second!");
}

//************************************************************************************************************************

bool UnivReadHandler::handle(mysqlpp::Row& row) const {
	vector<string> info;

	char tmpstr[25];
    sprintf(tmpstr, "%d", (int)row["enroll_year"]); 
    string enrollYear( tmpstr );
        
    string univName( row["university_name"] ); 
    string department( row["department"] );

	//去空格
    trim(univName);
    trim(department);
    trim(enrollYear);

	info.push_back( univName );
    info.push_back( department );
    info.push_back( enrollYear );

	_univInfo.push_back(info);

	return true;
}

//************************************************************************************************************************

bool HighschoolReadHandler::handle(mysqlpp::Row& row) const {
	vector<string> info;

	char tmpstr[25];
    sprintf(tmpstr, "%d", (int)row["enroll_year"]); 
    string enrollYear( tmpstr );

    string highschoolName( row["high_school_name"] );

	//去空格
    trim(highschoolName);
    trim(enrollYear); 

	info.push_back( highschoolName );
    info.push_back( enrollYear );

	_highschoolInfo.push_back(info);

	return true;
}
 
//************************************************************************************************************************

bool JuniorschoolReadHandler::handle(mysqlpp::Row& row) const {
	vector<string> info;

    char tmpstr[25];
    sprintf(tmpstr, "%d", (int)row["junior_high_school_year"]);
    string enrollYear( tmpstr );

    string juniorschoolName( row["junior_high_school_name"] );

	//去空格
    trim(juniorschoolName);
    trim(enrollYear);

	info.push_back( juniorschoolName );
    info.push_back( enrollYear );

	_juniorschoolInfo.push_back(info);	
	return true;
}

//************************************************************************************************************************

bool ElementaryschoolReadHandler::handle(mysqlpp::Row& row) const {
	vector<string> info;

    char tmpstr[25];
    sprintf(tmpstr, "%d", (int)row["elementary_school_year"]);
    string enrollYear( tmpstr );

    string elementaryschoolName( row["elementary_school_name"] );

	//去空格
    trim(elementaryschoolName);
    trim(enrollYear);	

	info.push_back( elementaryschoolName );
    info.push_back( enrollYear );

	_elementaryschoolInfo.push_back(info);	
	return true;
}

//************************************************************************************************************************

bool WorkplaceReadHandler::handle(mysqlpp::Row& row) const {
	vector<string> info;

	string workplaceName( row["workplace_name"] );

	//去空格
    trim(workplaceName);

	info.push_back( workplaceName );

	_workplaceInfo.push_back(info);	
	return true;
}

//************************************************************************************************************************

bool UserAgeReadHandler::handle(mysqlpp::Row& row) const {
    //char year[10];
	//time_t t = time(0);
	//strftime(year, sizeof(year), "%Y", localtime(&t));
	//int yearInt = atoi(year);
	_age = (int)row["birth_year"];
	//_age = yearInt - _age > 0 ? yearInt - _age : 0;	
	return true;
}

//************************************************************************************************************************

bool UserAgeBatchReadHandler::handle(mysqlpp::Row& row) const {
	int userId = (int) row["id"];
	int age = (int) row["birth_year"];
	_userAgeMap[userId] = age;
	return true;
}
