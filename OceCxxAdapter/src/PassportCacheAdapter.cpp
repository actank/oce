#include <PassportCacheAdapter.h>

using namespace ::xce::passportcache;
using namespace ::xce::clusterstate;
using namespace MyUtil;
using namespace std;

PassportCacheAdapter::PassportCacheAdapter() : _clientCS("ControllerPassportCache", 120, 300) {
	initialize();
}

void PassportCacheAdapter::initialize() {
	_clientCS.intialize();
}

bool PassportCacheAdapter::checkTicket(int userId, std::string key) {
	return _clientCS.getProxy(userId)->checkTicket(userId, key);
}

ObjectResultPtr PassportCacheAdapter::getPassportCache(vector<int> userIds) {
	if (userIds.empty()) {
		return new ObjectResult;
	}
	size_t numCluster = cluster();
	if (numCluster == 0 || numCluster == 1) {
		try {
			ObjectResultPtr result = _clientCS.getProxy(userIds.at(0))->getData(userIds);
			return result;
		} catch (...) {
			return new ObjectResult;
		}
	} else {
		vector<vector<int> > seperatedIds;
		seperatedIds.resize(numCluster);
		for (vector<int>::const_iterator it = userIds.begin(); it
				!= userIds.end(); ++it) {
			seperatedIds.at(*it % numCluster).push_back(*it);
		}
		try {
			ObjectResultPtr result = new ObjectResult;
			for (size_t posCluster = 0; posCluster < numCluster; ++posCluster) {
				ObjectResultPtr someResult = _clientCS.getProxy(posCluster)->getData(seperatedIds.at(posCluster));
				for (map<int64_t, ObjectPtr>::iterator it = someResult->data.begin(); it != someResult->data.end(); ++it) {
					result->data.insert(*it);
				}
			}
			return result;
		} catch (...) {
			return new ObjectResult;
		}
	}
}

bool PassportCacheAdapter::isValid(const Ice::ObjectPrx& proxy) {
        return PassportCacheManagerPrx::uncheckedCast(proxy)->isValid();
}

string PassportCacheAdapter::name() {
	return "PassportCache";
}

string PassportCacheAdapter::endpoints() {
	return "@PassportCache";
}

size_t PassportCacheAdapter::cluster() {
	return _clientCS.getCluster();
}
