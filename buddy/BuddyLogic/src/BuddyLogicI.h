#ifndef __BUDDY_CORE_LOGICI_H__
#define __BUDDY_CORE_LOGICI_H__

#include <BuddyCommon.h>
#include <Buddy.h>
#include <IceUtil/IceUtil.h>
#include "QueryRunner.h"
#include "ServiceI.h"

namespace xce {

namespace buddy {

const static string BUDDY_CORE_LOGIC = "BCL";

/* @brief	接口总类
 */
class BuddyLogicI : virtual public BuddyLogic, virtual public MyUtil::Singleton<BuddyLogicI> {
public:

	virtual void addApply(const Relationship& apply, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp, const ApplyInfo* applyInfo, const Ice::Current&);

	virtual void acceptApply(const Relationship& apply, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp,const Ice::Current&);

	virtual void denyApply(const Relationship& apply, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp,const Ice::Current&);

	virtual void removeFriend(const Relationship& buddy, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp,const Ice::Current&);

	virtual void addBlock(const Relationship& block, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp,const Ice::Current&);

	virtual void removeBlock(const Relationship& block, const BuddyRelationPtr& localbrp,
                const BuddyRelationPtr& remotebrp,const Ice::Current&);

	// virtual void run();
private:
	friend class MyUtil::Singleton<BuddyLogicI>;
};

// ------------------------------------------------
class DelInSqlTask : public MyUtil::Task {
public:
	DelInSqlTask(const string& source, int from, int to):
		_source(source),
		_from(from),
		_to(to)  {
	}
	virtual void handle();
private:
	string _source;
	int _from;
	int _to;
};

// -------------------------------------------------
class PresenceSubscribeTask : public MyUtil::Task {
public:
	PresenceSubscribeTask(const string& id, int from, int to, const string& status):
		_id(id),
		_from(from),
		_to(to),
		_status(status) {
	}
	virtual void handle();
private:
	string _id;
	int _from;
	int _to;
	string _status;
};

class PresenceSubscribedTask : public MyUtil::Task {
public:
	PresenceSubscribedTask(const string& id, int from, int to):
		_id(id),
		_from(from),
		_to(to) {
	}
	virtual void handle();
private:
	string _id;
	int _from;
	int _to;
};

class PresenceUnsubscribedTask : public MyUtil::Task {
public:
	PresenceUnsubscribedTask(const string& id, int from, int to):
		_id(id),
		_from(from),
		_to(to) {
	}
	virtual void handle();
private:
	string _id;
	int _from;
	int _to;
};

// -------------------------------------------

/* @brief RebuildBuddyApplyCacheTask
 */
class RebuildBuddyApplyCacheTask : public MyUtil::Task {
public:
	RebuildBuddyApplyCacheTask(const int id, const MyUtil::Str2StrMap& props) :
		_id(id), _props(props) {
	}
	virtual void handle();
private:
	int _id;
	MyUtil::Str2StrMap _props;
};

class RemoveBuddyApplyCacheTask : public MyUtil::Task {
public:
	RemoveBuddyApplyCacheTask(int id, int applicant) :
		_id(id),
		_applicant(applicant) {
	}
	virtual void handle();
private:
	int _id;
	int _applicant;
};

class IncUserFriendCountTask : public MyUtil::Task {
public:
	IncUserFriendCountTask(int id) :
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

class DecUserFriendCountTask : public MyUtil::Task {
public:
	DecUserFriendCountTask(int id) :
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

class RemoveBuddyNotifyTask : public MyUtil::Task {
public:
	RemoveBuddyNotifyTask(int from, int to) :
		_from(from),
		_to(to) {
	}
	virtual void handle();
private:
	int _from;
	int _to;
};

// --------------------------------------------
class IncUserCountGuestRequestTask : public MyUtil::Task {
public:
	IncUserCountGuestRequestTask(int id):
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

class DecUserCountGuestRequestTask : public MyUtil::Task {
public:
	DecUserCountGuestRequestTask(int id):
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

// -------------------------------------------
class RemoveCommonFriendTask : public MyUtil::Task {
public:
	RemoveCommonFriendTask(int userId, int friendId):
		_userId(userId),
		_friendId(friendId) {
	}
	virtual void handle();
private:
	int _userId;
	int _friendId;
};

class RebuildCommonFriendTask : public MyUtil::Task {
public:
	RebuildCommonFriendTask(int userId):
		_userId(userId) {
	}
	virtual void handle();
private:
	int _userId;
};

// ---------------------------------------
class RemoveOfferTask : public MyUtil::Task {
public:
	RemoveOfferTask(int userId, int offerId):
		_userId(userId),
		_offerId(offerId) {
	}
	virtual void handle();
private:
	int _userId;
	int _offerId;
};

// ---------------------------------------
class AddBuddyOnlineStatTask : public MyUtil::Task {
public:
	AddBuddyOnlineStatTask(int host, int guest) :
		_host(host),
		_guest(guest) {
	}
	virtual void handle();
private:
	int _host;
	int _guest;

};

class RemoveBuddyOnlineStatTask : public MyUtil::Task {
public:
	RemoveBuddyOnlineStatTask(int host, int guest) :
		_host(host),
		_guest(guest) {
	}
	virtual void handle();
private:
	int _host;
	int _guest;

};

// ---------------------------------------
class DeliverSupplyTask : public MyUtil::Task {
public:
	DeliverSupplyTask(int supplier, int accepter, const Ice::Context& ctx):
		_supplier(supplier),
		_accepter(accepter),
		_ctx(ctx) {
	}
	virtual void handle();
private:
	int _supplier;
	int _accepter;
	Ice::Context _ctx;
};

// ---------------------------------------
class RemoveBuddyGroupTask : public MyUtil::Task {
public:
	RemoveBuddyGroupTask(int from, int to) :
		_from(from),
		_to(to) {
	}
	virtual void handle();
private:
	int _from;
	int _to;
};

// ---------------------------------------
class ReloadBlockUserTask : public MyUtil::Task {
public:
	ReloadBlockUserTask(int id):
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

// ---------------------------------------
class RemoveRecommendFriendTask : public MyUtil::Task {
public:
	RemoveRecommendFriendTask(int ownerId, int otherId):
		_ownerId(ownerId),_otherId(otherId) {
	}
	virtual void handle();
private:
	int _ownerId;
	int _otherId;
};

// ---------------------------------------
class RebuildFriendDistTask : public MyUtil::Task {
public:
	RebuildFriendDistTask(int id):
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

class LoadBuddyCacheTask : public MyUtil::Task {
public:
	LoadBuddyCacheTask(int id):
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

class LoadBuddyCacheByIdTask : public MyUtil::Task {
public:
	LoadBuddyCacheByIdTask(int id):
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

// ---------------------------------------
class AddRecentlyOnlineBuddyTask : public MyUtil::Task {
public:
	AddRecentlyOnlineBuddyTask(int userid,int friendid):
		_userid(userid),
		_friendid(friendid){
	}
	virtual void handle();
private:
	int _userid;
	int _friendid;
};

class RemoveRecentlyOnlineBuddyTask : public MyUtil::Task {
public:
	RemoveRecentlyOnlineBuddyTask(int userid,int friendid):
		_userid(userid),
		_friendid(friendid){
	}
	virtual void handle();
private:
	int _userid;
	int _friendid;
};

class RebuildRecentlyOnlineBuddyTask : public MyUtil::Task {
public:
	RebuildRecentlyOnlineBuddyTask(int id):
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

// ---------------------------------------
class AddBuddyCacheByNameTask : public MyUtil::Task {
public:
	AddBuddyCacheByNameTask(int userid,int friendid):
		_userid(userid),
		_friendid(friendid){
	}
	virtual void handle();
private:
	int _userid;
	int _friendid;
};

class RemoveBuddyCacheByNameTask : public MyUtil::Task {
public:
	RemoveBuddyCacheByNameTask(int userid,int friendid):
		_userid(userid),
		_friendid(friendid){
	}
	virtual void handle();
private:
	int _userid;
	int _friendid;
};

class LoadBuddyCacheByNameTask : public MyUtil::Task {
public:
	LoadBuddyCacheByNameTask(int id):
		_id(id) {
	}
	virtual void handle();
private:
	int _id;
};

class LoadBuddyRelationTask : public MyUtil::Task {
public:
        LoadBuddyRelationTask(int id,BuddyRelationPtr brp):
                _id(id),_brp(brp) {
        }
        virtual void handle();
private:
	int _id;
        BuddyRelationPtr _brp;
};

class LoadBuddyByIdTask : public MyUtil::Task {
public:
        LoadBuddyByIdTask(int id, MyUtil::IntSeq& friends):
                _id(id),_friends(friends) {
        }
        virtual void handle();
private:
        int _id;
        MyUtil::IntSeq _friends;
};

}
}

#endif
