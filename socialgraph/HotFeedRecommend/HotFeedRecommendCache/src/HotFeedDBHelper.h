#ifndef _HOTFEED_DBHELPER_H_
#define _HOTFEED_DBHELPER_H_

#include <HotFeedRecommendCache.h>
#include <QueryRunner.h>

namespace xce {
namespace dbhelper {

class HotFeedDBHelper {
public:
	static void RemoveHotFeed(int hostId, const xce::socialgraph::HotFeedSeq& hotFeedList);
};

//*******************************************************************************

class TableName {
public:
	static std::string HotFeedBlockTable(int id) {
		ostringstream oss;
		oss << "hotfeedrecommend_block_" << (id % 10);	
		return oss.str();
	}
};

}
}

#endif
