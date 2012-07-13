#ifndef __USER_LOGIC_ADAPTER_H__
#define __USER_LOGIC_ADAPTER_H__

#include "UserLogic.h"
#include "Channel.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "Date.h"
#include <boost/lexical_cast.hpp>


namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;

class UserLogicAdapter :
	public MyUtil::ReplicatedClusterAdapterI<UserLogicManagerPrx>, public MyUtil::Singleton<UserLogicAdapter>
	{
public:
	UserLogicAdapter() : MyUtil::ReplicatedClusterAdapterI <UserLogicManagerPrx>("ControllerUserLogic", 120, 300) {}

	void updateUser(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserDesc(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserNetwork(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	
	void insertUserBorn(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserBorn(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserStage(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserStage(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserDoing(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserDoing(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserState(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserState(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserNames(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserNames(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserPassport(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserPassport(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserConfig(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserConfig(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserUrl(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserUrl(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserRight(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserRight(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserTime(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserTime(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserContact(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserContact(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void insertUserFav(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserFav(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());

	void updateUserRegion(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserWorkplace(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserElementarySchool(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserJuniorHighSchool(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserHighSchool(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserCollege(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
	void updateUserUniversity(int hostId, const Str2StrMap& props,
			const Str2StrMap& ctx = Str2StrMap());
protected:
/*	virtual string name() {
		return "UserLogic";
	}
	virtual string endpoints() {
		return "@UserLogic";
	}

	virtual size_t cluster() {
		return 0;
	}
	virtual void initialize();

	UserLogicManagerPrx _userLogicManager;
	UserLogicManagerPrx _userLogicManagerOneway;*/
};

}
;
}
;
}
;
}
;

#endif

