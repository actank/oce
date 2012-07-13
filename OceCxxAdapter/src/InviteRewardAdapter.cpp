#include "InviteRewardAdapter.h"

using namespace com::xiaonei::wService::invitereward;
using namespace wservice::invitereward::adapter;

InviteRewardManagerPrx InviteRewardAdapter::getManagerOneway(int id) {
	return locate<InviteRewardManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

void InviteRewardAdapter::addStar(int userId)
{
	getManagerOneway(userId)->addStar(userId);
}

