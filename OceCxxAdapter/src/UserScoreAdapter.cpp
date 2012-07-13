#include "UserScoreAdapter.h"

using namespace xce::userscore;


UserScoreLogicPrx UserScoreAdapter::getManager(int id) {
	return locate<UserScoreLogicPrx>(_managers, "M", id, TWO_WAY);
}

UserScoreLogicPrx UserScoreAdapter::getManagerOneway(int id) {
	return locate<UserScoreLogicPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

void UserScoreAdapter::addScore(int userId,int type,int count)
{
	getManagerOneway(userId)->addScore(userId,type,count);
}
