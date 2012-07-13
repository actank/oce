#ifndef BUDDYCOREADAPTER_H_
#define BUDDYCOREADAPTER_H_

#include "BuddyCore.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"

namespace com {
namespace xiaonei {
namespace service {
namespace buddy {

using namespace MyUtil;

class BuddyCoreAdapter : 
	public MyUtil::ReplicatedClusterAdapterI<BuddyCoreManagerPrx>, public MyUtil::Singleton<BuddyCoreAdapter>{
public:
	void addApply(const Applyship& apply, const ApplyInfo& info,
			const Ice::Context& ctx);
	OpState addApplyWithState(const Applyship& apply, const ApplyInfo& info);
	void acceptApply(const Applyship& apply, const Ice::Context& ctx);
	OpState acceptApplyWithState(const Applyship& apply);
	void denyApply(const Applyship& apply, const Ice::Context& ctx);
	OpState denyApplyWithState(const Applyship& apply);
	void denyAllApplied(::Ice::Int userId);
	::Ice::Int getAppliedCount(::Ice::Int userId);
	::MyUtil::IntSeq getAppliedSeq(::Ice::Int userId, ::Ice::Int begin,
			::Ice::Int limit);
	::Ice::Int getApplyCount(::Ice::Int userId);
	::MyUtil::IntSeq getApplySeq(::Ice::Int userId, ::Ice::Int begin,
			::Ice::Int limit);
	void removeFriend(const Friendship& buddy);
	OpState removeFriendWithState(const Friendship& buddy);
	::Ice::Int getFriendCount(::Ice::Int userId);
	::MyUtil::IntSeq getFriendSeq(::Ice::Int userId, ::Ice::Int begin,
			::Ice::Int limit, int timeout = 0);
	void addBlock(const Blockship& block);
	OpState addBlockWithState(const Blockship& block);
	void removeBlock(const Blockship& block);
	OpState removeBlockWithState(const Blockship& block);
	void changeBlockList(int from, const MyUtil::IntSeq& blockIds);
	::Ice::Int getBlockCount(::Ice::Int userId);
	::MyUtil::IntSeq getBlockSeq(::Ice::Int userId, ::Ice::Int begin,
			::Ice::Int limit, int timeout = 0);
	BuddyDesc getRelation(const Relationship& relation, int timeout = 0);
	void reload(int userId);

private:
	BuddyCoreAdapter():
                MyUtil::ReplicatedClusterAdapterI <BuddyCoreManagerPrx>("ControllerBuddyCore", 120, 300){
        }
        friend class MyUtil::Singleton<BuddyCoreAdapter>;
};

}
}
}
}

#endif /*BUDDYCOREADAPTER_H_*/
