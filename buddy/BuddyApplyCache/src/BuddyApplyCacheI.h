#ifndef __OfferFriendsI_h__
#define __OfferFriendsI_h__

#include <BuddyApplyCache.h>
#include "ObjectCacheI.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace buddyapplycache {

const static int BUDDYAPPLYCACHE = 0;

//***************************************************************************

class BuddyApplyCache: virtual public Ice::Object,
		virtual public IceUtil::Mutex {
public:
	friend class BuddyApplyCacheResultHandlerI;
	BuddyApplyCache(::Ice::Int ownerId);
	BuddyApplyCacheDataNSeq get(int begin, int limit) const;
	void add(BuddyApplyCacheDataN data);
	void remove(int applicant);
	void clear();
	void sort();
	int getTotalCount();
	bool hasExtra();
	void setExtra(bool extra);
	void push_front(MyUtil::Str2StrMap& props);
private:
	BuddyApplyCacheDataNSeq _buddyApplyCaches;
	const ::Ice::Int _ownerId;
	bool _extra;
	//time_t _createTime;
};
typedef IceUtil::Handle<BuddyApplyCache> BuddyApplyCachePtr;

//bool greater(const BuddyApplyCacheDataN& b1, const BuddyApplyCacheDataN& b2) {
//	return b1.time > b2.time;
//}

//***************************************************************************

class BuddyApplyCacheManagerI: virtual public BuddyApplyCacheManager,
        public MyUtil::Singleton<BuddyApplyCacheManagerI> {
public:
	void bootstrap(const string&,int,int);
	virtual BuddyApplyCacheDataNSeq getBuddyApplyCache(::Ice::Int, ::Ice::Int,
			::Ice::Int, const Ice::Current&);
	virtual BuddyApplyCacheWithTotal getBuddyApplyCacheWithTotal(::Ice::Int,
			::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual void removeBuddyApplyCache(::Ice::Int, ::Ice::Int,
			const Ice::Current&);
	virtual void clear(::Ice::Int, const Ice::Current&);
	virtual void rebuildBuddyApplyCache(::Ice::Int, MyUtil::Str2StrMap& props, const Ice::Current&);
	virtual ::Ice::Int getHardLimit(const Ice::Current&);
private:
	::Ice::Int _hardLimit;
        string _endpoint;
        int _mod;
        friend class MyUtil::Singleton<BuddyApplyCacheManagerI>;
	BuddyApplyCacheManagerI()
		: _hardLimit(100) {}
	BuddyApplyCachePtr locateBuddyApplyCache(::Ice::Int id);
	BuddyApplyCachePtr findBuddyApplyCache(::Ice::Int id);
};

//***************************************************************************

class BuddyApplyCacheFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
	//	virtual Ice::ObjectPtr delegate(::Ice::Long id);
private:
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

class BuddyApplyCacheResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BuddyApplyCacheResultHandlerI(const BuddyApplyCachePtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	BuddyApplyCachePtr _result;
};

//***************************************************************************

class NoApplyTask : virtual public MyUtil::Task {
public:
	NoApplyTask(const Ice::Int userId):_userId(userId){}
	virtual void handle();
private:
	Ice::Int _userId;
};


}

}

#endif
