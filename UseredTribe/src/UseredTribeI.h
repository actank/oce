#ifndef __UseredTribeI_h__
#define __UseredTribeI_h__

#include <UseredTribe.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceBox/IceBox.h>
#include "ServiceI.h"
#include "Channel.h"

#ifndef USERED_TRIBE_API
#   define USERED_TRIBE_API ICE_DECLSPEC_EXPORT
#endif

	extern "C"
	{
		    USERED_TRIBE_API ::IceBox::Service*
				    create(::Ice::CommunicatorPtr communicator);
	}


namespace mop
{

namespace hi
{

namespace svc
{

namespace model
{
const std::string USERED_TRIBE_MANAGER = "MANAGER";	
const Ice::Int ADMINED_TRIBE_SET = 0;
const Ice::Int MEMBERED_TRIBE_SET = 1;
const Ice::Int TRIBE_TINY_VIEW = 2;

class USERED_TRIBE_API UseredTribeServiceI : public MyUtil::ServiceI,
								public MyUtil::Singleton<UseredTribeServiceI>
{
public:	
	UseredTribeSetPtr getAdminedTribeSet(Ice::Int userId);
	UseredTribeSetPtr getMemberedTribeSet(Ice::Int userId);
	TribeTinyViewPtr getTribeTinyView(Ice::Int tribeId);
protected:
	virtual void initialize();
private:
	MyUtil::ChannelPtr _channel;
};

//---------------------------------------------------------------------------
class AdminedTribeSetFactoryI : public MyUtil::ServantFactory<Ice::Int>
{
public:
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};

class MemberedTribeSetFactoryI : public MyUtil::ServantFactory<Ice::Int>
{
public:
	Ice::ObjectPtr create(const Ice::Int& id);	
};

class TribeTinyViewFactoryI : public MyUtil::ServantFactory<Ice::Int>
{
public:
	Ice::ObjectPtr create(const Ice::Int& id);
};

//---------------------------------------------------------------------------
class UseredTribeManagerI : virtual public UseredTribeManager
{
public:
	virtual ::Ice::Int getAdminedTribeCount(::Ice::Int,
					const Ice::Current&);
	virtual ::Ice::Int getMemberedTribeCount(::Ice::Int,
					const Ice::Current&);

    virtual MyUtil::IntSeq getAdminedTribes(::Ice::Int,
                  ::Ice::Int,
                  ::Ice::Int,
                  const Ice::Current&);

    virtual MyUtil::IntSeq getMemberedTribes(::Ice::Int,
                   ::Ice::Int,
                   ::Ice::Int,
                   const Ice::Current&);
                   
    virtual TribeTinyViewSeq getAdminedTribeTinyViews(::Ice::Int,
                  ::Ice::Int,
                  ::Ice::Int,
                  const Ice::Current&);

    virtual TribeTinyViewSeq getMemberedTribeTinyViews(::Ice::Int,
                   ::Ice::Int,
                   ::Ice::Int,
                   const Ice::Current&);

	virtual void addMemberedTribe(::Ice::Int userId, ::Ice::Int tribeId, const Ice::Current&);
	virtual void removeMemberedTribe(::Ice::Int userId, ::Ice::Int tribeId, const Ice::Current&);
	
	virtual void addAdminedTribe(::Ice::Int userId, ::Ice::Int tribeId, const Ice::Current&);
	virtual void removeAdminedTribe(::Ice::Int userId, ::Ice::Int tribeId, const Ice::Current&);
	
	virtual void reloadTribeTinyView(Ice::Int tribeId, const Ice::Current&);
};

class UseredTribeSetI : 
	virtual public UseredTribeSet, 
	public IceUtil::Mutex
{
public:
    virtual void add(Ice::Int tribeId, const Ice::Current&);
    virtual void remove(Ice::Int tribeId, const Ice::Current&);
    	
    virtual ::Ice::Int size(const Ice::Current&);
	virtual ::MyUtil::IntSeq getAll(::Ice::Int,
                    ::Ice::Int,
                    const Ice::Current&);	
};

}

}

}

}

#endif
