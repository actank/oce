#include "InviteFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool InviteReadHandler::handle(mysqlpp::Row& row) const
{
	//printf("%d, %s, %d, %s, %d\n", 
	//		(int)row[0], row[1].c_str(), (int)row[2], row[3].c_str(), (int)row[4]);
	//getchar();
	USERID uid = atoi(row[0]);
	USERID fid = atoi(row[1]);
	// 该uid已经由InviteFetcher::GetRange添加过了
	map<USERID,UserModel>::iterator iter = _models.find(uid);
	if (iter != _models.end())
	{
		if (iter->second.guests.size()<500)
			iter->second.guests.insert(fid);
	}
	return true;
}

InviteFetcher::InviteFetcher()
{
	dbInstance = "stat_friendrank";
	//ConnectionPoolManager::instance().initialize();
}

int InviteFetcher::UpdateBatch(map<USERID,UserModel>& models, int low, int high)
{
	time_t time_start = time(NULL);
	// create sql statement
	Statement sql;
	sql
		<< "SELECT inviter_id,owner_id FROM reg_invite_map"
		<< " WHERE inviter_id>=" << low << " AND inviter_id<=" << high;
	// query database
	try {
		QueryRunner(dbInstance.c_str(), CDbRServer).query(sql,
				InviteReadHandler(models));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [InviteFetcher::UpdateBatch] Exception : " << e.what() << "\n";
	}
	return time(NULL) - time_start;
}

