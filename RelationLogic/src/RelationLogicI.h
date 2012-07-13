#ifndef __RelationLogicI_h__
#define __RelationLogicI_h__

#include <RelationLogic.h>
#include <CountCacheAdapter.h>

namespace mop {

namespace hi {

namespace svc {

namespace relation {

const static string RELATION_LOGIC_MANAGER = "M";

class RelationLogicManagerI : virtual public RelationLogicManager {
public:

	virtual void addApply(::Ice::Int, ::Ice::Int, const Ice::Current&);

	virtual void acceptRequest(::Ice::Int, ::Ice::Int, const Ice::Current&);

	virtual void denyRequest(::Ice::Int, ::Ice::Int, const Ice::Current&);

	virtual void removeBuddy(::Ice::Int, ::Ice::Int, const Ice::Current&);

	virtual void addBlock(::Ice::Int, ::Ice::Int, const Ice::Current&);

	virtual void removeBlock(::Ice::Int, ::Ice::Int, const Ice::Current&);
};

}

}

}

}

#endif
