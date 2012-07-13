
#include "logging/Counting/server/CountingI.h"

::xce::counting::CountingData
xce::counting::CountingManagerI::getCounting(const Ice::Current& current)
{
    return ::xce::counting::CountingData();
}

void
xce::counting::CountingManagerI::clearCounting(const ::std::string& type,
                                               const Ice::Current& current)
{
}
