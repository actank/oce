#ifndef _GENERALFILTER_I_H_
#define _GENERALFILTER_I_H_

#include "GeneralFilter.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Channel.h"
#include "Date.h"
#include "StepFootprint.h"
#include "StepMessage.h"
#include "StepLetter.h"
#include "StepDiaryAndReply.h"
#include "StepInvitation.h"
#include "StepAddAFriend.h"
#include "StepJoinAGroup.h"
#include "StepGreet.h"
#include "Step.h"

#include <Ice/Ice.h>
#include <Ice/Exception.h>
#include <Freeze/Freeze.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include <boost/lexical_cast.hpp>

using namespace MyUtil;

#ifndef GENERALFILTER_API
#define GENERALFILTER_API ICE_DECLSPEC_EXPORT
#endif

extern "C"
{
    GENERALFILTER_API ::IceBox::Service*
    create(::Ice::CommunicatorPtr communicator);
}

namespace mop
{
namespace hi
{
namespace svc
{
namespace gf
{

class GeneralFilterManagerI;
typedef IceUtil::Handle<GeneralFilterManagerI> GeneralFilterManagerIPtr;
//---------------------------------------------------------------------------
class GENERALFILTER_API GeneralFilterServiceI : public ServiceI,
    public Singleton<GeneralFilterServiceI>
{
public:
    virtual void initialize();

   	StepPtr getStep(Ice::Int userId,Ice::Int type);
    

};

//---------------------------------------------------------------------------
class GeneralFilterFactoryI: public ServantFactory<Ice::Int>
{
private:
	int type;
public:
	GeneralFilterFactoryI(int type);
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};
/*
class FootprintFilterFactoryI : public ServantFactory<Ice::Int>
{
public:
    virtual Ice::ObjectPtr create(const Ice::Int& id);
};
class MessageFilterFactoryI : public ServantFactory<Ice::Int>
{
public:
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};
class LetterFilterFactoryI : public ServantFactory<Ice::Int>
{
public:
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};
class DiaryAndReplyFilterFactoryI : public ServantFactory<Ice::Int>
{
public:
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};
class InvitationFilterFactoryI : public ServantFactory<Ice::Int>
{
public:
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};
class AddAFriendFilterFactoryI : public ServantFactory<Ice::Int>
{
public:
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};
class JoinAGroupFilterFactoryI : public ServantFactory<Ice::Int>
{
public:
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};
class GreetFilterFactoryI : public ServantFactory<Ice::Int>
{
public:
	virtual Ice::ObjectPtr create(const Ice::Int& id);
};
*/
//---------------------------------------------------------------------------
class GeneralFilterManagerI : virtual public GeneralFilterManager,
	virtual public IceUtil::AbstractMutexI<IceUtil::Mutex>
{
public:
	virtual int passFilter(const GeneralInformationPtr &fpt,const Ice::Current& = Ice::Current());
	virtual int filterControl(const MyUtil::Str2IntMap &cmd, const Ice::Int type,const Ice::Current& = Ice::Current());
private:
	int visitHomepage(const GeneralInformationPtr & fpt);
	int controlFootprint(const MyUtil::Str2IntMap &cmd);
	int sendMessage(const GeneralInformationPtr & fpt);
	int controlMessage(const MyUtil::Str2IntMap  &cmd);
	int sendLetter(const GeneralInformationPtr & fpt);
	int controlLetter(const MyUtil::Str2IntMap &cmd);
	int diaryAndReply(const GeneralInformationPtr & fpt);
	int controlDiaryAndReply(const MyUtil::Str2IntMap &cmd);
	int sendInvitation(const GeneralInformationPtr & fpt);
	int controlInvitation(const MyUtil::Str2IntMap &cmd);
	int addAFriend(const GeneralInformationPtr & fpt);
	int controlFriend(const MyUtil::Str2IntMap &cmd);
	int joinAGroup(const GeneralInformationPtr & fpt);
	int controlGroup(const MyUtil::Str2IntMap &cmd);	
	int sendGreet(const GeneralInformationPtr & fpt);
	int controlGreet(const MyUtil::Str2IntMap &cmd);
};




};
};
};
};
#endif
