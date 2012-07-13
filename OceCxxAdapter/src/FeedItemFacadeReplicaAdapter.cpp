/*
 * =====================================================================================
 *
 *       Filename:  FeedItemFacadeReplicaAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月21日 15时42分33秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "FeedItemFacadeReplicaAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace xce::feed;
using namespace xce::clusterstate;

void FeedItemFacadeReplicaAdapter::put(const FeedSeedPtr& seed, const ::Ice::Current& current) {
	try {
		getProxy(seed->actor)->put(seed,current.ctx);

	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemFacadeReplicaAdapter::put, userid: " << seed->actor << " feedid: " << seed->feed << e.what());
	} catch (...) {
		MCE_WARN("FeedItemFacadeReplicaAdapter::put, userid: " << seed->actor << " feedid: " << seed->feed << " Exception!");
	}
}

