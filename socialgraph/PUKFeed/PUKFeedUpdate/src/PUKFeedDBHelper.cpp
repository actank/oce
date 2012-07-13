#include "PUKFeedDBHelper.h"

using namespace dbhelper;
using namespace com::xiaonei::xce;

void PUKFeedDBHelper::GetRemovePUKeed(int userId, int friendId, std::set<int>& blockSet) {
	MCE_INFO("[PUKFeedDBHelper] GetRemovePUKeed userId(" << userId << ") friendId(" << friendId << ") blockSet size(" << blockSet.size() << ")");
	string tableName = TableName::PUKFeedBlockTable(userId);
	Statement sql;
	sql
		<< "SELECT * FROM " << tableName
		<< " WHERE owner_id=" << userId 
		<< " AND friend_id=" << friendId;	

	try {
        QueryRunner("social_feed", CDbRServer).query(sql, PUKFeedBlockHandler(blockSet));
    } catch (Ice::Exception& e) {
        cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC Connection Exception: " << e.what() << endl;
    } catch (...) {
        cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC DB Unknow Exception" << endl;
    }
	
}

//-----------------------------------------------------------------------------

bool PUKFeedBlockHandler::handle(mysqlpp::Row& row) const {
	_blockSet.insert((int)row["common_id"]);
	return true;
}
