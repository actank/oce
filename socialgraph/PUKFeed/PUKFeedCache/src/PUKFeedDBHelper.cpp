#include "PUKFeedDBHelper.h"

using namespace dbhelper;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;

void PUKFeedDBHelper::RemovePUKFeed(int userId, const xce::socialgraph::PUKFeedSeq& feedList) {
	string tableName = TableName::PUKFeedBlockTable(userId);
	MCE_INFO("[PUKFeedDBHelper] RemovePUKFeed userId(" << userId << ") feedList size(" << feedList.size() << ")");

    time_t ltime = time(NULL);
    struct tm *Tm = localtime(&ltime);
    int year = Tm->tm_year + 1900;
    int month = Tm->tm_mon + 1;
    int day = Tm->tm_mday;
    int time = year*10000 + month*100 + day;

	for (PUKFeedSeq::const_iterator iter = feedList.begin();
			iter != feedList.end(); ++iter) {
		Statement sql;
		sql << "INSERT INTO " << tableName << " (owner_id, friend_id, common_id, block_time) VALUES (" <<
			userId << ", " << iter->friendId << ", " << iter->commonId << ", " << time << ")" << 
			" ON DUPLICATE KEY UPDATE common_id=VALUES(common_id), block_time=VALUES(block_time);";     

		try {
			QueryRunner("social_feed", CDbWServer, tableName).execute( sql );
		} catch (Ice::Exception& e) {
			MCE_WARN("[PUKFeedDBHelper] RemovePUKFeed id=" << userId << " Ice Exception " << e.what());
		} catch (std::exception& e) {
			MCE_WARN("[PUKFeedDBHelper] RemovePUKFeed id=" << userId << " std exception " << e.what()); 
		} catch (...) {
			MCE_WARN("[PUKFeedDBHelper] RemovePUKFeed id=" << userId << " unknown exception"); 
		}
	}
}
