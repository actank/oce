/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月24日 14时15分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "FeedItemCacheByAppidReplicaAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

int main(int argc, char** argv) {
	try {
		int appid = 147795;
		std::vector<int> appids;
		appids.push_back(appid);
		FeedMetaSeq items = FeedItemCacheByAppidReplicaAdapter::instance().getGreaterThanFeedidWithUseridByType(appid, 13855189146, 20, 23, appids);

		std::cout << "items.size(): " << items.size() << std::endl;
		for (FeedMetaSeq::iterator it = items.begin();
				it != items.end(); ++it) {
			std::cout << " feedid: " << (*it).feedid << std::endl;
		}
	} catch (::Ice::Exception& e) {
		std::cout << "Exception" << e.what() << std::endl;
	}
}
