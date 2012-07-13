#include "PassportFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool PassportReadHandler::handle(mysqlpp::Row& row) const
{
	//printf("%d, %s, %d, %s, %d\n", 
	//		(int)row[0], row[1].c_str(), (int)row[2], row[3].c_str(), (int)row[4]);
	//getchar();
	++_recnum;
	if (atoi(row[1])==0)
	{
		_maxuid = atoi(row[0]);
		fwrite(&_maxuid, sizeof(int), 1, _fp);
	}
	return true;
}

PassportFetcher::PassportFetcher()
{
	dbInstance = "user_passport_friendrank";
	ConnectionPoolManager::instance().initialize();
}

pair<int,int> PassportFetcher::Save2File(int step, int beginid, FILE* fp)
{
	int maxuid = 0;
	int recnum = 0;
	// create sql statement
	Statement sql;
	sql
		<< "SELECT id,status FROM user_passport"
		<< " WHERE id>=" << beginid << " ORDER BY id LIMIT " << step;
	// query database
	try {
		QueryRunner(dbInstance.c_str(), CDbRServer).query(sql,
				PassportReadHandler(fp, maxuid, recnum));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [PassportFetcher::UpdateBatch] Exception : " << e.what() << "\n";
	}
	return make_pair<int,int>(maxuid,recnum);
}

