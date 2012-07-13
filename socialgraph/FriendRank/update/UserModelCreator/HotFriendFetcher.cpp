#include "HotFriendFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool HotFriendReadHandler::handle(mysqlpp::Row& row) const
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
		if (iter->second.hotFriends.size()<20)
			iter->second.hotFriends.insert(fid);
	}
	return true;
}

HotFriendFetcher::HotFriendFetcher()
{
	dbInstance = "relation_info_friendrank";
	//ConnectionPoolManager::instance().initialize();
}

int HotFriendFetcher::UpdateBatch(map<USERID,UserModel>& models, int low, int high)
{
	time_t time_start = time(NULL);
	// create sql statement
	Statement sql;
	sql
		<< "SELECT host,guest FROM hotfriend"
		<< " WHERE host>=" << low << " AND host<=" << high;
	// query database
	try {
		QueryRunner(dbInstance.c_str(), CDbRServer).query(sql,
				HotFriendReadHandler(models));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [HotFriendFetcher::UpdateBatch] Exception : " << e.what() << "\n";
	}
	return time(NULL) - time_start;
}

