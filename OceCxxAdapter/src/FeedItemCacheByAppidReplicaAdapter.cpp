/*
 * =====================================================================================
 *
 *       Filename:  FeedItemCacheByAppidReplicaAdapter.cpp
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

#include "FeedItemCacheByAppidReplicaAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace xce::feed;
using namespace xce::clusterstate;

void FeedItemCacheByAppidReplicaAdapter::put(const FeedMeta& meta, const ::Ice::Context& context) {
	try {
		vector<AppFeedsPrx> prxs = getAllProxySeq(meta.userid);

		for (vector<AppFeedsPrx>::iterator it = prxs.begin();
				it != prxs.end(); ++it) {
			(*it)->put(meta, context);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheByAppidReplicaAdapter::put, userid: " << meta.userid << " feedid: " << meta.feedid << e.what());
	} catch (...) {
		MCE_WARN("FeedItemCacheByAppidReplicaAdapter::put, userid: " << meta.userid << " feedid: " << meta.feedid << " Exception!");
	}
}

FeedMetaSeq FeedItemCacheByAppidReplicaAdapter::getByTypes(const MyUtil::IntSeq& appids, ::Ice::Int begin, ::Ice::Int limit, const MyUtil::IntSeq& types) {
	try {
		FeedMetaSeq result = getProxy(0)->getByTypes(appids, begin, limit, types);
		return result;
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheByAppidReplicaAdapter::getByTypes " << e.what());
	}

	return FeedMetaSeq();
}

FeedMetaSeq FeedItemCacheByAppidReplicaAdapter::getGreaterThanFeedid(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit) {
	try {
		FeedMetaSeq result = getProxy(0)->getGreaterThanFeedid(appid, feedid, limit);
		return result;
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheByAppidReplicaAdapter::get " << e.what());
	}

	return FeedMetaSeq();
}

FeedMetaSeq FeedItemCacheByAppidReplicaAdapter::getGreaterThanFeedidWithUseridByType(::Ice::Int appid, ::Ice::Long feedid, ::Ice::Int limit, ::Ice::Int userid, const MyUtil::IntSeq& types) {
	try {
		FeedMetaSeq result = getProxy(0)->getGreaterThanFeedidWithUseridByType(appid, feedid, limit, userid, types);
		return result;
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheByAppidReplicaAdapter::get " << e.what());
	}

	return FeedMetaSeq();
}
