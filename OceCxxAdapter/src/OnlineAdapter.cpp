#include "OnlineAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace xce::online::adapter;
using namespace std;

//---------------------------------------------------------------------------
OnlineManagerPrx OnlineAdapter::getOnlineManager(int id) {
	return OnlineManagerPrx::uncheckedCast(locate<OnlineManagerPrx>(_managers, "M", id, TWO_WAY)->ice_timeout(1000));
}

OnlineManagerPrx OnlineAdapter::getOnlineManagerOneway(int id) {
	return locate<OnlineManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

void OnlineAdapter::intialize() {

	_dispatchThread = new TrackDispatcher(*this);
	_dispatchThread->start(65535).detach();
}

void OnlineAdapter::ActiveUsers(int userId) {

	MCE_DEBUG("[OnlineAdapter]:new user active : " << userId);
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);	
	_activeids.push_back(userId);
}


void OnlineAdapter::ActiveUsers(const vector<int>& userIds) {
	if ( userIds.size() >= 0 ) {
		getOnlineManager(userIds[0])->UserActivity(userIds);
	}
}

map<int, int> OnlineAdapter::getOnlineStat(const vector<int>& userIds) {
	if ( userIds.size() >= 0 ) {
		return getOnlineManager(userIds[0])->UserOnlineStat(userIds);
	}
	else {
		map<int, int> map0;
		return map0;
	}
}

void OnlineAdapter::retrive(vector<int>& toInvokeCache) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	while( _activeids.empty() ) {
		timedWait(IceUtil::Time::seconds(1));
		if( toInvokeCache.size() != 0 ){
			break;
		}
	}
	toInvokeCache.insert(toInvokeCache.end(), _activeids.begin(), _activeids.end());
	_activeids.clear();
}
