#ifndef PERFORMANCE_H_
#define PERFORMANCE_H_

#include "TaskManager.h"
#include "IceLogger.h"
#include "Singleton.h"
#include "FeedCacheI.h"

using namespace xce::feed;
class PerformanceStat : public MyUtil::Timer, public MyUtil::Singleton<PerformanceStat>{
public:
	static const int INTERVAL = 10*1000;

	PerformanceStat() : Timer(INTERVAL, 1){
		_hit_get_count = 0;
		_load_get_count = 0;
	
		_hit_get_max_delay = 0;
		_hit_get_min_delay = 0;
		_hit_get_sum_delay = 0;

		_load_get_max_delay = 0;
		_load_get_min_delay = 0;
		_load_get_sum_delay = 0;
	}
	void handle(){
		pair<Ice::Long, Ice::Long> count = FeedCacheI::instance().queryCacheCount();
		MCE_INFO("### PerformanceStat ### users:" << count.first << " feeds:" << count.second 
			<< " hit get:" << _hit_get_count 
			<< " max:" << _hit_get_max_delay << " min:" << _hit_get_min_delay << " avg:" << _hit_get_sum_delay/(_hit_get_count+1)
			<< " load get:" << _load_get_count
			<< " max:" << _load_get_max_delay << " min:" << _load_get_min_delay << " avg:" << _load_get_sum_delay/(_load_get_count+1)
			);	
		_hit_get_count = 0;
		_load_get_count = 0;
	
		_hit_get_max_delay = 0;
		_hit_get_min_delay = 0;
		_hit_get_sum_delay = 0;

		_load_get_max_delay = 0;
		_load_get_min_delay = 0;
		_load_get_sum_delay = 0;

	}
	void incHitGet(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_hit_get_count++;
		_hit_get_sum_delay += delay;
		if(delay > _hit_get_max_delay || _hit_get_max_delay == 0){
			_hit_get_max_delay = delay;
		}
		if(delay < _hit_get_min_delay || _hit_get_min_delay == 0){
			_hit_get_min_delay = delay;
		}

	}
	void incLoadGet(float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		_load_get_count++;
		_load_get_sum_delay += delay;
		if(delay > _load_get_max_delay || _load_get_max_delay == 0){
			_load_get_max_delay = delay;
		}
		if(delay < _load_get_min_delay || _load_get_min_delay == 0){
			_load_get_min_delay = delay;
		}


	}
private:
	IceUtil::Mutex _mutex;
	int _hit_get_count;
	int _load_get_count;
	
	float _hit_get_max_delay;
	float _hit_get_min_delay;
	float _hit_get_sum_delay;

	float _load_get_max_delay;
	float _load_get_min_delay;
	float _load_get_sum_delay;
};

#endif
