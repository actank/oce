#ifndef BUDDYCOREADAPTER_H_
#define BUDDYCOREADAPTER_H_

#include "BuddyCommon.h"
#include "Buddy.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace buddy {

using namespace MyUtil;

const static bool Cache_LOCAL = false;
const static bool Cache_REMOTE = true;
const static std::string Cache_REMOTE_KEY = "Cache_REMOTE_KEY";
const static std::string Cache_REMOTE_VALUE = "1";
const static std::string Cache_LOCAL_KEY = "Cache_LOCAL_KEY";
const static std::string Cache_LOCAL_VALUE = "1";

const static bool Cache_BIBLOCK = true;
const static bool Cache_NORMALBLOCK = false;
const static std::string Cache_BIBLOCK_KEY = "Cache_BIBLOCK_KEY";
const static std::string Cache_BIBLOCK_VALUE = "1";

const static bool Cache_MAIN = true;
const static bool Cache_OTHER = false;
const static std::string Cache_MAIN_KEY = "Cache_MAIN_KEY";
const static std::string Cache_MAIN_VALUE = "1";


class BuddyAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyManagerPrx>,
        public MyUtil::Singleton<BuddyAdapter> {

public:
	BuddyAdapter() :
                MyUtil::ReplicatedClusterAdapterI <BuddyManagerPrx>("ControllerBuddy", 120, 300) {
	}

        void addApply(const Relationship& apply, const ApplyInfo& info );
        OpState addApplyWithState(const Relationship& apply, const ApplyInfo& info);
        void acceptApply(const Relationship& apply);
        OpState acceptApplyWithState(const Relationship& apply);
        void denyApply(const Relationship& apply);
        OpState denyApplyWithState(const Relationship& apply);
        void denyAllApplied(::Ice::Int userId);
        ::Ice::Int getAppliedCount(::Ice::Int userId);
        ::MyUtil::IntSeq getAppliedSeq(::Ice::Int userId, ::Ice::Int begin,
                        ::Ice::Int limit);
        ::Ice::Int getApplyCount(::Ice::Int userId);
        ::MyUtil::IntSeq getApplySeq(::Ice::Int userId, ::Ice::Int begin,
                        ::Ice::Int limit);
        void removeFriend(const Relationship& buddy);
        OpState removeFriendWithState(const Relationship& buddy);
        ::Ice::Int getFriendCount(::Ice::Int userId);
        ::MyUtil::IntSeq getFriendSeq(::Ice::Int userId, ::Ice::Int begin,
                        ::Ice::Int limit, int timeout = 0);
        void addBlock(const Relationship& block);
        OpState addBlockWithState(const Relationship& block);
        void removeBlock(const Relationship& block);
        OpState removeBlockWithState(const Relationship& block);
        void changeBlockList(int from, const MyUtil::IntSeq& blockIds);
        ::Ice::Int getBlockCount(::Ice::Int userId);
        ::MyUtil::IntSeq getBlockSeq(::Ice::Int userId, ::Ice::Int begin,
                        ::Ice::Int limit, int timeout = 0);
        BuddyDesc getRelation(const Relationship& relation, int timeout = 0);
	void reload(Ice::Int);
	void load(Ice::Int,Ice::ObjectPtr);
	void remove(Ice::Int);
	bool setLocalPrx(BuddyManagerPrx& prx){ 
		if(prx) {
			_localprx = prx;
			return true;	
		}
		return false;
	}
	void buildContext(Ice::Context&, const bool maincache, const bool remote = Cache_REMOTE,
                      const bool biblock = Cache_NORMALBLOCK);

	::MyUtil::Int2IntSeqMap getFriendSeqBatch(::MyUtil::IntSeq userIds, int timeout = 0);
	BuddyRelationPtr getBuddyRelation(::Ice::Int userId, int timeout = 0);
	//为BuddyRelationCache预加载使用的批量接口	
	Int2BuddyRelationMap getBuddyRelationBatch(::MyUtil::IntSeq userIds, int timeout = 0);
	
private:
	friend class BuddyManagerI;
	//void addFriend(const Relationship&, const Ice::Current&);
        void cacheAddApply(const Relationship&, const Ice::Context&);
        void cacheAcceptApply(const Relationship&, const Ice::Context&);
        void cacheDenyApply(const Relationship&, const Ice::Context&);
        void cacheRemoveFriend(const Relationship&, const Ice::Context&);
        void cacheAddBlock(const Relationship&, bool biblock, const Ice::Context&);
        void cacheRemoveBlock(const Relationship&, bool biblock, const Ice::Context&);
	void updateRelation(const Relationship& relation, const BuddyState& oper, 
			BuddyRelationPtr localbrp, BuddyRelationPtr remotebrp,
			const bool biblock = false, const Ice::Context& = Ice::Current().ctx);
	BuddyManagerPrx getManager(int timout = 0);	
	BuddyManagerPrx getManagerOneway();	
	Ice::ObjectPtr create(Ice::Int id);
	BuddyManagerPrx _localprx;
}; 

class LocalUpdateRelationTask: virtual public MyUtil::Task {
public:
        LocalUpdateRelationTask(
			BuddyManagerPrx& prx,
                        const Relationship& relation,
			const BuddyState& oper, 
			const bool biblock,
			const StateRecorder& sr, 
			const Ice::Context& context) :
                 _prx(prx),_relation(relation), _oper(oper),_biblock(biblock),_sr(sr),_context(context) {
        }
        virtual void handle();
private:
	BuddyManagerPrx _prx;
        Relationship _relation;
	BuddyState _oper;
	bool _biblock;
	StateRecorder _sr;
	Ice::Context _context;
};


class RemoteUpdateRelationTask: virtual public MyUtil::Task {
public:
        RemoteUpdateRelationTask(
                        BuddyManagerPrx& prx,
                        const Relationship& relation,
                        const BuddyState& oper,
                        const bool biblock,
			const StateRecorder& sr,
			const Ice::Context& context) :
                 _prx(prx),_relation(relation), _oper(oper),_biblock(biblock),_sr(sr),_context(context) {
        }
        virtual void handle();
private:
        BuddyManagerPrx _prx;
        Relationship _relation;
        BuddyState _oper;
        bool _biblock;
	StateRecorder _sr;
	Ice::Context _context;
};

class LoadTask: virtual public MyUtil::Task {
public:
        LoadTask( Ice::Int id, Ice::ObjectPtr objptr):
                 _id(id),_objptr(objptr) {
        }
        virtual void handle();
private:
        Ice::Int _id;
	Ice::ObjectPtr _objptr;
};

class BuddyLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI<BuddyLoaderPrx>, public MyUtil::Singleton<BuddyLoaderAdapter> {
public:
        BuddyLoaderAdapter() :
                MyUtil::ReplicatedClusterAdapterI <BuddyLoaderPrx>("ControllerBuddyLoader", 120, 300) {
        }
	void load(Ice::Int);
};

/*
class BuddyFriendFinderAdapter :
	public AbstractBuddyAdapter,
	public MyUtil::AdapterISingleton<MyUtil::OceChannel, BuddyFriendFinderAdapter> {

public:
	BuddyFriendFinderAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
protected:
	virtual string name() {
		return "BuddyFriendFinder";
	}

	virtual string endpoints() {
		return "@BuddyFriendFinder";
	}

	virtual size_t cluster() {
		return 0;
	}

};
*/
}
}

#endif /*BUDDYCOREADAPTER_H_*/
