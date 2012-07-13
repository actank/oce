/*
 * CardInfoCacheAdapter.h
 *
 *  Created on: 2009-12-24
 *      Author: zhanghan
 */

#ifndef __CARD_INFO_CACHE_H__
#define __CARD_INFO_CACHE_H__

#include "CardCache.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"
#include "search/Card/share/ParsePrivacy.h"

namespace xce {
namespace cardcache{


class CardInfoCacheAdapter :  public MyUtil::Singleton<CardInfoCacheAdapter>{
public:
	CardInfoCacheAdapter();
	CardInfoSeq get(const MyUtil::IntSeq&);
	PartOfCardInfoSeq getPart(const MyUtil::IntSeq&);
	void setProperty(int userId, const MyUtil::Str2StrMap& props);
	ParsePrivacy parse;
private:
	virtual string name() {
		return "M";
	}

	virtual string endpoints() {
		return "@CardInfoCache";
	}

	virtual size_t cluster() {
		return 1;
	}
	xce::clusterstate::ClientInterface<CardInfoCachePrx> clientCS_;

};

};
};

#endif
