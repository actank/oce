#include "FriendFinderByActiveUserWorkerAdapter.h"

using namespace xce::friendfinder::adapter;

void FriendFinderByActiveUserWorkerAdapter::load() {
	getProxy(0)->load();
}
