#ifndef __BUDDY_RELATION_REPLICA_ADAPTER_H__
#define __BUDDY_RELATION_REPLICA_ADAPTER_H__

#include <boost/lexical_cast.hpp>
#include <IceUtil/RWRecMutex.h>
#include <BuddyRelationCache.h>
#include <BuddyCommon.h>
#include <AdapterI.h>
#include <Channel.h>
#include <Singleton.h>
#include <TaskManager.h>

namespace xce {
namespace buddy {
namespace adapter {

#ifndef BIG_ENDIAN
#define BIG_ENDIAN
#endif

class BuddyRelationCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyRelationCacheManagerPrx>, public MyUtil::Singleton<BuddyRelationCacheAdapter> {
public:
	BuddyRelationCacheAdapter() :
		MyUtil::ReplicatedClusterAdapterI <BuddyRelationCacheManagerPrx>("ControllerBuddyRelationCache", 120, 300) {
	}
        BuddyDesc getRelation(const Relationship&);
	void setRelation(const Relationship&, BuddyDesc);

        void addBuddyRelationData(Ice::Int, const BuddyRelationDataPtr&);
private:
	friend class MyUtil::Singleton<BuddyRelationCacheAdapter>;
	Ice::Int getMinId(const Relationship&);
};

class BuddyRelationTTAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyRelationTTManagerPrx>, public MyUtil::Singleton<BuddyRelationTTAdapter> {
public:
	BuddyRelationTTAdapter() :
		MyUtil::ReplicatedClusterAdapterI <BuddyRelationTTManagerPrx>("ControllerBuddyRelationTT", 120, 300) {
	}
        BuddyDesc getRelation(const Relationship&);
	void setRelation(const Relationship&, BuddyDesc);

        void addBuddyRelationData(Ice::Int, const BuddyRelationDataPtr&);
private:
	friend class MyUtil::Singleton<BuddyRelationTTAdapter>;
	Ice::Int getMinId(const Relationship&);
};

class BuddyRelationLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyRelationLoaderPrx>, public MyUtil::Singleton<BuddyRelationLoaderAdapter> {
public:
        BuddyRelationLoaderAdapter() :
                MyUtil::ReplicatedClusterAdapterI <BuddyRelationLoaderPrx>("ControllerBuddyRelationLoader", 120, 300) {
        }
        void reload(Ice::Int);
	void setRelation(const Relationship&, BuddyDesc);

private:
	friend class MyUtil::Singleton<BuddyRelationLoaderAdapter>;
};


};
};
};

#endif
