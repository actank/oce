#ifndef __USERADSCACHEI_H__
#define __USERADSCACHEI_H__

#include <UserAdsCache.h>
#include <ServiceI.h>
#include <Allocator.h>
namespace xce {
namespace useradscache {

const ::Ice::Int USER_ADS_CACHE_URL = 0;
const ::std::string USER_ADS_CACHE_URL_MANAGER = "M";
const int TASK_LEVEL_FILL = 1;

class UserAdsCacheManagerI: virtual public UserAdsCacheManager,
                public ::MyUtil::Singleton<UserAdsCacheManagerI> {
public:
	virtual UserAdsCacheUrlDataPtr getUserAdsCacheUrl(::Ice::Int userId, const ::Ice::Current& = ::Ice::Current());
	virtual Int2UserAdsCacheUrlDataMap getUserAdsCacheUrlList(const ::MyUtil::IntSeq& userIds, const ::Ice::Current& = ::Ice::Current());

	virtual UserAdsCacheDataPtr getUserAdsCache(::Ice::Int userId, const ::Ice::Current& = ::Ice::Current());
	virtual Int2UserAdsCacheDataMap getUserAdsCacheList(const ::MyUtil::IntSeq& userIds, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const Ice::Current& = ::Ice::Current());
        virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());

	int mod() const {return _mod;}
	void set_mod(int mod) {_mod = mod;}
private:
	UserAdsCacheManagerI();
        UserAdsCacheUrlDataPtr getUserAdsCacheUrlData(int userId);
        ::MyUtil::ObjectResultPtr getUserAdsCacheUrlData(const ::MyUtil::IntSeq& userIds);

       UserAdsCacheDataPtr getUserAdsCacheData(int userId);
        ::MyUtil::ObjectResultPtr getUserAdsCacheData(const ::MyUtil::IntSeq& userIds);
	UserAdsCacheDataPtr toOld(const UserAdsCacheUrlDataPtr& obj);
	friend class MyUtil::Singleton<UserAdsCacheManagerI>;
	bool _valid;
	IceUtil::RWRecMutex _validMutex;
        int _mod;
};

class FillTask: virtual public ::MyUtil::Task {
public:
        FillTask(const ::MyUtil::ObjectResultPtr& userData);
        virtual void handle();
private:
        ::MyUtil::ObjectResultPtr _userData;
};

class UserAdsCacheFactoryI: public ::MyUtil::ServantFactory {
public:
        virtual ::Ice::ObjectPtr create(::Ice::Long id);
        virtual ::MyUtil::ObjectResultPtr create(const ::MyUtil::LongSeq& ids);
};

class ServiceSchemaI: public ::MyUtil::Descriptor {
public:
        virtual void configure(const ::Ice::PropertiesPtr& props);
};


}
}

#endif
