#include "FeedConfigFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool WhiteListReadHandler::handle(mysqlpp::Row& row) const
{
	//printf("%d, %s, %d, %s, %d\n", 
	//		(int)row[0], row[1].c_str(), (int)row[2], row[3].c_str(), (int)row[4]);
	//getchar();
	USERID uid = atoi(row[0]);
	USERID fid = atoi(row[1]);
	// 该uid已经由BasicFetcher::GetRange添加过了
	map<USERID,UserModel>::iterator iter = _models.find(uid);
	if (iter != _models.end())
	{
		if (iter->second.feedlove.size()<100)
			iter->second.feedlove.insert(fid);
	}
	return true;
}
bool BlackListReadHandler::handle(mysqlpp::Row& row) const
{
	//printf("%d, %s, %d, %s, %d\n", 
	//		(int)row[0], row[1].c_str(), (int)row[2], row[3].c_str(), (int)row[4]);
	//getchar();
	USERID uid = atoi(row[0]);
	USERID fid = atoi(row[1]);
	// 该uid已经由BasicFetcher::GetRange添加过了
	map<USERID,UserModel>::iterator iter = _models.find(uid);
	if (iter != _models.end())
	{
		if (iter->second.feedhate.size()<100)
			iter->second.feedhate.insert(fid);
	}
	return true;
}

FeedConfigFetcher::FeedConfigFetcher()
{
	dbInstance = "relation_info_friendrank";
	//ConnectionPoolManager::instance().initialize();
}

int FeedConfigFetcher::UpdateBatch(map<USERID,UserModel>& models, int low, int high)
{
	time_t time_start = time(NULL);
	ReadFeedConfig(models, low, high, "relation_feed_high", true);
	ReadFeedConfig(models, low, high, "relation_feed_low", false);
	return time(NULL) - time_start;
}

int FeedConfigFetcher::ReadFeedConfig(map<USERID,UserModel>& models, int low, int high,
		const char* tablename, bool love)
{
	time_t time_start = time(NULL);
	// create sql statement
	Statement sql;
	sql
		<< "SELECT source,target FROM " << tablename
		<< " WHERE source>=" << low << " AND source<=" << high;
	// query database
	try {
		if (love)
			QueryRunner(dbInstance.c_str(), CDbRServer).query(sql, WhiteListReadHandler(models));
		else
			QueryRunner(dbInstance.c_str(), CDbRServer).query(sql, BlackListReadHandler(models));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [FeedConfigFetcher::UpdateBatch] Exception : " << e.what() << "\n";
	}
	return time(NULL) - time_start;
}

