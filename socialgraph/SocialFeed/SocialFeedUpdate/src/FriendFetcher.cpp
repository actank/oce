#include "FriendFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool FriendReadHandler::handle(mysqlpp::Row& row) const
{
	if (_friends.size() < 2000)
		_friends.push_back(atoi(row[0]));
	return true;
}

FriendFetcher::FriendFetcher()
{
	dbInstance = "relation_friendrank";
	ConnectionPoolManager::instance().initialize();
}

int FriendFetcher::Read(int uid, vector<USERID>& friends)
{
	time_t time_start = time(NULL);
	ostringstream pattern;
	pattern << "relation_" << uid%100;
	Statement sql;
	sql
		<< "SELECT guest FROM relation_" << uid%100
		<< " WHERE host="<< uid;
	// query database
	try {
		QueryRunner(dbInstance.c_str(), CDbRServer, pattern.str()).query(sql,
				FriendReadHandler(friends));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [FriendFetcher::UpdateBatch] Exception : " << e.what() << "\n";
	}

	return time(NULL) - time_start;
}

