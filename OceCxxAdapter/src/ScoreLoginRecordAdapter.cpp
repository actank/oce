#include "ScoreLoginRecordAdapter.h"

using namespace xce::userscore;


LoginRecordPrx ScoreLoginRecordAdapter::getManager(int id) {
	return locate<LoginRecordPrx>(_managers, "M", id, TWO_WAY);
}

LoginRecordPrx ScoreLoginRecordAdapter::getManagerOneway(int id) {
	return locate<LoginRecordPrx>(_managersOneway, "M", id, ONE_WAY);
}

//---------------------------------------------

void ScoreLoginRecordAdapter::incLoginCount(int userId,char type)
{
	getManagerOneway(userId)->incrLoginCount(userId,type);
}
