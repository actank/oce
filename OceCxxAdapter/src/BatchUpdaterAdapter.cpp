#include "BatchUpdaterAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace mop::hi::svc::adapter;
using namespace std;

//---------------------------------------------------------------------------
void BatchUpdaterAdapter::initialize() {
	_batchUpdaterManager
			= locate<BatchUpdaterManagerPrx>("M", TWO_WAY);
	_batchUpdaterManagerOneway = locate<BatchUpdaterManagerPrx>("M", ONE_WAY);
}

void BatchUpdaterAdapter::inc(const MyUtil::IntSeq& keyvalue, ::Ice::Int count,
		const std::string& type) {
	_batchUpdaterManagerOneway->inc(keyvalue, count, type);
}

void BatchUpdaterAdapter::dec(const MyUtil::IntSeq& keyvalue, ::Ice::Int count,
		const std::string& type) {
	_batchUpdaterManagerOneway->dec(keyvalue, count, type);
}

void BatchUpdaterAdapter::incUserViewCount(int id, int count) {
	_batchUpdaterManagerOneway->incUserViewCount(id, count);
}

void BatchUpdaterAdapter::incLoginCount(int id, int count) {
	_batchUpdaterManagerOneway->incLoginCount(id, count);
}

void BatchUpdaterAdapter::setLastLoginTime(int id, string time) {
	_batchUpdaterManagerOneway->setLastLoginTime(id, time);
}

void BatchUpdaterAdapter::setLastLogoutTime(int id, string time) {
	_batchUpdaterManagerOneway->setLastLogoutTime(id, time);
}

void BatchUpdaterAdapter::setLastModifyTime(int id, string time) {
	_batchUpdaterManagerOneway->setLastModifyTime(id, time);
}

void BatchUpdaterAdapter::incFriendDist(int userId, int universityId, int count) {
	_batchUpdaterManagerOneway->incFriendDist(userId, universityId, count);
}

void BatchUpdaterAdapter::decFriendDist(int userId, int universityId, int count) {
	_batchUpdaterManagerOneway->decFriendDist(userId, universityId, count);
}

void BatchUpdaterAdapter::incExp(int userId, int count) {
	_batchUpdaterManagerOneway->incExp(userId, count);
}

void BatchUpdaterAdapter::decExp(int userId, int count) {
	_batchUpdaterManagerOneway->decExp(userId, count);
}

void BatchUpdaterAdapter::incTribeCount(int userId, int count) {
	_batchUpdaterManagerOneway->incTribeCount(userId, count);
}

void BatchUpdaterAdapter::decTribeCount(int userId, int count) {
	_batchUpdaterManagerOneway->decTribeCount(userId, count);
}

void BatchUpdaterAdapter::incTribeRequestCount(int userId, int count) {
	_batchUpdaterManagerOneway->incTribeRequestCount(userId, count);
}

void BatchUpdaterAdapter::decTribeRequestCount(int userId, int count) {
	_batchUpdaterManagerOneway->decTribeRequestCount(userId, count);
}

void BatchUpdaterAdapter::incNewMessageCount(int userId, int count) {
	_batchUpdaterManagerOneway->incNewMessageCount(userId, count);
}

void BatchUpdaterAdapter::decNewMessageCount(int userId, int count) {
	_batchUpdaterManagerOneway->decNewMessageCount(userId, count);
}

void BatchUpdaterAdapter::incMessageInCount(int id, int count) {
	_batchUpdaterManagerOneway->incMessageInCount(id, count);
}

void BatchUpdaterAdapter::decMessageInCount(int id, int count) {
	_batchUpdaterManagerOneway->decMessageInCount(id, count);
}

void BatchUpdaterAdapter::incMessageOutCount(int userId, int count) {
	_batchUpdaterManagerOneway->incMessageOutCount(userId, count);
}

void BatchUpdaterAdapter::decMessageOutCount(int userId, int count) {
	_batchUpdaterManagerOneway->decMessageOutCount(userId, count);
}

void BatchUpdaterAdapter::incFriendCount(int id, int count, const Ice::Context& ctx) {
	_batchUpdaterManagerOneway->incFriendCount(id, count, ctx);
}

void BatchUpdaterAdapter::decFriendCount(int id, int count, const Ice::Context& ctx) {
	_batchUpdaterManagerOneway->decFriendCount(id, count, ctx);
}

void BatchUpdaterAdapter::incGuestRequestCount(int id, int count,
		const Ice::Context& ctx) {
	_batchUpdaterManagerOneway->incGuestRequestCount(id, count, ctx);
}

void BatchUpdaterAdapter::decGuestRequestCount(int id, int count,
		const Ice::Context& ctx) {
	_batchUpdaterManagerOneway->decGuestRequestCount(id, count, ctx);
}

void BatchUpdaterAdapter::incLeavewordCount(int id, int count) {
	_batchUpdaterManagerOneway->incLeavewordCount(id, count);
}

void BatchUpdaterAdapter::decLeavewordCount(int id, int count) {
	_batchUpdaterManagerOneway->decLeavewordCount(id, count);
}
