/*
 * =====================================================================================
 *
 *       Filename:  FeedItemCacheReplicaAdapter.cpp
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

#include "FeedItemCacheReplicaAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace xce::feed;
using namespace xce::clusterstate;

void FeedItemCacheReplicaAdapter::put(const FeedMeta& meta) {
	try {
		vector<UserFeedsPrx> prxs = getAllProxySeq(meta.userid);

		for (vector<UserFeedsPrx>::iterator it = prxs.begin();
				it != prxs.end(); ++it) {
			(*it)->put(meta);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheReplicaAdapter::put, userid: " << meta.userid << " feedid: " << meta.feedid << e.what());
	} catch (...) {
		MCE_WARN("FeedItemCacheReplicaAdapter::put, userid: " << meta.userid << " feedid: " << meta.feedid << " Exception!");
	}
}

FeedMetaSeq FeedItemCacheReplicaAdapter::get(const MyUtil::IntSeq& ids, int begin, int limit) {
	try {
		FeedMetaSeq result = getProxy(0)->get(ids, begin, limit);
		return result;
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheReplicaAdapter::get " << e.what());
	}

	return FeedMetaSeq();
}

FeedMetaSeq FeedItemCacheReplicaAdapter::getByTypes(const MyUtil::IntSeq& ids, int begin, int limit, const MyUtil::IntSeq& types) {
	try {
		FeedMetaSeq result = getProxy(0)->getByTypes(ids, begin, limit, types);
		return result;
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheReplicaAdapter::getByTypes " << e.what());
	}

	return FeedMetaSeq();
}

FeedMetaSeq FeedItemCacheReplicaAdapter::getWithFilter(const MyUtil::IntSeq& ids, int begin, int limit, const MyUtil::StrSeq& filter) {
	try {
		FeedMetaSeq result = getProxy(0)->getWithFilter(ids, begin, limit, filter);
		return result;
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheReplicaAdapter::getWithFilter " << e.what());
	}

	return FeedMetaSeq();
}

FeedMetaSeq FeedItemCacheReplicaAdapter::getByTypesWithFilter(const MyUtil::IntSeq& ids, int begin, int limit, const MyUtil::IntSeq& types, const MyUtil::StrSeq& filter) {
	try {
		FeedMetaSeq result = getProxy(0)->getByTypesWithFilter(ids, begin, limit, types, filter);
		return result;
	} catch (Ice::Exception& e) {
		MCE_WARN("FeedItemCacheReplicaAdapter::getByTypesWithFilter " << e.what());
	}

	return FeedMetaSeq();
}
