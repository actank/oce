#include "BuddyCoreAdapter.h"

namespace com {
namespace xiaonei {
namespace service {
namespace buddy {

void BuddyCoreAdapter::addApply(const Applyship& apply,
		const ApplyInfo& info, const Ice::Context& ctx) {
	getProxyOneway(apply.applicant)->addApply(apply, info, ctx);

}

OpState BuddyCoreAdapter::addApplyWithState(const Applyship& apply,
		const ApplyInfo& info) {
	return getProxy(apply.applicant)->addApplyWithState(apply, info);
}

void BuddyCoreAdapter::acceptApply(const Applyship& apply,
		const Ice::Context& ctx) {
	getProxyOneway(apply.accepter)->acceptApply(apply, ctx);
}

OpState BuddyCoreAdapter::acceptApplyWithState(const Applyship& apply) {
	return getProxy(apply.accepter)->acceptApplyWithState(apply);
}

void BuddyCoreAdapter::denyApply(const Applyship& apply,
		const Ice::Context& ctx) {
	getProxyOneway(apply.accepter)->denyApply(apply, ctx);
}

OpState BuddyCoreAdapter::denyApplyWithState(const Applyship& apply) {
	return getProxy(apply.accepter)->denyApplyWithState(apply);
}

void BuddyCoreAdapter::denyAllApplied(::Ice::Int userId) {
	getProxyOneway(userId)->denyAllApplied(userId);
}

::Ice::Int BuddyCoreAdapter::getAppliedCount(::Ice::Int userId) {
	return getProxy(userId)->getAppliedCount(userId);
}

::MyUtil::IntSeq BuddyCoreAdapter::getAppliedSeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit) {
	return getProxy(userId)->getAppliedSeq(userId, begin, limit);
}

::Ice::Int BuddyCoreAdapter::getApplyCount(::Ice::Int userId) {
	return getProxy(userId)->getApplyCount(userId);
}

::MyUtil::IntSeq BuddyCoreAdapter::getApplySeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit) {
	return getProxy(userId)->getApplySeq(userId, begin, limit);
}

void BuddyCoreAdapter::removeFriend(const Friendship& buddy) {
	getProxyOneway(buddy.from)->removeFriend(buddy);
}

OpState BuddyCoreAdapter::removeFriendWithState(const Friendship& buddy) {
	return getProxy(buddy.from)->removeFriendWithState(buddy);
}

::Ice::Int BuddyCoreAdapter::getFriendCount(::Ice::Int userId) {
	return getProxy(userId)->getFriendCount(userId);
}

::MyUtil::IntSeq BuddyCoreAdapter::getFriendSeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit, int timeout) {
	//std::cout << "getFriendSeq " << userId << std::endl;
	//try {
		return getProxy(userId)->getFriendSeq(userId, begin, limit);
	/*} catch(std::exception& e) {
		std::cout << "BuddyCoreAdapter::getFriendSeq [std::exception] userId="<< userId << " begin=" << begin << " limit=" << limit << e.what() << std::endl;
		return ::MyUtil::IntSeq();
	} catch(...) {
		std::cout << "BuddyCoreAdapter::getFriendSeq [...] userId="<< userId << " begin=" << begin << " limit=" << limit << std::endl;
		return ::MyUtil::IntSeq();
	}*/
	//std::cout << "getFriendSeq done " << userId << std::endl;
}

void BuddyCoreAdapter::addBlock(const Blockship& block) {
	getProxyOneway(block.from)->addBlock(block);
}

OpState BuddyCoreAdapter::addBlockWithState(const Blockship& block) {
	return getProxy(block.from)->addBlockWithState(block);
}

void BuddyCoreAdapter::removeBlock(const Blockship& block) {
	getProxyOneway(block.from)->removeBlock(block);
}

OpState BuddyCoreAdapter::removeBlockWithState(const Blockship& block) {
	return getProxy(block.from)->removeBlockWithState(block);
}

void BuddyCoreAdapter::changeBlockList(int from,
		const MyUtil::IntSeq& blockIds) {
	MyUtil::IntSeq oldBlockIds = getBlockSeq(from, 0, -1);
	MyUtil::IntSeq::const_iterator oit = oldBlockIds.begin();
	MyUtil::IntSeq::const_iterator it = blockIds.begin();
	for (size_t i = 0; i < oldBlockIds.size(); ++i) {
		it = find(blockIds.begin(),blockIds.end(),oldBlockIds.at(i));
		if(it == blockIds.end()){
			Blockship b;
			b.from = from;
			b.to = oldBlockIds.at(i);
			removeBlock(b);
		}
	}
	for (size_t i = 0; i < blockIds.size(); ++i) {
		oit = find(oldBlockIds.begin(),oldBlockIds.end(),blockIds.at(i));
		if(oit == oldBlockIds.end()){
			Blockship b;
			b.from = from;
			b.to = blockIds.at(i);
			addBlock(b);
		}
	}
}

::Ice::Int BuddyCoreAdapter::getBlockCount(::Ice::Int userId) {
	return getProxy(userId)->getBlockCount(userId);
}

::MyUtil::IntSeq BuddyCoreAdapter::getBlockSeq(::Ice::Int userId,
		::Ice::Int begin, ::Ice::Int limit, int timeout) {
	return getProxy(userId)->getBlockSeq(userId, begin, limit);
}

BuddyDesc BuddyCoreAdapter::getRelation(const Relationship& relation,
		int timeout) {
	return getProxy(relation.from)->getRelation(relation);
}

void BuddyCoreAdapter::reload(int userId) {
	MCE_INFO("BuddyCoreAdapter::reload " << userId);
	getProxyOneway(userId)->reload(userId);
}

}
}
}
}

