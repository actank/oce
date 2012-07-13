/*
 * CardRelationCacheAdapter.h
 *
 *  Created on: 2009-12-24
 *      Author: zhanghan
 */

#ifndef __CARD_RELATION_CACHE_H__
#define __CARD_RELATION_CACHE_H__

#include "CardCache.h"
#include "Channel.h"
#include "AdapterI.h"
#include "Singleton.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace cardcache{


class CardRelationCacheAdapter :  public MyUtil::Singleton<CardRelationCacheAdapter>{
public:
	CardRelationCacheAdapter();
	MyUtil::IntSeq get(int,int,int,int);
	void remove(int,int);
	AllRelationPtr getAllRelation(int);

private:
	virtual string name() {
		return "M";
	}

	virtual string endpoints() {
		return "@CardRelationCache";
	}

	virtual size_t cluster() {
		return 1;
	}
	xce::clusterstate::ClientInterface<CardRelationCachePrx> clientCS_;

};

};
};

#endif
