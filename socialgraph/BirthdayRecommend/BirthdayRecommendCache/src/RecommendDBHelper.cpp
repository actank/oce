#include "RecommendDBHelper.h"

using namespace xce::helper;
using namespace com::xiaonei::xce;
using namespace std;

void RecommendDBHelper::RemoveRecommend(int hostId, int friendId) {
	string tableName = TableName::BirthdayBlockTable(hostId);
	MCE_INFO("[RecommendDBHelper] RemoveRecommend hostId(" << hostId << ") friendId(" << friendId << ") tableName(" << tableName << ")");
	
	time_t ltime = time(NULL);
	struct tm *Tm = localtime(&ltime);
	int year = Tm->tm_year + 1900;
	int month = Tm->tm_mon + 1;
	int day = Tm->tm_mday;
	int time = year*10000 + month*100 + day;

	Statement sql;
	sql << "INSERT INTO " << tableName << " (owner_id, friend_id, time) VALUES (" <<
		hostId << ", " << friendId << ", " << time << ")" << " ON DUPLICATE KEY UPDATE time=VALUES(time);";

	try {
		QueryRunner("birthday_recommend", CDbWServer, tableName).execute( sql );
	} catch (Ice::Exception& e) {
		MCE_WARN("[RecommendDBHelper] RemoveRecommend id=" << hostId << " Ice Exception " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[RecommendDBHelper] RemoveRecommend id=" << hostId << " std exception " << e.what());	
	} catch (...) {
		MCE_WARN("[RecommendDBHelper] RemoveRecommend id=" << hostId << " unknown exception");
	}
}
