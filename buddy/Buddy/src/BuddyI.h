#ifndef __BuddyI_h__
#define __BuddyI_h__

#include <BuddyCommon.h>
#include <Buddy.h>
#include "ServiceI.h"
#include "../../BuddyEntry/src/BuddyEntry.h"
#include "../../BuddyEntry/src/BuddyRelation.h"
namespace xce {

namespace buddy {

const static string DB_BUDDY_FRIEND = "Buddy_friend";
const static string DB_BUDDY_APPLY = "Buddy_apply";
const static string DB_BUDDY_BLOCK = "Buddy_block";
const int TABLE_FRIEND_COUNT = 100;
const static string TABLE_FRIEND = "relation_";
const static string TABLE_APPLY = "buddyapply";
const static string TABLE_BLOCK = "relation_block";

const static int BUDDY_RELATION = 0;

const static int TASK_LEVEL_DUPCLEANUP = 1;
const static int TASK_LEVEL_CACHEUSERCOUNT = 2;

class BuddyManagerI: virtual public BuddyManager,virtual public MyUtil::Singleton<BuddyManagerI> {
public:

	virtual void addApply(const Relationship&, const ApplyInfo& info,
			const Ice::Current& = Ice::Current());

	virtual OpState addApplyWithState(const Relationship&, const ApplyInfo& info,
			const Ice::Current& = Ice::Current());

	virtual void addApplyBatch(const ApplyshipMap&, const ApplyInfoMap&,
			const Ice::Current& = Ice::Current());

	virtual OpStateMap addApplyWithStateBatch(const ApplyshipMap&,
			const ApplyInfoMap&, const Ice::Current& = Ice::Current());

