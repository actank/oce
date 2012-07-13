#include "UserScoreEventAdapter.h"

using namespace wservice::userscore::adapter;

EventSenderPrx UserScoreEventAdapter::getManagerUdp(int id) {
	return locate<EventSenderPrx>(_managersUdp, "M", id, DATAGRAM);
}

EventSenderPrx UserScoreEventAdapter::getManagerOneway(int id) {
	return locate<EventSenderPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

void UserScoreEventAdapter::send(const MyUtil::Str2StrMap& event)
{
	getManagerOneway(0)->send(event);
}
