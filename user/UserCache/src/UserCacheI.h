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
	void setOnline(int userId, int online);
	void reloadSetOnline(int userId, int online);

	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const Ice::Current&);
	virtual void update(const UserCacheUpdateNewDataPtr&, const Ice::Current&);

	void set_mod(int mod) {MCE_INFO("UserCacheManagerI::set_mod -> "<<mod_);mod_ = mod;}
	int mod() const {return mod_;}

	void set_cluster(int cluster) {MCE_INFO("UserCacheManagerI::set_cluster -> "<<cluster); cluster_ = cluster;}
	int cluster() const {return cluster_;}
private:
	MyUtil::ObjectResultPtr getUserCacheData(const MyUtil::IntSeq& userIds);

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

class UserCacheEntry;

typedef IceUtil::Handle<UserCacheEntry> UserCacheEntryPtr;

class UserCacheEntry : public Ice::Object {
public:
	UserCacheEntry(const UserCacheEntryPtr& entry);
	UserCacheEntry(const UserCacheData3Ptr& data);

	MyUtil::Str2StrMap GetUserCacheEntryProp() const;

	UserCacheData3Ptr GetUserCacheData3NoRebuild( MyUtil::IntSeq& rebuildIds ) const;

	void set_online(int online) {
		if (OFFLINE == online) {
			gender_online_ &= (~(1 << (ONLINEWEBPAGGER - 1)));
			gender_online_ &= (~(1 << (ONLINEXNT - 1)));
		} else if (online > 0) {
			gender_online_ |= (1 << (online - 1));
		} else if (online < 0) {
			gender_online_ &= (~(1 << (std::abs(online) - 1)));
		} else {
			MCE_WARN("set_online online is wrang, online=" << online);
		}

	}

	void reload_set_online(int online) {
		gender_online_ &= GENDER_MASK_RESULT;
		gender_online_ |= ONLINE_MASK&online;
	}

	void set_lastonlinetime(int time) {lastonlinetime_ = time;}
	void set_id(int id) {id_ = id;}
	void set_gender(int gender) {int gender_online = 0; gender_online |= (gender_online_ & ONLINE_MASK); gender_online |= ((GENDER_MASK&gender) << SHIFT_BIT); gender_online_ = gender_online;}
	void set_status(int status) {status_ = status;}
	void set_univ(int univ) {univ_ = univ;}
	void set_state(int state) {state_ = state;}
	void set_level(int level) {level_ = level;}
	void set_name(const string& name) {name_ = name;}
	void set_tinyUrl(const string& tinyUrl) {tinyUrl_ = tinyUrl;}
	
private:
	int id_;
	int status_;
	int univ_;
	int state_;
	int level_;
	string name_;
	string tinyUrl_;
	int gender_online_;
	int lastonlinetime_;
};

// =========================================================================

class UpdateTask: public MyUtil::Task {
public:
	UpdateTask(const UserCacheUpdateNewDataPtr& data, const Ice::Current& current):Task(TASK_LEVEL_UPDATE),data_(data), current_(current){}
	virtual void handle();
private:
	void UpdateData(UserCacheEntryPtr& data, const Byte2StrMap&);
	UserCacheUpdateNewDataPtr data_;
	Ice::Current current_;
};

}
}
#endif
