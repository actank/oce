#ifndef _BIRTHDAY_RECOMMEND_CACHEI_H_
#define _BIRTHDAY_RECOMMEND_CACHEI_H_

#include <BirthdayRecommendCache.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "ServiceI.h"
#include <ObjectCacheI.h>
#include <QueryRunner.h>

#include "RecommendDBHelper.h"

namespace xce {
namespace socialgraph {

const int BIRTH_RECOMMEND_DATA = 1;
const int INTERVAL = 21 * 3600;     //数据加载到2点21小时后过期

//****************************************************************************************

class BirthdayRecommendData : virtual public Ice::Object, virtual IceUtil::Mutex {
public:
	BirthdayRecommendData(int hostId, const MyUtil::IntSeq& birthdayRecommendList) :
		_hostId(hostId), _birthdayRecommendList(birthdayRecommendList), _timestamp(time(NULL)) { };
	MyUtil::IntSeq GetBirthdayRecommendList(int limit = -1);
	int GetBirthdayRecommend();
	void RemoveBirthdayRecommend(int friendId);

private:
	bool CheckTimeout();

	int _hostId;	
	MyUtil::IntSeq _birthdayRecommendList;
	time_t _timestamp;
};
typedef IceUtil::Handle<BirthdayRecommendData> BirthdayRecommendDataPtr;

//****************************************************************************************

class BirthdayRecommendCacheI : virtual public BirthdayRecommendCache,
		virtual public MyUtil::Singleton<BirthdayRecommendCacheI> {	
public:
	virtual void Set(::Ice::Int, const MyUtil::IntSeq&, const Ice::Current&);
	virtual ::Ice::Int Get(::Ice::Int, const Ice::Current&);
	virtual ::Ice::Int GetAndRemove(::Ice::Int, const Ice::Current&);
	virtual MyUtil::IntSeq GetList(::Ice::Int, ::Ice::Int, const Ice::Current&);
	virtual void Remove(::Ice::Int, ::Ice::Int, const Ice::Current&);
	
private:
	BirthdayRecommendDataPtr FindBirthdayRecommendData(::Ice::Int id);
	friend class MyUtil::Singleton<BirthdayRecommendCacheI>;
};

//****************************************************************************************

class BirthdayRecommendFactory : virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(::Ice::Int id);
};

//****************************************************************************************

class RemoveTask : virtual public MyUtil::Task {
public:
	RemoveTask(Ice::Int ownerId, Ice::Int friendId) :
		_ownerId(ownerId), _friendId(friendId) { };	
	virtual void handle();

private:
	Ice::Int _ownerId;
	Ice::Int _friendId;
};

}
}

#endif
