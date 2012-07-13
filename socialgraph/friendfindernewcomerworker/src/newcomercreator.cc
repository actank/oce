#include "newcomercreator.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace MyUtil;
using namespace boost;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;
using namespace xce::socialgraph::friendfindernewcomer;

//***************************************************************************

NewComerDbData NewComerCreator::createGuest(int userId,int time,ostringstream& why){
	NewComerDbData res;
	res.time = time;
	res.userId = 0;

	ostringstream createLog;

	struct timeval tvStart, 
		tvStatusFilter, tvPhotoFilter, tvStarFilter,
		tvEnd;

	gettimeofday( &tvStart, NULL );
	double linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);

	bool status_is_true = NewComerUtil::isStatusTrue( userId );
	gettimeofday( &tvStatusFilter, NULL );
	double linStatusFilterEnd = ((double)tvStatusFilter.tv_sec*1000000 + (double)tvStatusFilter.tv_usec);
	double linStatusFilterTime = linStatusFilterEnd - linStart;
	linStatusFilterTime = linStatusFilterTime/1000000;
	createLog	<< " statusfilter(" << status_is_true << ")(" << linStatusFilterTime << "s)";
	if( status_is_true ){
		why << "status_is_true";
		MCE_INFO( "[NewComerCreator::createGuest] userId=" << userId << createLog.str() << " status_is_true" );
		return res;
	}

	bool photo_is_true = NewComerUtil::isPhotoStateTrue( userId );
	gettimeofday( &tvPhotoFilter, NULL );
	double linPhotoFilterEnd = ((double)tvPhotoFilter.tv_sec*1000000 + (double)tvPhotoFilter.tv_usec);
	double linPhotoFilterTime = linPhotoFilterEnd - linStatusFilterEnd;
	linPhotoFilterTime = linPhotoFilterTime/1000000;
	createLog	<< " photofilter(" << photo_is_true << ")(" << linPhotoFilterTime << "s)";
	if( !photo_is_true ){
		why << "photo_is_false";
		MCE_INFO( "[NewComerCreator::createGuest] userId=" << userId << createLog.str() << " photo_is_false" );
		return res;
	}

	bool star_is_true = NewComerUtil::isStateStarTrue( userId );
	gettimeofday( &tvStarFilter, NULL );
	double linStarFilterEnd = ((double)tvStarFilter.tv_sec*1000000 + (double)tvStarFilter.tv_usec);
	double linStarFilterTime = linStarFilterEnd - linPhotoFilterEnd;
	linStarFilterTime = linStarFilterTime/1000000;
	createLog	<< " starfilter(" << star_is_true << ")(" << linStarFilterTime << "s)";
	if( !star_is_true ){
		why << "star_is_false";
		MCE_INFO( "[NewComerCreator::createGuest] userId=" << userId << createLog.str() << " star_is_false" );
		return res;
	}

	gettimeofday( &tvEnd, NULL );
	double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
	double linTime = linEnd - linStart;
	linTime = linTime/1000000;
	createLog	<< " total(" << linTime << "s)";
	why << "created";
	MCE_INFO( "[NewComerCreator::createGuest] userId=" << userId << createLog.str() << " success!" );

	res.userId = userId;
	return res;
}

Int2IntSeqMap NewComerCreator::findHosts(int userId,const IntSeq& friendList){
	Int2IntSeqMap res;

        vector< vector<string> > univ_info_tmp = NetworkCollector::instance().getUnivInfo(userId);
        vector< vector<string> > highschool_info_tmp = NetworkCollector::instance().getHighschoolInfo(userId);

	for( vector< vector<string> >::iterator univ_it = univ_info_tmp.begin(); univ_it != univ_info_tmp.end(); ++univ_it ){
		vector<string>::iterator this_univ = univ_it->begin();
		if( this_univ != univ_it->end() ){
			int univ_id = boost::lexical_cast<int>(*this_univ);
			++this_univ;
			vector<string> univ_info = vector<string>( this_univ, univ_it->end() );
			res[ univ_id ] = filterHostFriends( userId, NewComerUtil::getUnivData(userId,univ_info), friendList );
		}
	}

	for( vector< vector<string> >::iterator highschool_it = highschool_info_tmp.begin(); highschool_it != highschool_info_tmp.end(); ++highschool_it ){
		vector<string>::iterator this_highschool = highschool_it->begin();
		if( this_highschool != highschool_it->end() ){
			int highschool_id = boost::lexical_cast<int>(*this_highschool);
			++this_highschool;
			vector<string> highschool_info = vector<string>( this_highschool, highschool_it->end() );
			res[ highschool_id ] = filterHostFriends( userId, NewComerUtil::getHighschoolData(userId,highschool_info), friendList );
		}
	}

	return res;
}

MyUtil::IntSeq NewComerCreator::filterHostFriends(int userId, const MyUtil::IntSeq& infoFriends, const MyUtil::IntSeq& friendList) {
        IntSeq result;
        for(IntSeq::const_iterator iter=infoFriends.begin(); iter!=infoFriends.end(); ++iter) {

                IntSeq::const_iterator fIt = lower_bound( friendList.begin(), friendList.end(), *iter );
                if( ( fIt != friendList.end() && *fIt == *iter ) || ( *iter == userId ) ) {
                        continue;
                }
                result.push_back( *iter );
        }

        return result;
}

NetworkCollector::NetworkCollector() {
}

vector< vector<string> > NetworkCollector::getUnivInfo(int userId) {
	vector< vector<string> > univInfo;

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
	return univInfo;
}

vector< vector<string> > NetworkCollector::getHighschoolInfo(int userId) {
	vector< vector<string> > highschoolInfo;

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
	return highschoolInfo;
}

vector< vector<string> > NetworkCollector::getJuniorschoolInfo(int userId) {
	vector< vector<string> > juniorschoolInfo;

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
	return juniorschoolInfo;

}

vector< vector<string> > NetworkCollector::getElementaryschoolInfo(int userId) {
	vector< vector<string> > elementaryschoolInfo;

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
	return elementaryschoolInfo;

}

vector< vector<string> > NetworkCollector::getWorkplaceInfo(int userId) {
	vector< vector<string> > workplaceInfo;

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
	return workplaceInfo;
}

int NetworkCollector::getUserAge(int userId) {
	int age;

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
	return age;
}

MyUtil::Int2IntMap NetworkCollector::getUserAgeBatch(const MyUtil::IntSeq& userIds) {
	MyUtil::Int2IntMap userAgeMap;

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
	return userAgeMap;

}

//************************************************************************************************************************

bool UnivReadHandler::handle(mysqlpp::Row& row) const {
	vector<string> info;

	char tmpstr[25];
	sprintf(tmpstr, "%d", (int)row["enroll_year"]); 
	string enrollYear( tmpstr );

	int univId = (int)row["university_id"]; 
	string univName( row["university_name"] ); 
	string department( row["department"] );

	//去空格
	trim(univName);
	trim(department);
	trim(enrollYear);

	info.push_back( boost::lexical_cast<string>(univId) );
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

	int highschoolId = (int)row["high_school_id"]; 
	string highschoolName( row["high_school_name"] );

	//去空格
	trim(highschoolName);
	trim(enrollYear); 

	info.push_back( boost::lexical_cast<string>(highschoolId) );
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
