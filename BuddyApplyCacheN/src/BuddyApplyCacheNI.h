#ifndef __BUDDYAPPLYCACHENI_h__
#define __BUDDYAPPLYCACHENI_h__

#include <BuddyApplyCache.h>
#include "ObjectCacheI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <BuddyApplyCacheN/share/BuddyApplyCacheNData.h>

namespace xce {
namespace buddyapplycache {

const static int BUDDYAPPLYCACHE = 0;

const int TASK_LEVEL_FILL=1;
//***************************************************************************

class BuddyApplyCacheManagerNI: virtual public BuddyApplyCacheManagerN {
public:
	BuddyApplyCacheManagerNI(::Ice::Int hardLimit);
	virtual BuddyApplyCacheDataSeqN getBuddyApplyCache(::Ice::Int, ::Ice::Int,
			::Ice::Int, const Ice::Current&);
	virtual BuddyApplyCacheWithTotalN getBuddyApplyCacheWithTotal(::Ice::Int,
			::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual void removeBuddyApplyCache(::Ice::Int, ::Ice::Int,
			const Ice::Current&);
	virtual void clear(::Ice::Int, const Ice::Current&);
	virtual void rebuildBuddyApplyCache(::Ice::Int, const Ice::Current&);
	virtual ::Ice::Int getHardLimit(const Ice::Current&);
	virtual void reload(::Ice::Int, const Ice::Current&);
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current()); 
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const Ice::Current& = ::Ice::Current());
private:
	const ::Ice::Int _hardLimit;
	BuddyApplyCacheNIPtr locateBuddyApplyCache(::Ice::Int id);
	BuddyApplyCacheNIPtr findBuddyApplyCache(::Ice::Int id);
	void checkApplyCache(::Ice::Int id);

	bool _valid;
	IceUtil::RWRecMutex _validMutex;
};

typedef IceUtil::Handle<BuddyApplyCacheManagerNI> BuddyApplyCacheManagerNIPtr;
//***************************************************************************

class BuddyApplyCacheFactoryN: virtual public MyUtil::ServantFactory, virtual public MyUtil::Singleton<BuddyApplyCacheFactoryN> {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
	virtual Ice::ObjectPtr createFromMaster(::Ice::Int id);
	//	virtual Ice::ObjectPtr delegate(::Ice::Long id);
private:
	friend class MyUtil::Singleton<BuddyApplyCacheFactoryN>;
	BuddyApplyCacheFactoryN(){
		;
	}
	class SetGuestRequestCountTask: virtual public MyUtil::Task {
	public:
		SetGuestRequestCountTask(int userId, int newValue) :
			_userId(userId), _newValue(newValue) {
		}
		virtual void handle();
	private:
		int _userId;
		int _newValue;
	};
};

//***************************************************************************

class FillTask: virtual public ::MyUtil::Task 
{ 
	public:
		FillTask(const ::MyUtil::ObjectResultPtr& data):Task(TASK_LEVEL_FILL),_data(data)
	{}
		virtual void handle();

	private:
		::MyUtil::ObjectResultPtr _data;
};  //template FillTask


}

}

#endif
