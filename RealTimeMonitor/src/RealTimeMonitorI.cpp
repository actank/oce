/*
 * RealTimeMonitorI.cpp
 *
 *  Created on: 2010-4-12
 *      Author: ld
 */

#include "RealTimeMonitorI.h"
#include <stdlib.h>
#include <ctime>
#include <IceUtil/Mutex.h>
#include <sstream>
#include "QueryRunner.h"
#include "ServiceI.h"
#include "IceLogger.h"

using namespace xce::monitor;
using namespace com::xiaonei::xce;


void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  RealTimeMonitorI& monitor = RealTimeMonitorI::instance();
  service.getAdapter()->add(&monitor,
        service.createIdentity("M", ""));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string phone_no = props->getPropertyWithDefault("Service.RealTimeMonitor.PhoneNO",
      "");
  MCE_DEBUG("phone_no:" << phone_no);
  monitor.InitPhoneNoVec(phone_no);

  TaskManager::instance().scheduleRepeated(new SaveAndAlarmTimer());
}


namespace xce {

namespace monitor {

void SaveAndAlarmTimer::handle() {
  RealTimeMonitorI::instance().TraverseMap();  
}
 


bool ExceptionMap::InsertException(string key, ExceptionItem item) {
	IceUtil::Mutex::Lock lock(mutex_);
  map<string, ExceptionItem>::iterator it = exception_map_.find(key);
	if (it != exception_map_.end()) {
		it->second.AddNumPerMin();
		it->second.AddNumPerHour();
		return false;
	} else {
		exception_map_[key] = item;
		return true;
	}
}


void ExceptionMap::UpdateMapAndSaveDb(bool flag){
  for(map<string,ExceptionItem>::iterator it = exception_map_.begin(); it != exception_map_.end(); ++it) {
    MCE_INFO("ExceptionMap::UpdateMapAndSaveDb --> service_name:" << it->second.service_name()
        << " file_name:" << it->second.file_name() << " class_name:"
        << it->second.class_name() << " method:" << it->second.method()
        << " line_no:" << it->second.line_no() << " remote_addr:" << it->second.remote_addr()
        << " num_per_min:" << it->second.num_per_min()
        << " num_per_hour:" << it->second.num_per_hour());
    if(it->second.num_per_min() >= kThresholdMin) {
      ostringstream msg;
      msg << it->second.service_name() << ":" << it->second.num_per_hour() << " exceptions.";
      RealTimeMonitorI::instance().ReportException(msg.str());
    }
    it->second.ClearNumPerMin();
  }

  if (flag && !exception_map_.empty()) {
    Statement sql;
    sql << "insert into " << kDbTableName 
      << " (server_name, file_name, class_name, method, line_no, remote_addr, num_per_hour, time) values ";
    for(map<string,ExceptionItem>::iterator it = exception_map_.begin(); it != exception_map_.end(); ++it){
      if(it != exception_map_.begin()){
        sql << ",";
      }
      sql  << "(" << mysqlpp::quote << it->second.service_name() << "," << mysqlpp::quote <<it->second.file_name()
        << "," << mysqlpp::quote <<  it->second.class_name() << "," << mysqlpp::quote << it->second.method()
        << "," << it->second.line_no() << "," << mysqlpp::quote << it->second.remote_addr()
        << "," << it->second.num_per_hour() 
        << "," << " now() "  << ")";
    }
    exception_map_.clear();//clear map per hour
   
    try{
      QueryRunner(kDbLogicName, CDbWServer).store(sql);
    } catch (const Ice::Exception& e) {
      MCE_INFO("ExceptionMap::UpdateMapAndSvaeDb --> sql ice exception:" << e);
    } catch (const std::exception& e){
      MCE_INFO("ExceptionMap::UpdateMapAndSvaeDb --> sql exception:" << e.what());
    }

  }

}


void RealTimeMonitorI::ReportException(string msg) {
  for (vector<string>::iterator it = phone_no_vec_.begin(); it != phone_no_vec_.end(); it++) {
    string phone_no = *it;
    ostringstream cmd;
    cmd << "wget -q -O /dev/null \"http://10.22.198.81:2000/receiver?number="
    << phone_no << "&message=" << msg << "\"";
    MCE_DEBUG("RealTimeMonitorI::ReportException --> msg:" << cmd.str()); 
   
    system(cmd.str().c_str());
  }
}


RealTimeMonitorI::~RealTimeMonitorI() {
	// TODO Auto-generated destructor stub
}



bool RealTimeMonitorI::RecordException(const MonitoredException& 
    exception_content	, const Ice::Current& cur) {
	string remote_addr = cur.con->toString();
	MCE_INFO("ExceptionMonitorI::RecordException --> service:"
      <<exception_content.serviceName
			<< " file_name:" <<exception_content.fileName << " class_name:" << exception_content.className << " method:" << exception_content.method
			<< " line_no:" << exception_content.lineNo << " type:" << exception_content.type << " para:" << exception_content.para 
      << " remote_addr:" << remote_addr);

	ExceptionItem item(exception_content.serviceName, 
      exception_content.fileName, 
      exception_content.className, 
      exception_content.method, 
      exception_content.lineNo, 
      remote_addr);
  ostringstream  map_key;
  map_key << exception_content.fileName << ":" << exception_content.lineNo;
	bool ret = exception_container_.InsertException(map_key.str(), item);
  return ret;
}

void RealTimeMonitorI::TraverseMap() {
  MCE_DEBUG("RealTimeMonitorI::TraverseMap --> prehour:" << hour);

  int now_hour = GetNowHour();
  MCE_DEBUG("RealTimeMonitorI::TraverseMap --> nowhour:" << now_hour);

  if (hour != now_hour) {
    hour = now_hour;
    exception_container_.UpdateMapAndSaveDb(true);  
  } else {
    exception_container_.UpdateMapAndSaveDb(false);
  } 	
  
}



void RealTimeMonitorI::InitPhoneNoVec(string phone_no_str) {
  if (!phone_no_str.empty()) {
    string::size_type pos;
    string no;
    
    pos =  phone_no_str.find(",");
    while(pos != string::npos) {
      no =  phone_no_str.substr(0,pos);
      phone_no_vec_.push_back(no);
      phone_no_str =  phone_no_str.substr(pos + 1);
      MCE_DEBUG("RealTimeMonitorI::InitPhoneNoVec --> phone no:" << no); 
      pos =  phone_no_str.find(",");
    }
    if (!phone_no_str.empty()) {
      MCE_DEBUG("RealTimeMonitorI::InitPhoneNoVec --> last phone no:" << phone_no_str); 
      phone_no_vec_.push_back(phone_no_str);
    }

  }
  
}

int RealTimeMonitorI::GetNowHour() {
  time_t now_time = time(NULL);
  struct tm* local_time = localtime(&now_time);
  return  local_time->tm_hour;
}

}

}
