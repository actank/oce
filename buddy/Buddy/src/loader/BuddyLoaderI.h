#ifndef __BUDDY_LOADER_I_H__
#define __BUDDY_LOADER_I_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "Buddy.h"
#include "BuddyCommon.h"

namespace xce {
namespace buddy {

class BuddyLoaderI : virtual public BuddyLoader,
        virtual public MyUtil::Singleton<BuddyLoaderI> {
public:
	virtual void load(::Ice::Int, const Ice::Current&);
private:
        friend class MyUtil::Singleton<BuddyLoaderI>;
};

//***************************************************************************
/*
class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};*/

}
}

#endif//__BUDDY_LOADER_I_H__
