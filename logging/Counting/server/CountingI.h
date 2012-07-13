#ifndef __CountingI_h__
#define __CountingI_h__

#include <Counting.h>

namespace xce
{

namespace counting
{

class CountingManagerI : virtual public CountingManager
{
public:

    virtual ::xce::counting::CountingData getCounting(const Ice::Current&);

    virtual void clearCounting(const ::std::string&,
                               const Ice::Current&);
};

}

}

#endif
