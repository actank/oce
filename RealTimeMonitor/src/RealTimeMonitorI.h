/*
 * RealTimeMonitorI.h
 *
 *  Created on: 2010-4-12
 *      Author: ld
 */

#ifndef __MONITOR_REALTIMEMONITORI_H__
#define __MONITOR_REALTIMEMONITORI_H__


#include <string>
#include <vector>
#include <map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "TaskManager.h"
#include "Singleton.h"
#include "RealTimeMonitor.h"


namespace xce {

namespace monitor {

using std::string;
using std::map;
using std::cout;
using std::endl;


const int kThresholdMin = 3;
const int kThresholdHour = 20; 
//const int kInterval = 1000 * 60; 
const int kInterval =1000 * 60;
const string kDbTableName = "real_time_monitor";
const string kDbLogicName = "im_stat";


class SaveAndAlarmTimer : public MyUtil::Timer{ 
public:
  SaveAndAlarmTimer():Timer(kInterval) {}
  virtual void handle();
};



class ExceptionItem {
public:
	ExceptionItem(const std::string& service_name,
			const std::string& file_name, const std::string& class_name,
			const std::string& method, Ice::Int line_no, const std::string& remote_addr)
	    :service_name_(service_name),file_name_(file_name),
			class_name_(class_name),method_(method),line_no_(line_no),remote_addr_(remote_addr),
			num_per_min_(1),num_per_hour_(1) {}

  ExceptionItem():num_per_min_(1),num_per_hour_(1) {}

	int AddNumPerMin() {
		return ++num_per_min_;
	}

	void ClearNumPerMin() {
		num_per_min_=0;
	}

	int AddNumPerHour() {
		return ++num_per_hour_;
	}

	void ClearNumPerHour() {
		num_per_hour_=0;
	}

	int num_per_min() {
		return num_per_min_;
	}

	int num_per_hour() {
		return num_per_hour_;
	}

	string service_name() {
		return service_name_;
	}

	string file_name() {
			return file_name_;
	}

	string class_name() {
			return class_name_;
	}

	string method() {
			return method_;
	}

	int line_no() {
		return line_no_;
	}

	string remote_addr() {
		return remote_addr_;
	}




private:
	string service_name_;
	string file_name_;
	string class_name_;
	string method_;
	int line_no_;
	string remote_addr_;
	int num_per_min_;
	int num_per_hour_;


};

class ExceptionMap {
public:


	bool InsertException(string key,ExceptionItem item);
	//void UpdateMap();
  void UpdateMapAndSaveDb(bool flag);


private:
  map<string,ExceptionItem> exception_map_;
  IceUtil::Mutex mutex_;


};  



class RealTimeMonitorI: public RealTimeMonitor,
    public MyUtil::Singleton<RealTimeMonitorI>{
public:
	RealTimeMonitorI() {
    hour = GetNowHour();
  }

	virtual ~RealTimeMonitorI();

	virtual bool RecordException(const MonitoredException& exception_content, 
		  const Ice::Current& = Ice::Current());

	void TraverseMap();
	void ReportException(string msg);
  
  void InitPhoneNoVec(string phone_no_str);  

  static int GetNowHour();
 

private:
	ExceptionMap exception_container_;
  int hour;
  vector<string> phone_no_vec_;   

};

}

}

#endif /* __MONITOR_REALTIMEMONITORI_H__ */
