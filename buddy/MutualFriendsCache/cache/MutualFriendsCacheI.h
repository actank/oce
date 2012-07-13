#ifndef _MUTUAL_FRIENDS_CACHE_I_H_
#define _MUTUAL_FRIENDS_CACHE_I_H_
#include "MutualFriendsCache.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <string>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/ServiceI.h"

namespace xce {
namespace buddy {
const int FANS_CACHE_DATA = 0;
//const int MUTUAL_CACHE_DATA = 1;

const int TASK_LEVEL_FANS_CACHE_FILL = 1;
const int TASK_LEVEL_FANS_CACHE_RELOAD = 2;
const int TASK_LEVEL_FANS_CACHE_REMOVE = 3;
const int TASK_LEVEL_FANS_CACHE_ADD = 4;

//----------------------------------FansDataI-----------------------------------
class FansDataI: virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	FansDataI() {
	}

	FansDataI(MyUtil::IntSeq& ids) {
		_ids.swap(ids);
		_timestamp = time(0);
	}

	FansDataI(FansObjectPtr fansObj) {
		fansObj->fans.swap(_ids);
		_timestamp = time(0);
	}

	virtual ~FansDataI() {
		;
	}

	MyUtil::IntSeq get(int begin, int limit) {
		IceUtil::Mutex::Lock lock(*this);
		if (limit < 0) {
			return _ids;
		}
		if (((unsigned int) begin >= _ids.size()) || (begin < 0)) {
			return MyUtil::IntSeq();
		}
		if ((unsigned int) (begin + limit) >= _ids.size()) {
			return MyUtil::IntSeq(_ids.begin() + begin, _ids.end());
		} else {
			return MyUtil::IntSeq(_ids.begin() + begin, _ids.begin() + begin + limit);
		}
	}

	bool find(int id) {

		IceUtil::Mutex::Lock lock(*this);
		MyUtil::IntSeq::iterator it = lower_bound(_ids.begin(), _ids.end(), id);
		if (it != _ids.end() && *it == id) {
			return true;
		} else {
			return false;
		}
	}

	void add(int id) {
		IceUtil::Mutex::Lock lock(*this);
		MyUtil::IntSeq::iterator it = lower_bound(_ids.begin(), _ids.end(), id);
		if (it != _ids.end() && *it == id) {
		} else {
			_ids.insert(it, id);
		}
	}

	void remove(int id) {
		IceUtil::Mutex::Lock lock(*this);
		MyUtil::IntSeq::iterator it = lower_bound(_ids.begin(), _ids.end(), id);
		if (it != _ids.end() && *it == id) {
			_ids.erase(it);
		}
	}

	void clear() {
		IceUtil::Mutex::Lock lock(*this);
		_ids.clear();
	}

	int getCount() {
		IceUtil::Mutex::Lock lock(*this);
		return _ids.size();
	}

	time_t getTimestamp() {
		IceUtil::Mutex::Lock lock(*this);
		return _timestamp;
	}

	void setTimestamp(time_t timestamp) {
		IceUtil::Mutex::Lock lock(*this);
		_timestamp = timestamp;
	}

private:
	MyUtil::IntSeq _ids;
	time_t _timestamp;
};
typedef IceUtil::Handle<FansDataI> FansDataIPtr;

//--------------------------MutualFriendsCacheManagerI--------------------------
class MutualFriendsCacheManagerI: virtual public MutualFriendsCacheManager, virtual public MyUtil::Singleton<MutualFriendsCacheManagerI> {
public:
	virtual MyUtil::IntSeq getMutualFriends(int uid, const MyUtil::GlobalIdPtr& gid, int begin, int limit, const Ice::Current&);
	virtual MyUtil::IntSeq getUnMutualFriends(int uid, const MyUtil::GlobalIdPtr& gid, int begin, int limit, const Ice::Current&);
	virtual int getMutualFriendsCount(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current&);
	virtual int getUnMutualFriendsCount(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current&);
	virtual int getFansCount(const MyUtil::GlobalIdPtr& gid, const Ice::Current&);
	virtual bool isFans(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current&);
	virtual MyUtil::IntSeq getFans(const MyUtil::GlobalIdPtr& gid, int begin, int limit, const Ice::Current&);

	virtual bool isValid(const Ice::Current&);
	virtual void setValid(bool valid, const Ice::Current&);
	virtual void add(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current&);
	virtual void remove(int uid, const MyUtil::GlobalIdPtr& gid, const Ice::Current&);
	virtual void setData(const ::MyUtil::ObjectResultPtr& data, const Ice::Current&);

	void bootstrap(const string& endpoint, int mod);

private:
	int _mod;
	bool _isValid;
	IceUtil::RWRecMutex _validMutex;
	string _endpoint;

	friend class MyUtil::Singleton<MutualFriendsCacheManagerI>;
	MutualFriendsCacheManagerI() {
	}
	MyUtil::IntSeq getFriendsAsc(int uid);
	MyUtil::IntSeq getAllFansAsc(const MyUtil::GlobalIdPtr& gid);
};

//------------------------------FansCacheFactory--------------------------------
class FansCacheFactory: virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long id);
};

//-----------------------------------Task---------------------------------------
class FillTask: virtual public MyUtil::Task {
public:
	FillTask(const MyUtil::ObjectResultPtr& fansMap, ::Ice::Int level = TASK_LEVEL_FANS_CACHE_FILL) :
		MyUtil::Task(level), _fansMap(fansMap) {
	}
	virtual void handle();
private:
	MyUtil::ObjectResultPtr _fansMap;
};

class ReloadTask: virtual public MyUtil::Task {
public:
	ReloadTask(Ice::Long gid, Ice::Int level = TASK_LEVEL_FANS_CACHE_RELOAD) :
		MyUtil::Task(level), _gid(gid) {
	}
	virtual void handle();
private:
	Ice::Long _gid;

};

class AddTask: virtual public MyUtil::Task {
public:
	AddTask(int uid, Ice::Long gid, ::Ice::Int level = TASK_LEVEL_FANS_CACHE_ADD) :
		Task(level), _uid(uid), _gid(gid) {
	}
protected:
	virtual void handle();
private:
	int _uid;
	Ice::Long _gid;
};

class RemoveTask: virtual public MyUtil::Task {
public:
	RemoveTask(int uid, Ice::Long gid, ::Ice::Int level = TASK_LEVEL_FANS_CACHE_REMOVE) :
		Task(level), _uid(uid), _gid(gid) {
	}
protected:
	virtual void handle();
private:
	int _uid;
	Ice::Long _gid;
};

//--------------------------------------------------------------------------
class ServiceSchemaI: public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

}//end of namespace buddy
}//end of namespace xce
#endif
