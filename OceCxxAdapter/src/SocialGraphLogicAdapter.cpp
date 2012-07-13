#include "SocialGraphLogicAdapter.h"

using namespace xce::socialgraph;

void SocialGraphLogicAdapter::Report(int hostId, Message msg) {
	getProxy(hostId)->Report(msg);
}
