/*
 * BuddyXoaLogicAdapter.cpp
 *
 *  Created on: 2011-7-25
 *      Author: xiaofeng.liang@renren-inc.com
 */

#include "BuddyXoaLogicAdapter.h"

using namespace xce::buddy;

void BuddyXoaLogicAdapter::addApply(int applicant, int apply, const Ice::Context ctx){
	MCE_INFO("[BuddyXoaLogicAdapter::addApply] applicant=" << applicant << " apply=" << apply);
	try{
		getManagerOneway(applicant)->addApply(applicant, apply, ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::addApply] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::addApply] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[BuddyXoaLogicAdapter::addApply] UNKNOWN exception");
	}
}

void BuddyXoaLogicAdapter::acceptApply(int applicant, int apply, const Ice::Context ctx){
	MCE_INFO("[BuddyXoaLogicAdapter::acceptApply] applicant=" << applicant << " apply=" << apply);
	try{
		getManagerOneway(applicant)->acceptApply(applicant, apply, ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::acceptApply] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::acceptApply] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[BuddyXoaLogicAdapter::acceptApply] UNKNOWN exception");
	}
}

void BuddyXoaLogicAdapter::denyApply(int applicant, int apply, const Ice::Context ctx){
	MCE_INFO("[BuddyXoaLogicAdapter::denyApply] applicant=" << applicant << " apply=" << apply);
	try{
		getManagerOneway(applicant)->denyApply(applicant, apply, ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::denyApply] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::denyApply] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[BuddyXoaLogicAdapter::denyApply] UNKNOWN exception");
	}
}

void BuddyXoaLogicAdapter::addFriend(int from, int to, const Ice::Context ctx){
	MCE_INFO("[BuddyXoaLogicAdapter::addFriend] from=" << from << " to=" << to);
	try{
		getManagerOneway(from)->addFriend(from, to, ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::addFriend] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::addFriend] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[BuddyXoaLogicAdapter::addFriend] UNKNOWN exception");
	}
}

void BuddyXoaLogicAdapter::removeFriend(int from, int to, const Ice::Context ctx){
	MCE_INFO("[BuddyXoaLogicAdapter::removeFriend] from=" << from << " to=" << to);
	try{
		getManagerOneway(from)->removeFriend(from, to, ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::removeFriend] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::removeFriend] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[BuddyXoaLogicAdapter::removeFriend] UNKNOWN exception");
	}
}

void BuddyXoaLogicAdapter::addBlock(int block, int blocked, const Ice::Context ctx){
	MCE_INFO("[BuddyXoaLogicAdapter::addBlock] block=" << block << " blocked=" << blocked);
	try{
		getManagerOneway(block)->addBlock(block, blocked, ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::addBlock] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::addBlock] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[BuddyXoaLogicAdapter::addBlock] UNKNOWN exception");
	}
}

void BuddyXoaLogicAdapter::removeBlock(int block, int blocked, const Ice::Context ctx){
	MCE_INFO("[BuddyXoaLogicAdapter::removeBlock] block=" << block << " blocked=" << blocked);
	try{
		getManagerOneway(block)->removeBlock(block, blocked, ctx);
	} catch (Ice::Exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::removeBlock] Ice::Exception : " << e.what());
	} catch (std::exception& e) {
		MCE_WARN("[BuddyXoaLogicAdapter::removeBlock] std::exception : " << e.what());
	} catch (...) {
		MCE_WARN("[BuddyXoaLogicAdapter::removeBlock] UNKNOWN exception");
	}
}

void BuddyXoaLogicAdapter::reload(int userId, const Ice::Context ctx){
  MCE_INFO("[BuddyXoaLogicAdapter::reload] userId=" <<  userId);
  try{
    getManagerOneway(userId)->reload(userId, ctx);
  } catch (Ice::Exception& e) {
    MCE_WARN("[BuddyXoaLogicAdapter::removeBlock] Ice::Exception : " << e.what());
  } catch (std::exception& e) {
    MCE_WARN("[BuddyXoaLogicAdapter::removeBlock] std::exception : " << e.what());
  } catch (...) {
    MCE_WARN("[BuddyXoaLogicAdapter::removeBlock] UNKNOWN exception");
  }
}


BuddyXoaLogicPrx BuddyXoaLogicAdapter::getManagerOneway(int id){
	return locate<BuddyXoaLogicPrx> (_managersOneway, "M", id, ONE_WAY, 300);
}

BuddyXoaLogicPrx BuddyXoaLogicAdapter::getManager(int id){
	return locate<BuddyXoaLogicPrx> (_managers, "M", id, TWO_WAY, 300);
}


