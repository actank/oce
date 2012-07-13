#include "GiftFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool GiftReadHandler::handle(mysqlpp::Row& row) const
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
		if (iter->second.giftReceivers.size()<200)
			iter->second.giftReceivers.insert(fid);
	}
	return true;
}

GiftFetcher::GiftFetcher()
{
	dbInstance = "gift_friendrank";
	//ConnectionPoolManager::instance().initialize();
}

int GiftFetcher::UpdateBatch(map<USERID,UserModel>& models, int low, int high)
{
	time_t time_start = time(NULL);
	for (int index=0; index<100; ++index)
	{
		// create sql statement
		ostringstream pattern;		
		pattern << "gift_record_sender_" << index;
		Statement sql;
		sql
			<< "SELECT sender_uid,receiver_uid"
			<< " FROM gift_record_sender_"<<index
			<< " WHERE sender_uid>="<< low << " AND sender_uid<=" << high;
		// query database
		try {
			QueryRunner(dbInstance.c_str(), CDbRServer, pattern.str()).query(sql,
					GiftReadHandler(models));
		} catch (Ice::Exception& e) {
			cout << time(NULL) << " [GiftFetcher::UpdateBatch] Exception : " << e.what() << "\n";
		}
	}
	return time(NULL) - time_start;
}

