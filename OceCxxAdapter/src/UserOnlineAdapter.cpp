#include "UserOnlineAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>

using namespace mop::hi::svc::online;
using namespace mop::hi::svc::adapter;
using namespace std;

//---------------------------------------------------------------------------
void UserOnlineAdapter::initialize() {
	_manager = locate<OnlineManagerPrx>("Manager", TWO_WAY);
	_managerOneway = locate<OnlineManagerPrx>("Manager", ONE_WAY);
}

void UserOnlineAdapter::imOnline(int userId) {
	_managerOneway->setStat(userId, OT_IM, true);
}

void UserOnlineAdapter::webOnline(int userId) {
	_managerOneway->setStat(userId, OT_WEB, true);
}

void UserOnlineAdapter::imOffline(int userId) {
	_managerOneway->setStat(userId, OT_IM, false);
}

void UserOnlineAdapter::webOffline(int userId) {
	_managerOneway->setStat(userId, OT_WEB, false);
}

void UserOnlineAdapter::imOfflineBatch(IntSeq userIds) {
	_managerOneway->setStats(userIds, OT_IM, false);
}

void UserOnlineAdapter::webOfflineBatch(IntSeq userIds) {
	_managerOneway->setStats(userIds, OT_WEB, false);
}

int UserOnlineAdapter::size() {
	return _manager->size(3);
}

