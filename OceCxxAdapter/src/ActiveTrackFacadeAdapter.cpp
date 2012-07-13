#include "ActiveTrackFacadeAdapter.h"

using namespace MyUtil;
using namespace xce::activetrack;
using namespace xce::activetrack::adapter;

ActiveTrackFacadePrx ActiveTrackFacadeAdapter::getManagerUDP(int id) {
	return locate<ActiveTrackFacadePrx>(_managersUDP, "ATF", id, DATAGRAM);
}

void ActiveTrackFacadeAdapter::action(int userId, const MyUtil::StrSeq& paths, const std::string& action, const std::string& object) {
	getManagerUDP(userId)->action(userId,paths,action,object);
}

