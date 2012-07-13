#ifndef __ActivitiesI_h__
#define __ActivitiesI_h__

#include <Activities.h>

namespace com
{

namespace xiaonei
{

namespace svc
{

namespace activities
{

const std::string& MANAGER = "Manager";

class ActivitiesManagerI : virtual public ActivitiesManager
{
public:

    virtual void occur(const ::com::xiaonei::svc::activities::ActivityRepresent&,
                       const ::com::xiaonei::svc::activities::ActivityRepresent&,
                       const Ice::Current&);
};

}

}

}

}

#endif
