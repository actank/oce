#include "FriendRankLoaderAdapter.h"

using namespace MyUtil;
using namespace xce::socialgraph;

FriendRankLoaderAdapter::FriendRankLoaderAdapter()
{
	_managers.resize(cluster() > 0 ? cluster() : 1);
	//MCE_INIT("./log", "INFO");
}

FriendRankLoaderAdapter::~FriendRankLoaderAdapter()
{
}

bool FriendRankLoaderAdapter::isValid(const Ice::ObjectPrx& proxy)
{
	return true;
}

void FriendRankLoaderAdapter::reload(int userId)
{
	try {
		locate<FriendRankLoaderPrx>(_managers, "FriendRankLoader", userId, TWO_WAY)->reload(userId);
	} catch (Ice::TimeoutException& e) {
		MCE_ERROR("GetRank timeout exception" << e);
	} catch (Ice::Exception& e) {
		MCE_ERROR("GetRank ice::exception " << e.what());
	} catch (...) {
		MCE_ERROR("GetRank exception");
	}
}

