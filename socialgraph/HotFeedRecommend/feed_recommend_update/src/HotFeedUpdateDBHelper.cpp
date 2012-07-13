#include "HotFeedUpdateDBHelper.h"

using namespace xce::dbhelper;
using namespace item;
using namespace com::xiaonei::xce;

void HotFeedUpdateDBHelper::GetUserPhotoUGC(int userId, std::vector<item::UGCItem>& ugcList, int begin, int end) {
    string tableName = TableName::PhotoTableName(userId);
    string pattern = TableName::PhotoTablePattern(userId);

    char year[10];
    char month[10];
    char date[10];

    time_t t = time(0);
    t = t - end * 24 * 3600;     //回退end天
    strftime(year, sizeof(year), "%Y", localtime(&t));
    strftime(month, sizeof(month), "%m", localtime(&t));
    strftime(date, sizeof(date), "%d", localtime(&t));

    string endTime = string(year) + "-" + string(month) + "-" + string(date);
    //cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC userId=" << userId << "  endTime=" << endTime << endl; 

    t = time(0);
    t = t - begin * 24 * 3600;     //回退begin天
    strftime(year, sizeof(year), "%Y", localtime(&t));
    strftime(month, sizeof(month), "%m", localtime(&t));
    strftime(date, sizeof(date), "%d", localtime(&t));
    string startTime = string(year) + "-" + string(month) + "-" + string(date);
    //cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC userId=" << userId << "  startTime=" << startTime << endl; 

    Statement sql;
    sql
        << "SELECT * FROM " << tableName <<
        " WHERE owner=" << userId << " AND time BETWEEN " <<
        mysqlpp::quote << startTime << " AND " << mysqlpp::quote << endTime;

    try {
        QueryRunner("adminsearch", CDbRServer, pattern).query(sql, PhotoDataReadHelper(ugcList));
    } catch (Ice::Exception& e) {
        cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC Connection Exception: " << e.what() << endl;
    } catch (...) {
        cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC DB Unknow Exception" << endl;
    }

    //cout << "ugcList size = " << ugcList.size() << endl;
}

void HotFeedUpdateDBHelper::GetUserBlogUGC(int userId, std::vector<item::UGCItem>& ugcList, int begin, int end) {
    string tableName = TableName::BlogTableName(userId);
    string pattern = TableName::BlogTablePattern(userId);

    char year[10];
    char month[10];
    char date[10];

    time_t t = time(0);
    t = t - end * 24 * 3600;     //回退end天
    strftime(year, sizeof(year), "%Y", localtime(&t));
    strftime(month, sizeof(month), "%m", localtime(&t));
    strftime(date, sizeof(date), "%d", localtime(&t));

    string endTime = string(year) + "-" + string(month) + "-" + string(date);
    //cout << "[HotFeedUpdateDBHelper] GetUserBlogUGC userId=" << userId << "  endTime=" << endTime << endl; 

    t = time(0);
    t = t - begin * 24 * 3600;     //回退begin天
    strftime(year, sizeof(year), "%Y", localtime(&t));
    strftime(month, sizeof(month), "%m", localtime(&t));
    strftime(date, sizeof(date), "%d", localtime(&t));

    string startTime = string(year) + "-" + string(month) + "-" + string(date);
    //cout << "[HotFeedUpdateDBHelper] GetUserBlogUGC userId=" << userId << "  startTime=" << startTime << endl; 
    //cout << "tableName : " << tableName << endl;

    Statement sql;
    sql
        << "SELECT * FROM " << tableName <<
        " WHERE owner=" << userId << " AND time BETWEEN " <<
        mysqlpp::quote << startTime << " AND " << mysqlpp::quote << endTime;

    try {
        QueryRunner("adminsearch", CDbRServer, pattern).query(sql, BlogDataReadHelper(ugcList));
    } catch (Ice::Exception& e) {
        cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC Connection Exception: " << e.what() << endl;
    } catch (...) {
        cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC DB Unknow Exception" << endl;
    }
    //cout << "ugcList size = " << ugcList.size() << endl;
}

void HotFeedUpdateDBHelper::GetUserBlockUGC(int accepterId, int userId, std::vector<long>& blockUgcList) {
	string tableName = TableName::HotFeedBlockTable(accepterId);

	Statement sql;
	sql
		<< "SELECT * FROM " << tableName << " WHERE owner_id=" 
		<< accepterId << " AND friend_id=" << userId;

	try {
		QueryRunner("social_feed", CDbRServer).query(sql, BlockUGCReadHelper(blockUgcList));
	} catch (Ice::Exception& e) {
		cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC Connection Exception: " << e.what() << endl;
	} catch (...) {
		cout << "[HotFeedUpdateDBHelper] GetUserPhotoUGC DB Unknow Exception" << endl;
	}
}

//--------------------------------------------------------------------------------------------

bool PhotoDataReadHelper::handle(mysqlpp::Row& row) const {
	//int state = 0;
	if (IsMultiPhoto(row["state"])) {
		//state = MULTI_PHOTO;

		UGCItem ugcUnit((long)row["album"], (int)row["commentcount"], (int)row["viewcount"], MULTI_PHOTO);
		_ugcList.push_back(ugcUnit);
	} else {
		//state = SINGLE_PHOTO;

		UGCItem ugcUnit((long)row["id"], (int)row["commentcount"], (int)row["viewcount"], SINGLE_PHOTO);
		_ugcList.push_back(ugcUnit);
	}	

	return true;
}

bool PhotoDataReadHelper::IsMultiPhoto(int state) const {
	if ((state & STATE_MULTI_PHOTO) == STATE_MULTI_PHOTO) {
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------

bool BlogDataReadHelper::handle(mysqlpp::Row& row) const {
	if ((int)row["blogControl"] == -1) {         // 过滤隐私设置,仅自己可见
		return true;
	}

	UGCItem ugcUnit((long)row["id"], (int)row["comment_count"], (int)row["view_count"], BLOG);
	_ugcList.push_back(ugcUnit);

	return true;
}

//--------------------------------------------------------------------------------------------

bool BlockUGCReadHelper::handle(mysqlpp::Row& row) const {
	_blockUgcList.push_back((long)row["feed_id"]);
	return true;
}
