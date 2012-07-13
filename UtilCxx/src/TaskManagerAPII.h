#ifndef __TaskManagerAPII_h__
#define __TaskManagerAPII_h__

#include <TaskManagerAPI.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

namespace xce
{

namespace svcapi
{

class TaskManagerAPII : virtual public TaskManagerAPI
{
public:
	TaskManagerAPII(const Ice::ObjectAdapterPtr& adapter) {
		if (adapter) {
			adapter->add(this, adapter->getCommunicator()->stringToIdentity("TMA"));
		}
	}

    virtual void config(::Ice::Int,
                              ::Ice::Int,
                              ::Ice::Int,
                              ::Ice::Int,
                              const Ice::Current&);

    virtual int clear(::Ice::Int,
                             const Ice::Current&);

    virtual int size(::Ice::Int,
                             const Ice::Current&) const;
		
		virtual MyUtil::Int2IntMap getLevel2Size(const Ice::Current&) const;
};

typedef IceUtil::Handle<TaskManagerAPII> TaskManagerAPIIPtr;

}

}

#endif