	virtual void acceptApply(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual OpState acceptApplyWithState(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual void acceptAllApplied(::Ice::Int, const Ice::Current& =
			Ice::Current());

	virtual void denyApply(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual OpState denyApplyWithState(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual void denyAllApplied(::Ice::Int, const Ice::Current& =
			Ice::Current());

	virtual ::Ice::Int getAppliedCount(::Ice::Int, const Ice::Current& =
			Ice::Current());

	virtual ::MyUtil::IntSeq getAppliedSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());

	virtual ::Ice::Int getApplyCount(::Ice::Int, const Ice::Current& =
			Ice::Current());

	virtual ::MyUtil::IntSeq getApplySeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());
	virtual ::MyUtil::Int2IntSeqMap getApplySeqBatch(const ::MyUtil::IntSeq&,
			const Ice::Current& = Ice::Current());

	virtual void removeFriend(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual OpState removeFriendWithState(const Relationship&,
			const Ice::Current& = Ice::Current());

	virtual ::Ice::Int getFriendCount(::Ice::Int, const Ice::Current& =
			Ice::Current());

	virtual ::MyUtil::IntSeq getFriendSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());
	virtual ::MyUtil::Int2IntSeqMap getFriendSeqBatch(const ::MyUtil::IntSeq&,
			const Ice::Current& = Ice::Current());

	virtual void addBlock(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual OpState addBlockWithState(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual void removeBlock(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual OpState removeBlockWithState(const Relationship&, const Ice::Current& =
			Ice::Current());

	virtual ::Ice::Int getBlockCount(::Ice::Int, const Ice::Current& =
			Ice::Current());

	virtual ::MyUtil::IntSeq getBlockSeq(::Ice::Int, ::Ice::Int, ::Ice::Int,
			const Ice::Current& = Ice::Current());
	virtual ::MyUtil::Int2IntSeqMap getBlockSeqBatch(const ::MyUtil::IntSeq&,
			const Ice::Current& = Ice::Current());

	virtual void reload(::Ice::Int, const Ice::Current& = Ice::Current());

        OpState processWithState(const BuddyState& state, const Relationship& relation,
                const ApplyInfo* applyInfo = NULL, const Ice::Current& =  Ice::Current());

	void bootstrap(const string& endpoint, int mod);

	virtual void addFriend(const Relationship&, const Ice::Current& =
                        Ice::Current());

        virtual void cacheAddApply(const Relationship&, const Ice::Current& =
                        Ice::Current());

        virtual void cacheAcceptApply(const Relationship&, const Ice::Current& =
                        Ice::Current());

        virtual void cacheDenyApply(const Relationship&, const Ice::Current& =
                        Ice::Current());

        virtual void cacheRemoveFriend(const Relationship&, const Ice::Current& =
                        Ice::Current());

        virtual void cacheAddBlock(const Relationship&, const Ice::Current& =
                        Ice::Current());

        virtual void cacheRemoveBlock(const Relationship&, const Ice::Current& =
                        Ice::Current());

private:
	BuddyDesc translate(uint32_t desc);
	BuddyRelationHolderPtr locateBuddyRelationHolder(uint32_t from);
        OpState itemEmptyCheck(const BuddyState& state, const Relationship& relation, OpState& result,
                                const ApplyInfo* applyInfo = NULL,  const Ice::Current& current = Ice::Current() );
        OpState itemOverflowCheck(const BuddyState& state ,  const Relationship& relation , 
					OpState& result, const Ice::Current& current = Ice::Current());
        OpState addApplyHandle( const Relationship& apply,const ApplyInfo& applyInfo,
                                 OpState& result, const Ice::Current& current = Ice::Current());
        OpState acceptApplyHandle( const Relationship& apply,OpState& result,
                                     const Ice::Current& current = Ice::Current());
        OpState denyApplyHandle( const Relationship& apply,OpState& result,
                                  const Ice::Current& current = Ice::Current());
        OpState removeFriendHandle( const Relationship& buddy, OpState& result,
                                    const Ice::Current& current = Ice::Current());
        OpState addBlockHandle( const Relationship& block, OpState& result,
                                const Ice::Current& current = Ice::Current());
        OpState removeBlockHandle( const Relationship& block, OpState& result,
                                    const Ice::Current& current = Ice::Current());

        OpState processHandleWithState(const BuddyState& state, const Relationship& relation, OpState& result,
                const ApplyInfo* applyInfo = NULL, const Ice::Current& current = Ice::Current());
        void specialHandle(const BuddyState& state, const Relationship& relation, 
                                         const Ice::Current& current = Ice::Current());

	bool isLocalCache(const Ice::Current& current);
        bool isBiblock(const Ice::Current& current);
	BuddyRelationHolderPtr findBuddyRelationHolder(uint32_t from);
	void removeBuddyRelationHolder(uint32_t from);
        void cacheUserCountGuestRequest(uint32_t userId);

        string translateBuddyState(const BuddyState& state) const
        {
            switch(state)
            {
                case NULLSTATE:
                    return "Null State";
                case ADDAPPLY:
                    return "Add Apply";
                case ACCEPTAPPLY:
                    return "Accept Apply";
                case DENYAPPLY:
                    return "Deny Apply";
                case REMOVEFRIEND:
                    return "Remove Friend";
                case ADDBLOCK:
                    return "Add Block";
                case REMOVEBLOCK:
                    return "Remove Block";
                default:
                    return "Null State";
            }
        }
	string _endpoint;
        int _cluster;
        int _mod;
};


class BuddyDatabaseHelper: public MyUtil::Singleton<BuddyDatabaseHelper> {
public:
	bool insertBuddyApply(int applicant, int accepter,
			const MyUtil::Str2StrMap& props);
	bool deleteBuddyApply(int applicant, int accepter);
	bool insertRelationBi(int from, int to);
	bool deleteRelationBi(int from, int to);
	bool insertBlock(int from, int to);
	bool deleteBlock(int from, int to);
private:
	string getRelationTable(int from);
};

class BuddyRelationHolderFactory: public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class BuddyResultHandler: public com::xiaonei::xce::ResultHandler {
public:
	BuddyResultHandler(int userId, set<int>& building, MyUtil::IntSeq& duplicates, const std::string& toField) :
		_userId(userId), _building(building), _duplicates(duplicates),_toField(toField) {
	}
	virtual ~BuddyResultHandler() {
	}
private:
	virtual bool handle(mysqlpp::Row& row) const {
		int to = (int) row[_toField.c_str()];
		if (!_building.insert(to).second) {
			_duplicates.push_back(to);
		}
		return true;
	}

	int _userId;
	set<int>& _building;
	MyUtil::IntSeq& _duplicates;
	std::string _toField;
};

class DupCleanupTask: public MyUtil::Task {
public:
	DupCleanupTask(int& userId, BuddyRelation& dup) :
		MyUtil::Task(TASK_LEVEL_DUPCLEANUP), _userId(userId), _dup(dup) {
	}
	virtual void handle();
private:
	void handle(const BuddyDesc,const int);
	int _userId;
	BuddyRelation _dup;
};

class AddApplyTask: virtual public MyUtil::Task {
public:
	AddApplyTask(BuddyManagerI* bm,
			const Relationship& apply,
			const ApplyInfo& info,
			const Ice::Current& current) :
		_bm(bm), _apply(apply), _info(info), _current(current) {
	}
	virtual void handle();
private:
	BuddyManagerI* _bm;
	Relationship _apply;
	ApplyInfo _info;
	Ice::Current _current;
};

class AcceptApplyTask: virtual public MyUtil::Task {
public:
	AcceptApplyTask(BuddyManagerI* bm,
			const Relationship& apply,
			const Ice::Current& current) :
		_bm(bm), _apply(apply), _current(current) {
	}
	virtual void handle();
private:
	BuddyManagerI* _bm;
	Relationship _apply;
	Ice::Current _current;
};

class DenyApplyTask: virtual public MyUtil::Task {
public:
	DenyApplyTask(BuddyManagerI* bm,
			const Relationship& apply,
			const Ice::Current& current) :
		_bm(bm), _apply(apply), _current(current) {
	}
	virtual void handle();
private:
	BuddyManagerI* _bm;
	Relationship _apply;
	Ice::Current _current;
};

class RemoveFriendTask: virtual public MyUtil::Task {
public:
	RemoveFriendTask(BuddyManagerI* bm,
			const Relationship& buddy,
			const Ice::Current& current) :
		_bm(bm), _buddy(buddy), _current(current) {
	}
	virtual void handle();
private:
	BuddyManagerI* _bm;
	Relationship _buddy;
	Ice::Current _current;
};

class AddBlockTask: virtual public MyUtil::Task {
public:
	AddBlockTask(BuddyManagerI* bm,
			const Relationship& block,
			const Ice::Current& current) :
		_bm(bm), _block(block), _current(current) {
	}
	virtual void handle();
private:
	BuddyManagerI* _bm;
	Relationship _block;
	Ice::Current _current;
};

class RemoveBlockTask: virtual public MyUtil::Task {
public:
	RemoveBlockTask(BuddyManagerI* bm,
			const Relationship& block,
			const Ice::Current& current) :
		_bm(bm), _block(block), _current(current) {
	}
	virtual void handle();
private:
	BuddyManagerI* _bm;
	Relationship _block;
	Ice::Current _current;
};

class CacheUserCountGuestRequestTask: virtual public MyUtil::Task {
public:
	CacheUserCountGuestRequestTask(uint32_t& userId, uint32_t& count) :
		Task(TASK_LEVEL_CACHEUSERCOUNT), _userId(userId), _count(count) {
	}
	virtual void handle();
private:
	uint32_t _userId;
	uint32_t _count;
};

}
}

#endif
