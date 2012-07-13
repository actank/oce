#ifndef __VIPINFO_WRITER_I_H__
#define __VIPINFO_WRITER_I_H__

#include "VipInfo.h"
#include "ServiceI.h"
#include "TempMutex.h"

namespace xce {
namespace vipinfo{

class VipMemberDataWriterI : virtual public VipMemberDataWriter, public MyUtil::Singleton<VipMemberDataWriterI>
{

public:

	VipMemberDataWriterI();
	
	virtual void createVipMemberData(const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void setVipMemberData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void reload(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

	bool isValid();
private:

	xce::tempmutex::TempMutexManagerPtr _tempMutexManager;
	IceUtil::Mutex _mutex;
	bool _valid;
};

class Icon2UrlDataWriterI : virtual public Icon2UrlDataWriter, public MyUtil::Singleton<Icon2UrlDataWriterI>
{

public:

	Icon2UrlDataWriterI();
	
	virtual void createIcon2UrlData(const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void setIcon2UrlData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void reload(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

	bool isValid();
private:

	xce::tempmutex::TempMutexManagerPtr _tempMutexManager;
	IceUtil::Mutex _mutex;
	bool _valid;
};

class HatDataWriterI : virtual public HatDataWriter, public MyUtil::Singleton<HatDataWriterI>
{

public:

	HatDataWriterI();

	virtual void createHatData(const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void setHatData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void reload(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

	bool isValid();
private:

	xce::tempmutex::TempMutexManagerPtr _tempMutexManager;
	IceUtil::Mutex _mutex;
	bool _valid;
};

};
};


#endif

