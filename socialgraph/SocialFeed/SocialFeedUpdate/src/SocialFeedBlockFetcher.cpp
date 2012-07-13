#include "SocialFeedBlockFetcher.h"
using namespace com::xiaonei::xce;
using namespace MyUtil;

bool SocialFeedBlockHandler::handle(mysqlpp::Row& row) const
{
	if (_feedfps.size() < 2000)
		_feedfps.insert(atol(row[0]));
	return true;
}

SocialFeedBlockFetcher::SocialFeedBlockFetcher()
{
	dbInstance = "social_feed";
	ConnectionPoolManager::instance().initialize();
}

int SocialFeedBlockFetcher::Read(int uid, set<long>& feedfps)
{
	time_t time_start = time(NULL);
	ostringstream pattern;
	pattern << "socialfeed_block_" << uid%10;
	Statement sql;
	sql
		<< "SELECT feed_index FROM socialfeed_block_" << uid%10
		<< " WHERE owner_id="<< uid;
	// query database
	try {
		QueryRunner(dbInstance.c_str(), CDbRServer, pattern.str()).query(sql,
				SocialFeedBlockHandler(feedfps));
	} catch (Ice::Exception& e) {
		cout << time(NULL) << " [SocialFeedBlockFetcher::UpdateBatch] Exception : " << e.what() << "\n";
	}

	return time(NULL) - time_start;
}

