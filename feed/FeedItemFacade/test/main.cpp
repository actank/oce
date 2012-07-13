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

#include "FeedItemFacadeReplicaAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

int main(int argc, char** argv) {
	try {
		FeedSeedPtr seed = new FeedSeed;
		seed->actor = 233940269;
		seed->feed = 0;
		seed->time= 0;

		std::map<string, string> tmp;
		tmp["AppId"] = "23394";
		tmp["UserId"] = "233940269";

		seed->extraProps = tmp;

		::Ice::Current current;
		current.ctx = tmp;
		FeedItemFacadeReplicaAdapter::instance().put(seed, current);
	} catch (::Ice::Exception& e) {
		std::cout << "Exception" << e.what() << std::endl;
	}
}
