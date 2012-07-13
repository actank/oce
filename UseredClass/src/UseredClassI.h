#ifndef __USEREDCLASSI_H__
#define __USEREDCLASSI_H__

#include <UseredClass.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceBox/IceBox.h>
#include "ServiceI.h"
#include "Channel.h"
#include "EvictorTemplate.h"

#ifndef USERED_CLASS_API
#   define USERED_CLASS_API ICE_DECLSPEC_EXPORT
#endif

extern "C"
{
USERED_CLASS_API IceBox::Service*
	    create(Ice::CommunicatorPtr communicator);
}


namespace mop
{

namespace hi
{

namespace svc
{

namespace model
{
const Ice::Int CLASS_SET = 0;
const std::string UNIV_SET = "US";
const std::string USERED_CLASS_MANAGER = "MANAGER";
class USERED_CLASS_API UseredClassServiceI : public MyUtil::ServiceI,
								public MyUtil::Singleton<UseredClassServiceI>
{
public:
    UseredClassSetPtr getClassSet(Ice::Int userId);
protected:
    virtual void initialize();
};

//---------------------------------------------------------------------------
class ServantFactoryI : public MyUtil::ServantFactory<Ice::Int>
{
    public:
        virtual Ice::ObjectPtr create(const Ice::Int& id);
};

//---------------------------------------------------------------------------
class UseredClassManagerI : virtual public UseredClassManager,
                            public IceUtil::Mutex
{
    public:
        virtual Ice::Int getAllClassCount(Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());
        virtual Ice::Int getAdminedClassCount(Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());
        virtual Ice::Int getMemberedClassCount(Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());
        virtual Ice::Int getFoundedClassCount(Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());


        virtual ::MyUtil::IntSeq getAllClasses(Ice::Int,
                Ice::Int,
                Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());

        virtual ::MyUtil::IntSeq getAdminedClasses(Ice::Int,
                Ice::Int,
                Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());

        virtual ::MyUtil::IntSeq getMemberedClasses(Ice::Int,
                Ice::Int,
                Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());

        virtual MyUtil::IntSeq getFoundedClasses(Ice::Int,
                Ice::Int,
                Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());

        virtual MyUtil::IntSeq getDispatchClasses(Ice::Int,
                const Ice::Current& = Ice::Current());

        virtual MyUtil::Str2StrMapSeq getAllClassInfo(Ice::Int,
                Ice::Int,
                Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());
        virtual MyUtil::Str2StrMapSeq getAdminedClassInfo(Ice::Int,
                Ice::Int,
                Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());
        virtual MyUtil::Str2StrMapSeq getMemberedClassInfo(Ice::Int,
                Ice::Int,
                Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());
        virtual MyUtil::Str2StrMapSeq getFoundedClassInfo(Ice::Int,
                Ice::Int,
                Ice::Int,
                Ice::Int,
                const Ice::Current& = Ice::Current());

        virtual void addMemberedClass(Ice::Int userId, Ice::Int classId, Ice::Int schoolTye, const Ice::Current& = Ice::Current());
        virtual void removeMemberedClass(Ice::Int userId, Ice::Int classId,Ice::Int schoolType, const Ice::Current& = Ice::Current());
        virtual void addAdminedClass(Ice::Int userId, Ice::Int classId, Ice::Int schoolType, const Ice::Current& = Ice::Current());
        virtual void removeAdminedClass(Ice::Int userId, Ice::Int classId, Ice::Int schoolType, const Ice::Current& = Ice::Current());
        virtual void addFoundedClass(Ice::Int userId, Ice::Int classId , Ice::Int schoolType, const Ice::Current& = Ice::Current());
        virtual void removeFoundedClass(Ice::Int userId, Ice::Int classId, Ice::Int schoolType, const Ice::Current& = Ice::Current());

        //virtual void setClassInfo(Ice::Int classId, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
        virtual MyUtil::Str2StrMap getClassInfo(Ice::Int classId, const Ice::Current& = Ice::Current());
        //virtual void removeClassInfo(Ice::Int classId, const Ice::Current& = Ice::Current());


        /*UseredClassManagerI()
        {
            classInfo.setSize(200000);
        }
    private:

      //  MyUtil::EvictorTemplate< Ice::Int, MyUtil::Str2StrMap > classInfo ;*/


};

class UseredClassSetI :
	virtual public UseredClassSet,
	public IceUtil::Mutex
{
public:
    virtual void add(Ice::Int schoolType ,Ice::Int classId, Ice::Int role, const Ice::Current& = Ice::Current());
    virtual void remove(Ice::Int schoolType, Ice::Int classId, const Ice::Current& = Ice::Current());

    virtual Ice::Int size(Ice::Int schoolType , const MyUtil::IntSeq& roles, const Ice::Current& = Ice::Current());
	virtual ::MyUtil::IntSeq getAll(Ice::Int schoolType ,const MyUtil::IntSeq & roles ,Ice::Int begin,
                    Ice::Int limit,
                    const Ice::Current& = Ice::Current());

private:
    MyUtil::Int2IntMap highschoolIds;
    MyUtil::Int2IntMap univIds;

    MyUtil::IntSeq match(Ice::Int schoolType, const MyUtil::IntSeq& roles);
};



};

};

};

};

#endif
