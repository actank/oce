#include "FriendFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool FriendReadHandler::handle(mysqlpp::Row& row) const
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
		if (iter->second.friends.size()<2000)
			iter->second.friends.insert(fid);
	}
	return true;
}

FriendFetcher::FriendFetcher()
{
	dbInstance = "relation_friendrank";
	ConnectionPoolManager::instance().initialize();
}

int FriendFetcher::UpdateBatch(map<USERID,UserModel>& models, int low, int high)
{
	time_t time_start = time(NULL);
	for (int index=0; index<100; ++index)
	{
		// create sql statement
		ostringstream pattern;		
		pattern << "relation_" << index;
		Statement sql;
		sql
			<< "SELECT host,guest FROM relation_" << index
			<< " WHERE host>="<< low << " AND host<=" << high;
		// query database
		try {
			QueryRunner(dbInstance.c_str(), CDbRServer, pattern.str()).query(sql,
					FriendReadHandler(models));
		} catch (Ice::Exception& e) {
			cout << time(NULL) << " [FriendFetcher::UpdateBatch] Exception : " << e.what() << "\n";
		}
	}
	return time(NULL) - time_start;
}

