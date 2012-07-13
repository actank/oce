#include "HotFeedDBHelper.h"

using namespace xce::socialgraph;
using namespace xce::dbhelper;
using namespace com::xiaonei::xce;

void HotFeedDBHelper::RemoveHotFeed(int hostId, const xce::socialgraph::HotFeedSeq& hotFeedList) {
	string tableName = TableName::HotFeedBlockTable(hostId);
	MCE_INFO("[HotFeedDBHelper] RemoveHotFeed hostId(" << hostId << 
			") hotFeedList size(" << hotFeedList.size() << ") tableName(" << tableName << ")");

	time_t ltime = time(NULL);
    struct tm *Tm = localtime(&ltime);
    int year = Tm->tm_year + 1900;
    int month = Tm->tm_mon + 1;
    int day = Tm->tm_mday;    
    int time = year*10000 + month*100 + day;

	for (HotFeedSeq::const_iterator iter = hotFeedList.begin();
			iter != hotFeedList.end(); ++iter) {
		Statement sql;
		sql << "INSERT INTO " << tableName << " (owner_id, friend_id, feed_id, block_time) VALUES (" <<
			hostId << ", " << iter->userId << ", " << iter->feedId << ", " << time << ")" << 
			" ON DUPLICATE KEY UPDATE feed_id=VALUES(feed_id), block_time=VALUES(block_time);"; 	

		try {
			QueryRunner("social_feed", CDbWServer, tableName).execute( sql );
		} catch (Ice::Exception& e) {
			MCE_WARN("[HotFeedDBHelper] RemoveHotFeed id=" << hostId << " Ice Exception " << e.what());
		} catch (std::exception& e) {
			MCE_WARN("[HotFeedDBHelper] RemoveHotFeed id=" << hostId << " std exception " << e.what()); 
		} catch (...) {
			MCE_WARN("[HotFeedDBHelper] RemoveHotFeed id=" << hostId << " unknown exception"); 
		}
	}
}
