#ifndef __BUDDY_RELATION_LOADER_I_H__
#define __BUDDY_RELATION_LOADER_I_H__

#include <IceUtil/IceUtil.h>
#include <ServiceI.h>
#include "BuddyRelationCache.h"

namespace xce {
namespace buddy {

static const int TASK_LEVEL_RELOAD = 1;

//***************************************************************************

class BuddyRelationLoaderI : virtual public BuddyRelationLoader,
	virtual public MyUtil::Singleton<BuddyRelationLoaderI> {
public:
	virtual void reload(::Ice::Int, const Ice::Current& = Ice::Current());
	virtual void setRelation(const Relationship&, BuddyDesc, const Ice::Current& = Ice::Current());

private:
	friend class MyUtil::Singleton<BuddyRelationLoaderI>;
	BuddyRelationLoaderI(){
	}
};

//***************************************************************************

class ReloadTask : virtual public MyUtil::Task {
public:
	ReloadTask( ::Ice::Int id, ::Ice::Int level=TASK_LEVEL_RELOAD ) :
		Task(level),_id(id){
		}
protected:
	virtual void handle();
private:
	::Ice::Int _id;
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
	public:
		virtual void configure(const Ice::PropertiesPtr& props);
};

//***************************************************************************

}
}

#endif
