#ifndef PERFORMANCESTAT_H_
#define PERFORMANCESTAT_H_

#include "Singleton.h"
#include "TaskManager.h"
#include <IceUtil/Mutex.h>
#include "Refactor.h"

using namespace MyUtil;

class PerfromanceStat : public MyUtil::Timer, public Singleton<PerfromanceStat>{

public:
	PerfromanceStat() :
		Timer(10*1000) {
		start_time = ::time(NULL);
		total_stop = 0;
		total_stop_per_tenseconds = 0;
		active_stop_per_tenseconds = 0;
		passive_stop_per_tenseconds = 0;
		conn_per_tenseconds = 0;
		total_conns = 0;
	}
	;
	virtual void handle() {
		Ice::Long ts, tsp, asp, psp;
		Ice::Long tc, cp;
		{
			IceUtil::Mutex::Lock lock(_mutex);
			ts = total_stop;
			tsp = total_stop_per_tenseconds;
			asp = active_stop_per_tenseconds;
			psp = passive_stop_per_tenseconds;
			total_stop_per_tenseconds = 0;
			active_stop_per_tenseconds = 0;
			passive_stop_per_tenseconds = 0;

			tc = total_conns;
			cp = conn_per_tenseconds;
			conn_per_tenseconds = 0;
		}
		Ice::Long cc = _refactor->connectionPoolSize();
		time_t t = ::time(NULL) - start_time;
		MCE_INFO(	"~~up_time(s):" << t
					<< ", total conns:" << tc
					<< ", cur_conns:" << cc
					<< ", conn_per_tenseconds:" << cp

					<< ", tatal_stops:" << ts
					<< ", active_stop_per:" << asp
					<< ", passive_stop_per:" << psp
					<< ", total_stop_per:" << tsp
				 );
		//TaskManager::instance().schedule(this);
	}
	void ConnectionStarted(){
		IceUtil::Mutex::Lock lock(_mutex);
		conn_per_tenseconds++;
		total_conns++;
	}
	void ConnectionStopped(int active){
		IceUtil::Mutex::Lock lock(_mutex);
		if(active){
			active_stop_per_tenseconds++;
		}else{
			passive_stop_per_tenseconds++;
		}
	}
	void ConnectionStopped(){
		IceUtil::Mutex::Lock lock(_mutex);
		total_stop++;
		total_stop_per_tenseconds++;
	}
	void SetRefactor(xce::nio::RefactorPtr refactor){
		_refactor = refactor;
	}
private:
	xce::nio::RefactorPtr _refactor;

	time_t start_time;
	Ice::Long total_stop;
	Ice::Long total_stop_per_tenseconds;
	Ice::Long active_stop_per_tenseconds;
	Ice::Long passive_stop_per_tenseconds;

	Ice::Long conn_per_tenseconds;
	Ice::Long total_conns;


	IceUtil::Mutex _mutex;
};

#endif
