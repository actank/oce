#include "FriendFinderWorkerAdapter.h"

using namespace xce::socialgraph;

void FriendFinderWorkerAdapter::Report(int host_id, const Info& information) {
	getProxy(host_id)->Report(information);	
}

void FriendFinderWorkerAdapter::RebuildCommonFriend(int host_id) {
	getProxy(host_id)->Rebuild(host_id);
}

void FriendFinderWorkerAdapter::RemoveCommonFriend(int host_id, int friend_id) {
	Info information;
	information.operation = DELETE;
	information.hostId = host_id;
	information.friendIds.push_back(friend_id);	
	getProxy(host_id)->Report(information);
}
