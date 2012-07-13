#ifndef PERFORMANCE_H_
#define PERFORMANCE_H_

#include <cmath>
#include "TaskManager.h"
#include "IceLogger.h"
#include "Singleton.h"
#include "FeedItemI.h"
using namespace xce::feed;

class TimeStat{
        timeval _begin, _end;
public:
        TimeStat(){
                reset();
        }
        void reset(){
                gettimeofday(&_begin, NULL);
        }
        float getTime(bool rs = true){
                gettimeofday(&_end, NULL);
                float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
                                -_begin.tv_usec;
                timeuse/=1000;
                if(rs){
                	reset();
                }
                return timeuse;
        }
};

class StatData{
public:
	int count;
	float max;
	float min;
	float sum;
	map<int, int> costs;
public:
	StatData(){
		clear();
	}
	map<int, int> getCosts(){
		return costs;
	}
	void clear(){
		count = 0;
		max = 0;
		min = 0;
		sum = 0;
		costs.clear();
	}
	string toString(){
		ostringstream os;
		float avg = count>0 ? sum/count : sum;
		os << " count:" << count << " max:" << max << " min:" << min << " avg:" << avg;
		return os.str();
	}
};

class PerformanceStat : public MyUtil::Timer, public MyUtil::Singleton<PerformanceStat>{
public:
	static const int INTERVAL = 10*1000;

	PerformanceStat() : Timer(INTERVAL, 111){
		clear();
	}
	void clear(){
		map<string, StatData>::iterator it = _stats.begin();
		for(; it!=_stats.end(); ++it){
			it->second.clear();
		}
	}
	void handle(){
		MCE_INFO("### PerformanceStat ### --> usersize:" << FeedItemManagerI::instance().Size());
		IceUtil::Mutex::Lock lock(_mutex);
		map<string, StatData>::iterator it = _stats.begin();
		for(; it!=_stats.end(); ++it){
			ostringstream os;
			os << "### PerformanceStat ### ";
			os << it->first << " --> " << it->second.toString();
			MCE_INFO(os.str());
		}
		clear();	
	}
	void stat(const string& name, float delay){
		IceUtil::Mutex::Lock lock(_mutex);
		updateStatData(_stats[name], delay);
	}
	void updateStatData(StatData& data, float delay){
		data.costs[(int)round(delay)]++;
		if(data.count == 0){
			data.max = delay;
			data.min = delay;
		}
		data.count++;
		data.sum += delay;
		if(delay > data.max || data.max == 0){
			data.max = delay;
		}
		if(delay < data.min || data.min == 0){
			data.min = delay;
		}
	}
private:
	IceUtil::Mutex _mutex;
	map<string, StatData> _stats;
};

#endif
