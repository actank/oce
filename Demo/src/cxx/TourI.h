#ifndef ___home_bbc_code_XiaoNei_trunk_Tour_generated_cxx_TourI_h__
#define ___home_bbc_code_XiaoNei_trunk_Tour_generated_cxx_TourI_h__

#include <Tour.h>

namespace com
{

namespace xiaonei
{

namespace service
{

namespace tour
{

class CalcI : virtual public Calc
{
public:

    virtual ::Ice::Int op1(::Ice::Int,
                           ::Ice::Int,
                           const Ice::Current&);

    virtual ::Ice::Int op2(::Ice::Int,
                           ::Ice::Int,
                           const Ice::Current&);
};

}

}

}

}

#endif
