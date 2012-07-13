/*
 * =====================================================================================
 *
 *       Filename:  FeedItemWrapperI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月10日 12时04分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FEEDITEMWRAPPER_I_H__
#define __FEEDITEMWRAPPER_I_H__

#include <set>
#include <vector>
#include "UserFeeds.h"
#include "Singleton.h"
#include <TaskManager.h>
#include "QueryRunner.h"
#include "ObjectCacheI.h"
#include "IceUtil/IceUtil.h"
#include "IceUtil/Mutex.h"

namespace xce {
namespace feed {

using namespace xce::feed;
using namespace std;

const Ice::Int USER_TYPE = 0;
const Ice::Int USER_SHIELD = 1;

const static int TASK_LEVEL_PRELOAD = 1;

const int TYPE[] = {502, 103, 102, 110, 701, 702, 2002, 104, 708, 2401, 8190, 709, 601, 107};

class UserType : public ::IceUtil::Mutex ,
	public ::Ice::Object {
public:
	void put(int type) {
		IceUtil::Mutex::Lock lock(*this);
		types_.insert(type);
	}

	std::vector<int> get() {
		IceUtil::Mutex::Lock lock(*this);
		std::vector<int> result(types_.begin(), types_.end());
		return result;
	}
private:
	std::set<int> types_;
};
typedef IceUtil::Handle<UserType> UserTypePtr;

class UserTypeFactoryI : public MyUtil::ServantFactory {
public:
	UserTypeFactoryI() {}
	virtual ~UserTypeFactoryI() {}

	virtual Ice::ObjectPtr create(int userid) {
		return new UserType;
	}
};

class UserShield : public ::IceUtil::Mutex ,
	public ::Ice::Object {
public:
	void put(int id) {
		IceUtil::Mutex::Lock lock(*this);
		ids_.push_back(id);
	}

	std::vector<int> get() {
		IceUtil::Mutex::Lock lock(*this);
		std::vector<int> result(ids_.begin(), ids_.end());
		return result;
	}
private:
	std::vector<int> ids_;
};
typedef IceUtil::Handle<UserShield> UserShieldPtr;

class UserShieldFactoryI : public MyUtil::ServantFactory {
public:
	UserShieldFactoryI() {}
	virtual ~UserShieldFactoryI() {}

	virtual Ice::ObjectPtr create(int userid);
};

class BuildUserTypeTask : virtual public MyUtil::Task {
public:

	BuildUserTypeTask() : MyUtil::Task(TASK_LEVEL_PRELOAD) {}
	~BuildUserTypeTask() {}
protected:
	virtual void handle();
};

class FeedItemWrapperI : public FeedItemWrapper, public MyUtil::Singleton<FeedItemWrapperI> {
public:
	FeedItemWrapperI();
	~FeedItemWrapperI();

	virtual FeedMetaSeq get(::Ice::Int userid, ::Ice::Int begin, ::Ice::Int limit, const ::Ice::Current& = ::Ice::Current());
 	virtual FeedMetaSeq GetFeeds(::Ice::Int userid, ::Ice::Int begin, ::Ice::Int limit, const MyUtil::IntSeq& types, const MyUtil::IntSeq& ntypes, const ::Ice::Current& = ::Ice::Current());
  virtual FeedMetaSeq GetFeedsByGroupId(int userid, int group, int begin, int limit, const MyUtil::IntSeq& types, const MyUtil::IntSeq& ntypes, const ::Ice::Current& = ::Ice::Current());
  virtual FeedMetaSeq GetFeedsByGroupName(int userid, const string& name, int begin, int limit, const MyUtil::IntSeq& types, const MyUtil::IntSeq& ntypes, const ::Ice::Current& = ::Ice::Current());
 	virtual FeedMetaSeq GetFeedsByFocus(int userid, int begin, int limit, const MyUtil::IntSeq& types, const MyUtil::IntSeq& ntypes, const ::Ice::Current& = ::Ice::Current());
private:
	MyUtil::IntSeq getUserType(int userid);
	MyUtil::IntSeq getUserShield(int userid);

	MyUtil::IntSeq getFixedType();
	
  mysqlpp::StoreQueryResult getPages(int user);
  mysqlpp::StoreQueryResult getMinisite(int user);
  MyUtil::StrSeq createFilter(int user, const MyUtil::IntSeq ntypes);
};

}
}

#endif
