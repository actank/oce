#include "GroupFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool GroupNameHandler::handle(mysqlpp::Row& row) const
{
	//printf("%d, %s, %d, %s, %d\n", 
	//		(int)row[0], row[1].c_str(), (int)row[2], row[3].c_str(), (int)row[4]);
	//getchar();
	int groupid = atoi(row[0].c_str());
	if (!(row[1].is_null()) && row[1]=="家人亲戚")
		_familyIds.insert(groupid);
	return true;
}
bool GroupReadHandler::handle(mysqlpp::Row& row) const
{
	//printf("%d, %s, %d, %s, %d\n", 
	//		(int)row[0], row[1].c_str(), (int)row[2], row[3].c_str(), (int)row[4]);
	//getchar();
	USERID uid = atoi(row[0]);
	USERID fid = atoi(row[1]);
	int groupid = atoi(row[2]);
	// 该uid已经由GroupFetcher::GetRange添加过了
	map<USERID,UserModel>::iterator iter = _models.find(uid);
	if (iter != _models.end() && _familyIds.find(groupid)!=_familyIds.end())
	{
		iter->second.groups.insert(fid);
	}
	return true;
}

GroupFetcher::GroupFetcher()
{
	dbInstance = "buddygroup_friendrank";
	//ConnectionPoolManager::instance().initialize();
}

int GroupFetcher::UpdateBatch(map<USERID,UserModel>& models, int low, int high)
{
	set<int> familyIds;
	ReadGroupFamilyId(low, high, familyIds);

	time_t time_start = time(NULL);
	for (int i=0; i<100; ++i)
	{
		// create sql statement
		ostringstream pattern;		
		pattern << "relation_group_item_" << i;
		Statement sql;
		sql
			<< "SELECT host,guest,groupid"
			<< " FROM relation_group_item_"<<i
			<< " WHERE host>="<< low << " AND host<=" << high;
		// query database
		try {
			QueryRunner(dbInstance.c_str(), CDbRServer, pattern.str()).query(sql,
					GroupReadHandler(models, familyIds));
		} catch (Ice::Exception& e) {
			cout << time(NULL) << " [GroupFetcher::UpdateBatch] Exception : " << e.what() << "\n";
		}
	}
	return time(NULL) - time_start;
}

int GroupFetcher::ReadGroupFamilyId(int begin, int end, set<int>& familyIds)
{
	time_t time_start = time(NULL);
	for (int i=0; i<100; ++i)
	{
		// create sql statement
		ostringstream pattern;		
		pattern << "relation_group_" << i;
		Statement sql;
		sql
			<< "SELECT groupid,name FROM relation_group_" << i
			<< " WHERE host>="<< begin << " AND host<=" << end;
		// query database
		try {
			QueryRunner(dbInstance.c_str(), CDbRServer, pattern.str()).query(sql,
					GroupNameHandler(familyIds));
		} catch (Ice::Exception& e) {
			cout << time(NULL) << " [GroupFetcher::UpdateBatch] Exception : " << e.what() << "\n";
		}
	}
	return time(NULL) - time_start;
}

