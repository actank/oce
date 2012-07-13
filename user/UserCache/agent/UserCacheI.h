#ifndef __USERCACHEI_H__
#define __USERCACHEI_H__

#include "UserCache.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/TopicI.h"
#include "IceExt/src/ServiceI.h"
//#include "OceCxxAdapter/src/UserCacheLoaderAdapter.h"
#include <QueryRunner.h>

#include "OnlineEventMediator.h"
#include "UserCacheAdapter.h"
using xce::mediator::onlineevent::OFFLINE;
using xce::mediator::onlineevent::ONLINEWEBPAGGER;
using xce::mediator::onlineevent::ONLINEXNT;
using xce::mediator::onlineevent::ONLINEWAP;
using xce::mediator::onlineevent::OFFLINEWAP;

namespace xce {
namespace usercache {

const Ice::Int USER_CACHE = 0;

const std::string USER_CACHE_MANAGER = "M";

const int TASK_LEVEL_UPDATE = 4;
const int TASK_LEVEL_LOAD = 5;
const int TASK_LEVEL_SEND = 6;

const int GENDER_MASK = 0x000000ff;
const int GENDER_MASK_RESULT = 0xff000000;
const int ONLINE_MASK = 0x00ffffff;
const int SHIFT_BIT = 24;

// ========= UserCacheManagerI主类 ========================================
class UserCacheManagerI: virtual public UserCacheManager,
		public MyUtil::Singleton<UserCacheManagerI> {
public:
	UserCacheManagerI():_valid(false), mod_(0) {}
	virtual ~UserCacheManagerI() {}
	virtual ::MyUtil::Str2StrMap getUserCache(::Ice::Int, const Ice::Current&);

	virtual ::MyUtil::Str2StrMapSeq getUserCacheList(const ::MyUtil::IntSeq&,
			const Ice::Current&);

	virtual UserCacheData3Map getUserCacheMap3(const ::MyUtil::IntSeq&,
			const Ice::Current&);

	virtual UserCacheData3Seq getUserCacheList3(const ::MyUtil::IntSeq&,
			const Ice::Current&);

	virtual void load(const ::MyUtil::IntSeq&, const Ice::Current&);

	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
	virtual void setOnline(const ::MyUtil::Int2IntMap& data, const Ice::Current&);
	virtual void reloadSetOnline(const ::MyUtil::Int2IntMap& data, const Ice::Current&);

	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const Ice::Current&);
	virtual void update(const UserCacheUpdateNewDataPtr&, const Ice::Current&);

	void set_mod(int mod) {MCE_INFO("UserCacheManagerI::set_mod -> "<<mod_);mod_ = mod;}
	int mod() const {return mod_;}

	void set_cluster(int cluster) {MCE_INFO("UserCacheManagerI::set_cluster -> "<<cluster); cluster_ = cluster;}
	int cluster() const {return cluster_;}
private:

	UserCacheData3Ptr trans( xce::usercache::UserCachePtr data );
	bool _valid;
	int mod_;
	int cluster_;
	IceUtil::RWRecMutex _validMutex;
};

// ========== Factory部分 =================================================
class UserCacheFactoryI: public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long id);
	virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids);
};

}
}
#endif
