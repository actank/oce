#ifndef __VIPINFO_READER_I_H__
#define __VIPINFO_READER_I_H__

#include "VipInfo.h"
#include "user/VipInfo/share/VipInfoDataI.h"
#include "TaskManager.h"
#include "ServiceI.h"


namespace xce {
namespace vipinfo{

//Task level config

const int TASK_LEVEL_FILL = 1;

//Cache category config

const Ice::Int CACHE_VIP_MEMBER = 0;
const Ice::Int CACHE_ICON2URL = 1;
const Ice::Int CACHE_HAT = 2;

class IdsFilter : public MyUtil::Singleton<IdsFilter>
{
	public:

	static void filter(const MyUtil::LongSeq& ids, const MyUtil::ObjectResultPtr& data, MyUtil::LongSeq& filterIds);
};

class VipMemberDataReaderI : virtual public VipMemberDataReader, public MyUtil::Singleton<VipMemberDataReaderI>
{

public:
	
	VipMemberDataReaderI();
	virtual VipMemberDataSeqMap getVipMemberDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
	virtual VipMemberDataNSeqMap getVipMemberDataNSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
	virtual void setVipMemberData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

public:

	VipMemberMemDataIPtr locate(int id);
	std::map<int,VipMemberDataSeq> locate(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types);
	std::map<int,VipMemberDataNSeq> locateN(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types);
	bool isValid();

private:

	IceUtil::Mutex _mutex;
	bool _valid;
};

class Icon2UrlDataReaderI : virtual public Icon2UrlDataReader, public MyUtil::Singleton<Icon2UrlDataReaderI>
{

public:

	Icon2UrlDataReaderI();
	virtual MyUtil::Int2StrMap getIcon2UrlDataMap(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
	virtual void setIcon2UrlData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& data, const ::Ice::Current& = ::Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

public:

	Icon2UrlDataIPtr locate(int id);
	MyUtil::Int2StrMap locate(const MyUtil::IntSeq& ids);
	bool isValid();

private:

	IceUtil::Mutex _mutex;
	bool _valid;
};

class HatDataReaderI : virtual public HatDataReader, public MyUtil::Singleton<HatDataReaderI>
{

public:

	HatDataReaderI();
	virtual HatDataMap getHatDataMap(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
	virtual void setHatData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& data, const ::Ice::Current& = ::Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

public:

	HatDataIPtr locate(int id);
	HatDataMap locate(const MyUtil::IntSeq& ids);
	bool isValid();

private:

	IceUtil::Mutex _mutex;
	bool _valid;
};

class VipIconDataReaderI : virtual public VipIconDataReader, public MyUtil::Singleton<VipIconDataReaderI>
{

public:

	virtual VipIconDataSeqMap getVipIconDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
	virtual VipFeedDataSeqMap getVipFeedDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
};

class VipFullDataReaderI : virtual public VipFullDataReader, public MyUtil::Singleton<VipFullDataReaderI>
{

public:

	virtual VipFullDataSeqMap getVipFullDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
	virtual VipFullDataNSeqMap getVipFullDataNSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& = Ice::Current());
};

template <class T,class P> class FillTask: virtual public ::MyUtil::Task 
{

public:

	FillTask(const Ice::Int identifier, const ::MyUtil::ObjectResultPtr& userData);
	virtual void handle();

private:

	Ice::Int _identifier;
	MyUtil::ObjectResultPtr _userData;
};

};
};


#endif

