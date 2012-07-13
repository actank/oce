#include "UserScoreActiveTrackAdapter.h"

using namespace wservice::userscore::adapter;

ActiveTrackLogicPrx UserScoreActiveTrackAdapter::getManagerUdp(int id) {
	return locate<ActiveTrackLogicPrx>(_managersUdp, "M", id, DATAGRAM);
}

ActiveTrackLogicPrx UserScoreActiveTrackAdapter::getManagerOneway(int id) {
	return locate<ActiveTrackLogicPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

void UserScoreActiveTrackAdapter::incrLoginCount(int userId)
{
	getManagerUdp(userId)->incrLoginCount(userId,4);
}

void UserScoreActiveTrackAdapter::incrLoginCountTcp(int userId)
{
	getManagerOneway(userId)->incrLoginCount(userId,4);
}

