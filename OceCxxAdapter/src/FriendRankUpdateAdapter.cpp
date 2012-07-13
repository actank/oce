#include "FriendRankUpdateAdapter.h"

using namespace MyUtil;
using namespace xce::socialgraph;

FriendRankUpdateAdapter::FriendRankUpdateAdapter()
{
	_managers.resize(cluster() > 0 ? cluster() : 1);
	//MCE_INIT("./log", "INFO");
}

FriendRankUpdateAdapter::~FriendRankUpdateAdapter()
{
}

bool FriendRankUpdateAdapter::isValid(const Ice::ObjectPrx& proxy)
{
	return true;
}

int FriendRankUpdateAdapter::TriggerUserModel()
{
	try {
		return locate<FriendRankUpdatePrx>(_managers, "FriendRankUpdate", 0, TWO_WAY)->TriggerUserModel();
	} catch (Ice::TimeoutException& e) {
		MCE_ERROR("TriggerUserModel timeout exception" << e);
	} catch (Ice::Exception& e) {
		MCE_ERROR("TriggerUserModel ice::exception " << e.what());
	} catch (...) {
		MCE_ERROR("TriggerUserModel exception");
	}
	return -1;
}

int FriendRankUpdateAdapter::TriggerFriendRank()
{
	try {
		return locate<FriendRankUpdatePrx>(_managers, "FriendRankUpdate", 0, TWO_WAY)->TriggerFriendRank();
	} catch (Ice::TimeoutException& e) {
		MCE_ERROR("TriggerUserModel timeout exception" << e);
	} catch (Ice::Exception& e) {
		MCE_ERROR("TriggerUserModel ice::exception " << e.what());
	} catch (...) {
		MCE_ERROR("TriggerUserModel exception");
	}
	return -1;
}

