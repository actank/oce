#include "Tasks.h"
#include "FeedCacheLogicAdapter.h"
#include <iostream>
#include "PerformanceStat.h"
#include <vector>
#include <set>

using namespace std;
using namespace xce::feed;

void RejectTask::handle() {

	FeedCacheConfigPtr cfg = FeedCacheI::instance().getConfig();
	vector<Ice::Long>  rejectedIds;

	FeedCacheI::instance().printThreadPoolInfoByLevel();

	try {
		timeval beg, end;
		gettimeofday(&beg, NULL);


		if(cfg->enableLogicReject) {
			rejectedIds = FeedCacheLogicAdapter::instance().resortFeedItems(_uid, _fis);

			gettimeofday(&end, NULL);
			float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
			timeuse/=1000;
			PerformanceStat::instance().incRejectFeed(timeuse);
			MCE_INFO("RejectTask::put -->  resort uid:" << _uid << " _fis size:" << _fis.size() << " rejectedIds size:" << rejectedIds.size() << " time use:" << timeuse);
		} else {
			MCE_INFO("RejectTask::put -->  resort uid:" << _uid << " _fis size:" << _fis.size() << " enableLogicReject is false.");
		} 
	} catch(Ice::Exception& e) {
		MCE_WARN("RejectTask::put --> FeedCacheLogic resort exception. uid:" << _uid << "_fis size:" << _fis.size() << " e:" << e);
	} catch(Ice::Exception& e) {
		MCE_WARN("RejectTask::put --> FeedCacheLogic resort exception. uid:" << _uid << "_fis size:" << _fis.size() << " e:" << e.what());
	} catch(Ice::Exception& e) {
		MCE_WARN("RejectTask::put --> FeedCacheLogic resort exception. uid:" << _uid << "_fis size:" << _fis.size() << " unknown exception!");
	}

	if(0 == rejectedIds.size() && cfg->enableCacheReject) {
		int i = 0;
		for(::xce::feed::FeedItemSeq::const_reverse_iterator rit = _fis.rbegin(); i < 20 && rit != _fis.rend(); i++, rit++) {
			rejectedIds.push_back(rit->feed);
		} 
		MCE_INFO("RejectTask::put -->  resort uid:" << _uid << " _fis size:" << _fis.size() << "Logic rejectedIds size == 0. now rids:" << rejectedIds.size());
	}

	FeedCacheI::instance().reject(_uid, rejectedIds); 
}

