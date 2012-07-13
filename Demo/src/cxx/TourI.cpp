
#include "ServiceI.h"
#include <TourI.h>

using namespace com::xiaonei::service::tour;
void
MyUtil::initialize() {
    ServiceI& service = ServiceI::instance();
    service.getAdapter()->add(new CalcI, service.createIdentity("O", ""));
}

::Ice::Int
com::xiaonei::service::tour::CalcI::op1(::Ice::Int a,
                                        ::Ice::Int b,
                                        const Ice::Current& current)
{
    return a+b;
}

::Ice::Int
com::xiaonei::service::tour::CalcI::op2(::Ice::Int a,
                                        ::Ice::Int b,
                                        const Ice::Current& current)
{
    return a*b;
}
