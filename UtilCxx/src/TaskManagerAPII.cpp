
#include <TaskManagerAPII.h>
#include "TaskManager.h"

void
xce::svcapi::TaskManagerAPII::config(::Ice::Int level,
                                     ::Ice::Int core,
                                     ::Ice::Int max,
                                     ::Ice::Int stack,
                                     const Ice::Current& current)
{
    MyUtil::TaskManager::instance().config(level, xce::util::ThreadPoolConfig(core, max));
}

int
xce::svcapi::TaskManagerAPII::clear(::Ice::Int level,
                                    const Ice::Current& current)
{
    return MyUtil::TaskManager::instance().clear(level);
}

int
xce::svcapi::TaskManagerAPII::size(::Ice::Int level,
                                    const Ice::Current& current) const
{
    return MyUtil::TaskManager::instance().size(level);
}

MyUtil::Int2IntMap
xce::svcapi::TaskManagerAPII::getLevel2Size(const Ice::Current& current) const
{
	return MyUtil::TaskManager::instance().getLevel2Size();
}
