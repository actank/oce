/*
 * UserCacheClient.h
 *
 *  Created on: 2008-8-11
 *      Author: bbc
 */

#ifndef USERCACHEADAPTER_H_
#define USERCACHEADAPTER_H_

#include <IceUtil/RWRecMutex.h>
#include <UserCache.h>
#include <AdapterI.h>
#include <UserBase.h>

#include "distcache/DistCacheAdapter.h"
#include "UserCache.pb.h"

namespace xce {
namespace usercache {

using xce::userbase::CStateSELECTED;
using xce::userbase::CStateVIPMEMBER;
using xce::userbase::CStateMSNIMME;
using xce::userbase::CStateKEEPUSE;
using xce::userbase::CStateEVERVIPMEMBER;

class InvokeSetOnline : virtual public IceUtil::Thread {
public:
	void invoke(int userId, int online);
	void invoke(const MyUtil::Int2IntMap& data);
protected:
	virtual void run();
private:
	MyUtil::Int2IntMap _cache;
	IceUtil::Monitor<IceUtil::Mutex> _mutex;
};

class UserCache : public Ice::Object {
public:
	UserCache(const xce::distcache::usercache::UserCacheData& data);

	int id() const {return id_;}
	int status() const {return status_;}
	string name() const {return name_;}
	int univ() const {return univ_;}
	string tinyurl() const {return tinyurl_;}
	string gender() const {return gender_;}
	int state() const {return state_;}
	int level() const {return level_;}
	int online() const {return online_;}

	bool IsSelected() const {return state_ & (1<<CStateSELECTED);}
	bool IsVipMember() const {return state_ & (1<<CStateVIPMEMBER);}
	bool IsMSNIMME() const {return state_ & (1<<CStateMSNIMME);}
	bool IsKEEPUSE() const {return state_ & (1<<CStateKEEPUSE);}
	bool IsEverVipMember() const {return state_ & (1<<CStateEVERVIPMEMBER);}
	
private:

	int id_;
	int status_;
	string name_;
	int univ_;
	string tinyurl_;
	string gender_;
	int state_;
	int level_;
	int online_;
};

typedef IceUtil::Handle<UserCache> UserCachePtr;

class UserCacheAdapter : public MyUtil::ReplicatedClusterAdapterI<UserCacheManagerPrx>, public MyUtil::Singleton<UserCacheAdapter> {
public:

	UserCacheAdapter();
	
	void setOnline(int userId, int online);
	void setOnline(const MyUtil::Int2IntMap& data);
	void reloadSetOnline(const MyUtil::Int2IntMap& data);

	std::map<int, UserCachePtr> GetUserCache(const MyUtil::IntSeq& ids);
private:
	InvokeSetOnline _invokeSetOnline;

	friend class InvokeSetOnline;
	void doSetOnline(const MyUtil::Int2IntMap& data);
	void doReloadSetOnline(const MyUtil::Int2IntMap& data);
private:
	xce::distcache::DistCacheAdapterPtr _distCacheAdapterPtr;
};

}
}

#endif /* USERCACHECLIENT_H_ */
