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

#include "FeedItemCacheByUseridReplicaAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

int main(int argc, char** argv) {
	try {
		int userid = 233940269;
		std::vector<int> userids;
		userids.push_back(userid);
		FeedMetaSeq items = FeedItemCacheByUseridReplicaAdapter::instance().get(userids, 0, 20);

		std::cout << "items.size(): " << items.size() << std::endl;
		for (FeedMetaSeq::iterator it = items.begin();
				it != items.end(); ++it) {
			std::cout << " feedid: " << (*it).feedid << std::endl;
		}
	} catch (::Ice::Exception& e) {
		std::cout << "Exception" << e.what() << std::endl;
	}
}
