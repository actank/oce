#include "BasicFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool BasicReadHandler::handle(mysqlpp::Row& row) const
{
	//printf("%d, %s, %d, %s, %d\n", 
	//		(int)row[0], row[1].c_str(), (int)row[2], row[3].c_str(), (int)row[4]);
	//getchar();
	int uid = atoi(row[0].c_str());
	// 该uid已经由BasicFetcher::GetRange添加过了
	map<USERID,UserModel>::iterator iter = _models.find(uid);
	if (iter != _models.end())
	{
		if (row[1].c_str()==NULL)
			iter->second.gender = -1;  // 用户可以不填写
		else if (strcmp(row[1].c_str(),"男生")==0)
			iter->second.gender = 1;
		else if (strcmp(row[1].c_str(),"女生")==0)
			iter->second.gender = 0;
		else iter->second.gender = -1;

		if (row[2].c_str()==NULL)
			iter->second.birth_year = 0;
		else
			iter->second.birth_year = atoi(row[2].c_str());
	}
	return true;
}

BasicFetcher::BasicFetcher()
{
	dbInstance = "user_born_friendrank";
	//ConnectionPoolManager::instance().initialize();
}

int BasicFetcher::UpdateBatch(map<USERID,UserModel>& models, int low, int high)
{
	time_t time_start = time(NULL);
	// create sql statement
	Statement sql;
	sql	
		<< "SELECT id,gender,birth_year FROM user_born"
		<< " WHERE id>=" << low << " AND id<=" << high;
	// query database
	try {
		QueryRunner(dbInstance.c_str(), CDbRServer).query(sql,
				BasicReadHandler(models));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [BasicFetcher::UpdateBatch] Exception : " << e.what() << "\n";
	}
	return time(NULL) - time_start;
}

