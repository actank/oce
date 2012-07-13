/*
 *  MonitorClient.cpp
 *  MonitorClient
 *
 *  Created by Viwox on 08-12-14.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "MonitorClient.h"
#include <boost/lexical_cast.hpp>
#include "util/cpp/String.h"
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>
#include <stdlib.h>
#include "XceMonitor.h"
#include "IceLogger.h"
#include <unistd.h>
#include "stdlib.h"
#include <MonitorAdapter.h>

using boost::lexical_cast;
using monitor::MonitorDispatcherAdapter;
using monitor::Status;
using monitor::MemoryStatus;
using monitor::LiveStatus;
using monitor::CpuStatus;
using monitor::LoadAvgStatus;
using monitor::DiskStatus;
using MyUtil::StrUtil;

string readFile(const string &filename){
	ifstream ifile;
	ifile.open(filename.c_str(), ios::in);
	if (!ifile){
		cout<<"open file: "<<filename<<" error"<<endl;
    return "";
	}
	ostringstream content;
	while(ifile.good()){
    string buf;
		getline(ifile, buf);
		content<<buf<<endl;;
	}
	ifile.close();
	return content.str();
}

string trim(const string& s) {
  ostringstream oss;
  for (unsigned int i = 0; i < s.size(); ++i) {
    if (s[i] != '\r' && (int)s[i] !='\n') {
      oss << s[i];
    }
  }
  return oss.str();
}

string Status::getName(){
  return trim(readFile(string(getenv("HOME"))+"/.nickname"));
}

void MemoryStatus::report(){
  string content=readFile("/proc/meminfo");
  map<string, string> lines;
  vector<string> fields = StrUtil::split(content, "\n\t ");
  for (unsigned int i = 0; i < fields.size() / 3; ++i) {
    lines[fields.at(i * 3)] = fields.at(i * 3 + 1);
  }
	
  if(lines.find("MemTotal:") != lines.end()
    && lines.find("MemFree:") != lines.end()
    && lines.find("Buffers:") != lines.end()
    && lines.find("Cached:") != lines.end()
    && lines.find("SwapTotal:") != lines.end()
    && lines.find("SwapFree:")!=lines.end()) {
    ostringstream oss;
    oss << "Name=" << getName() 
      << " MemTotal=" << lines["MemTotal:"]
      << " MemFree=" << lines["MemFree:"]
      << " Buffers=" << lines["Buffers:"]
      << " Cached=" << lines["Cached:"]
      << " SwapTotal=" << lines["SwapTotal:"]
      << " SwapFree=" << lines["SwapFree:"];
    MonitorDispatcherAdapter::instance().report("MASHINE_MEMORY", oss.str());
	}else{
		cout << "gain memory error"<<endl;
		return;
	}
}

void CpuStatus::report() {
	string content=readFile("/proc/stat");
	vector<string> fields=StrUtil::split(content," \n");
  long user = lexical_cast<long>(fields[1]);
  long nice = lexical_cast<long>(fields[2]);
  long system = lexical_cast<long>(fields[3]);
  long idle = lexical_cast<long>(fields[4]);
  long iowait = lexical_cast<long>(fields[5]);
  long irq = lexical_cast<long>(fields[6]);
  long softirq = lexical_cast<long>(fields[7]);
  if (!flag_) {
    flag_ = true;
  } else {
    ostringstream oss;
    oss << "Name=" << getName()
    << " User=" << (user-user_)
    << " Nice=" << (nice-nice_)
	  << " System=" << (system-system_)
	  << " Idle=" << (idle-idle_)
	  << " Iowait=" << (iowait-iowait_)
	  << " Irq=" << (irq-irq_)
	  << " Softirq=" << (softirq-softirq_);
    MonitorDispatcherAdapter::instance().report("MASHINE_CPU", oss.str());
  }
  user_ = user;
  system_ = system;
  nice_ = nice;
  idle_ = idle;
  iowait_ = iowait;
  irq_ = irq;
  softirq_ = softirq;
}

void LiveStatus::report(){
  MonitorDispatcherAdapter::instance().report("MASHINE_LIVE", getName());
}

void LoadAvgStatus::report(){
  ostringstream oss;
  double o[3];
  getloadavg(o, 3);
  oss << "Name=" << getName()
    << " LoadAvg1=" << lexical_cast<string>(static_cast<int>(o[0]*100))
    << " LoadAvg5=" << lexical_cast<string>(static_cast<int>(o[1]*100))
    << " LoadAvg15=" << lexical_cast<string>(static_cast<int>(o[2]*100))
    << " CpuNumber=" << getNumCore();
  MonitorDispatcherAdapter::instance().report("MASHINE_LOAD", oss.str());
}

int LoadAvgStatus::getNumCore(){ 
  string file = readFile("/proc/cpuinfo");
  deque<string> strings;
  boost::find_all(strings, file, "processor");
  return strings.size();
}

void DiskStatus::report(){
  FILE *fp;
  char tmp[PATH_MAX];
  while (true) {
    fp=popen("df -P", "r");
    if (fp == NULL) {
      continue;
    }
    fgets(tmp, PATH_MAX, fp);
    string manname;
    while (fgets(tmp,PATH_MAX,fp)!=NULL) {
      ostringstream oss;
      oss << "Name="<<getName() << " " << string(tmp);
      MonitorDispatcherAdapter::instance().report("MASHINE_DISK", trim(oss.str()));
    }
    pclose(fp);
    break;
  }
}

int main (int argc, char * const argv[]) {
	pid_t pid;
  pid=fork();
  if (pid<0){
    cout<<"ERROR IN FORK"<<endl;
  }
  if (pid==0){
    MemoryStatus memory;
    CpuStatus cpu;
    DiskStatus disk;
    LoadAvgStatus load;
    LiveStatus live;
    while(true){
      try {
      memory.report();
      cpu.report();
      disk.report();
      load.report();
      live.report();
      } catch (...) {
      }
      sleep(10);
    }
  }else{
    return 0;
  }
  return 0;
}

