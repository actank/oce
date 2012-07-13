#ifndef SOCIALFEEDHELPER_H_
#define SOCIALFEEDHELPER_H_

#include <string>

#include <SocialFeedCache.h>
#include <QueryRunner.h>
#include <ObjectCacheI.h>
#include "MD5Util.h"

namespace xce {
namespace helper {

class SocialFeedHelper {
public:
	static void removeSocialFeed(int userId, const xce::socialgraph::Feed& feedItem);
};

//***************************************************************************************

class TableName {
public:
	static std::string FeedBlockTable(int id) {
		ostringstream oss;
		oss << "socialfeed_block_" << (id % 10);
		return oss.str();
	}
};

}
}

#endif
