#include <UserStateCacheAdapter.h>

std::map<int, xce::userstatecache::UserStateCachePtr> xce::userstatecache::UserStateCacheAdapter::GetUserStateCache(const std::vector<int>& ids) {
	if (ids.empty()) {
		return std::map<int, xce::userstatecache::UserStateCachePtr>();
	}
	size_t numcluster = getCluster();
	cout << "numcluster: "<<numcluster<<endl;
	if (numcluster==0 || numcluster==1) {
		cout << "if"<<endl;
		try {
			std::map<int, xce::userstatecache::UserStateCachePtr> result;
			xce::userstatecache::UserStateCacheNewDataMap someresult=getProxy(0)->getUserStateCacheNewList(ids);
			for (UserStateCacheNewDataMap::iterator it=someresult.begin();it!=someresult.end();++it) {
				result[it->first] = new xce::userstatecache::UserStateCache(it->second);
			}
			return result;
		} catch(...) {
			return std::map<int, xce::userstatecache::UserStateCachePtr>();
		}
	} else {
		cout << "else"<<endl;
		std::vector<std::vector<int> > seperatedids;
		seperatedids.resize(numcluster);
		for (std::vector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
			seperatedids.at((*it)%numcluster).push_back(*it);
		}
		try {
			std::map<int, UserStateCachePtr> result;
			for (size_t poscluster = 0; poscluster < numcluster; ++poscluster) {
				xce::userstatecache::UserStateCacheNewDataMap someresult = getProxy(poscluster)->getUserStateCacheNewList(ids);
				for (UserStateCacheNewDataMap::iterator it=someresult.begin();it!=someresult.end();++it) {
					result[it->first] = new xce::userstatecache::UserStateCache(it->second);
				}
			}
			return result;
		} catch (...) {
			return std::map<int, xce::userstatecache::UserStateCachePtr>();
		}
	}
}
