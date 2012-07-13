#include "BuddyCoreLogicAdapter.h"

using namespace com::xiaonei::service::buddy;

void BuddyCoreLogicAdapter::initialize() {
}

void BuddyCoreLogicAdapter::addApply(const Applyship& apply, const Ice::Context& ctx) {
	getProxyOneway(0)->addApply(apply, ctx);
}

void BuddyCoreLogicAdapter::acceptApply(const Applyship& apply, const Ice::Context& ctx) {
	getProxyOneway(0)->acceptApply(apply, ctx);
}

void BuddyCoreLogicAdapter::denyApply(const Applyship& apply, const Ice::Context& ctx) {
	getProxyOneway(0)->denyApply(apply, ctx);
}

void BuddyCoreLogicAdapter::removeFriend(const Friendship& buddy, const Ice::Context& ctx) {
	getProxyOneway(0)->removeFriend(buddy, ctx);
}

void BuddyCoreLogicAdapter::addBlock(const Blockship& block, const Ice::Context& ctx) {
	getProxyOneway(0)->addBlock(block, ctx);
}

void BuddyCoreLogicAdapter::removeBlock(const Blockship& block, const Ice::Context& ctx) {
	getProxyOneway(0)->removeBlock(block, ctx);
}

void BuddyCoreLogicAdapter::addFriend(const Ice::Int from, const Ice::Int to, const Ice::Context& ctx) {
        getProxyOneway(0)->addFriend(from, to, ctx);
}

void BuddyCoreLogicAdapter::reload(const Ice::Int userId) {
	getProxyOneway(0)->reload(userId);
}
