#include "PlacesFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool PlacesReadHandler::handle(mysqlpp::Row& row) const
{
	//printf("%d, %s, %d, %s, %d\n", 
	//		(int)row[0], row[1].c_str(), (int)row[2], row[3].c_str(), (int)row[4]);
	//getchar();
	USERID uid = atoi(row[0]);
	const char* place = row[1].c_str();
	int year = atoi(row[2]);
	// 该uid已经由PlacesFetcher::GetRange添加过了
	map<USERID,UserModel>::iterator iter = _models.find(uid);
	if (iter != _models.end())
	{
		if (iter->second.places.size()<20)
			iter->second.places.insert(make_pair<string,int>(place,year));
	}
	return true;
}

PlacesFetcher::PlacesFetcher()
{
	dbInstance = "common_friendrank";
	//ConnectionPoolManager::instance().initialize();
}

int PlacesFetcher::UpdateBatch(map<USERID,UserModel>& models, int low, int high)
{
	time_t time_start = time(NULL);
	// create sql statement
	{
		Statement sql;
		sql
			<< "SELECT userid,workplace_name,join_year FROM workplace_info"
			<< " WHERE userid>=" << low << " AND userid<=" << high;
		try {
			QueryRunner(dbInstance.c_str(), CDbRServer).query(sql,
					PlacesReadHandler(models));
		} catch (Ice::Exception& e) {
			MCE_ERROR(time(NULL)<<" [PlacesFetcher::UpdateBatch] Exception : " << e.what());
		}
	}
	{
		Statement sql;
		sql
			<< "SELECT userid,university_name,enroll_year FROM university_info"
			<< " WHERE userid>=" << low << " AND userid<=" << high;
		try {
			QueryRunner(dbInstance.c_str(), CDbRServer).query(sql,
					PlacesReadHandler(models));
		} catch (Ice::Exception& e) {
			MCE_ERROR(time(NULL)<<" [PlacesFetcher::UpdateBatch] Exception : " << e.what());
		}
	}
	{
		Statement sql;
		sql
			<< "SELECT userid,high_school_name,enroll_year FROM high_school_info"
			<< " WHERE userid>=" << low << " AND userid<=" << high;
		try {
			QueryRunner(dbInstance.c_str(), CDbRServer).query(sql,
					PlacesReadHandler(models));
		} catch (Ice::Exception& e) {
			MCE_ERROR(time(NULL)<<" [PlacesFetcher::UpdateBatch] Exception : " << e.what());
		}
	}

	// query database
	return time(NULL) - time_start;
}

