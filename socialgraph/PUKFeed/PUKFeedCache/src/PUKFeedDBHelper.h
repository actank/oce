#ifndef _PUKFEED_DBHELPER_H_
#define _PUKFEED_DBHELPER_H_

#include <PUKFeedCache.h>
#include <QueryRunner.h>

namespace dbhelper {

class PUKFeedDBHelper {
public:
	static void RemovePUKFeed(int userId, const xce::socialgraph::PUKFeedSeq& feedList);
};

//-----------------------------------------------------------------------------

class TableName {
public:
	static std::string PUKFeedBlockTable(int id) {
		ostringstream oss;
		oss << "pukfeed_block_" << (id % 10);
		return oss.str();
	}
};

}

#endif
