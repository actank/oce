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

using namespace std;
using namespace boost;
using namespace MyUtil;
using namespace Ice;
using namespace xce::storm;
using namespace xce::monitor;

string readFile(const string &filename){
	ifstream ifile;
	ifile.open(filename.c_str(), ios::in);
	if (!ifile){
		cout<<"open fild:"<<filename<<"error"<<endl;
	}
	ostringstream content;
	char buf[1024];
	while(ifile.good()){
		ifile.getline(buf,1024);
		content<<buf<<endl;
	}
	ifile.close();
	ifile.clear();
	return content.str();
}

Status::~Status(){}

/*void Status::report(){
	//define datas
	map<string,string> datas;
	datas["name"] = getName();
	datas["item"] = getItem();
	datas["type"] = getType();
	check(datas);
	//report
	TopicI topic("XceMonitorStorm","Monitor");
	XiaoNeiMonitorPrx monitor = XiaoNeiMonitorPrx::uncheckedCast(topic.getPublisher());
	monitor->report(datas);
}*/

string Status::getName(){
/*
	char name[256];
	gethostname(name,256);
	return string(name);
*/
	char* nickname=getenv("NICKNAME");
	if (nickname) {
		return nickname;
	} else {
		char name[256];
		gethostname(name,256);
		return string(name);
	}
}

string Status::getItem(){
	return "default";
}

string Status::getType(){
	return "default";
}

MemoryStatus::~MemoryStatus(){}

string MemoryStatus::getType() {
	return "Memory";
}

void MemoryStatus::report(){
        //define datas
        map<string,string> datas;
        datas["name"] = getName();
        datas["item"] = getItem();
        datas["type"] = getType();
        check(datas);
        //report
        TopicI topic("XceMonitorStorm","MemoryMonitor");
        XiaoNeiMonitorPrx monitor = XiaoNeiMonitorPrx::uncheckedCast(topic.getPublisher());
        monitor->report(datas);
}

void MemoryStatus::check(map<string,string>& datas){
	string content=readFile("/proc/meminfo");
	vector<string> fields;
	fields=StrUtil::split(content,"\n\t ");
	map<string,string> lines;
	for(unsigned int i=0;i<fields.size()/3;i++){
		lines[fields.at(i*3)]=fields.at(i*3+1);
	}
	if(lines.find("MemTotal:")!=lines.end()&&lines.find("MemFree:")!=lines.end()&&lines.find("Buffers:")!=lines.end()&&lines.find("Cached:")!=lines.end()&&lines.find("SwapTotal:")!=lines.end()&&lines.find("SwapFree:")!=lines.end()){
		datas["_memtotal"] = lines["MemTotal:"];
		datas["_memfree"] = lines["MemFree:"];
		datas["_buffers"] = lines["Buffers:"];
		datas["_cached"] = lines["Cached:"];
		datas["_swaptotal"] = lines["SwapTotal:"];
		datas["_swapfree"] = lines["SwapFree:"];
		datas["value"] = boost::lexical_cast<string>(boost::lexical_cast<int>(datas["_memfree"])+boost::lexical_cast<int>(datas["_buffers"])+boost::lexical_cast<int>(datas["_cached"])-boost::lexical_cast<int>(datas["_swaptotal"])+boost::lexical_cast<int>(datas["_swapfree"]));
	}else{
		cout << "gain memory error"<<endl;
		return;
	}
}

/*CpuStatus::~CpuStatus(){}

void CpuStatus::check(map<string,string>& datas){
	string content=readFile("/proc/stat");
	vector<string> fields;
	fields=StrUtil::split(content," \n");
	datas["_user"] = fields[1];
	datas["_nice"] = fields[2];
	datas["_system"] = fields[3];
	datas["_idle"] = fields[4];
	datas["_iowait"] = fields[5];
	datas["_irq"] = fields[6];
	datas["_softirq"] = fields[7];
	datas["CpuIdleRate"] = lexical_cast<string>(getIdleRate());
	datas["CpuIOWRate"] = lexical_cast<string>(getIOWRate());
}

Long CpuStatus::getTotal(){
	return _user+_nice+_system+_idle+_iowait+_irq+_softirq;
}

CpuStatus& CpuStatus::operator = (const CpuStatus& a){
	_user = a._user;
	_nice = a._nice;
	_system=a._system;
	_idle=a._idle;
	_iowait=a._iowait;
	_irq=a._irq;
	_softirq=a._softirq;
	return *this;
}

CpuStatus operator - (const CpuStatus& a,const CpuStatus& b){
	CpuStatus c;
	c._user=a._user-b._user;
	c._nice=a._nice-b._nice;
	c._system=a._system-b._system;
	c._idle=a._idle-b._idle;
	c._iowait=a._iowait-b._iowait;
	c._irq=a._irq-b._irq;
	c._softirq=a._softirq-b._softirq;
	return c;
}

int CpuStatus::getIdleRate(){
	CpuStatus status=*this-*_last;
	Long total=status.getTotal();
	int ret=-1;
	if(total!=0L){
		ret=static_cast<int>(static_cast<float>(status._idle)/static_cast<float>(total)*10000);
	}
	return ret;
}

int CpuStatus::getIOWRate(){
	CpuStatus status=*this-*_last;
	Long total=status.getTotal();
	int ret=-1;
	if(total){
		ret=static_cast<int>(static_cast<float>(status._iowait)/static_cast<float>(total)*10000);
	}
	return ret;
}
*/
LiveStatus::~LiveStatus(){}

void LiveStatus::report(){
        //define datas
        map<string,string> datas;
        datas["name"] = getName();
        datas["item"] = getItem();
        datas["type"] = getType();
        check(datas);
        //report
        TopicI topic("XceMonitorStorm","LiveMonitor");
        XiaoNeiMonitorPrx monitor = XiaoNeiMonitorPrx::uncheckedCast(topic.getPublisher());
        monitor->report(datas);
}

string LiveStatus::getType() {
	return "Live";
}

void LiveStatus::check(map<string,string>& maps){
	maps["Live"] = "";
}

LoadAvgStatus::~LoadAvgStatus(){}

void LoadAvgStatus::report(){
        //define datas
        map<string,string> datas;
        datas["name"] = getName();
        datas["item"] = getItem();
        datas["type"] = getType();
        check(datas);
        //report
        TopicI topic("XceMonitorStorm","LoadMonitor");
        XiaoNeiMonitorPrx monitor = XiaoNeiMonitorPrx::uncheckedCast(topic.getPublisher());
        monitor->report(datas);
}

string LoadAvgStatus::getType(){
	return "Load";
}

void LoadAvgStatus::check(map<string,string>& maps){
	double o[3];
	if (3==getloadavg(o,3))
	{
		maps["LoadAvg1"] = lexical_cast<string>(static_cast<int>(o[0]*100));
		maps["LoadAvg5"] = lexical_cast<string>(static_cast<int>(o[1]*100));
		maps["LoadAvg15"] = lexical_cast<string>(static_cast<int>(o[2]*100));
	}
	maps["value"] = lexical_cast<string>((int)(o[0]*100/getCpuCount()));
}

int LoadAvgStatus::getCpuCount(){
	int i;
	FILE *f=popen("cat /proc/cpuinfo |grep processor |wc -l","r");
	fscanf(f,"%d",&i);
	pclose(f);
	return i;
}

Long MountPoint::getTotal(){
	return _used+_free;
}

DiskStatus::~DiskStatus(){}

void DiskStatus::report(){
        //define datas
        map<string,string> datas;
        datas["name"] = getName();
        datas["item"] = getItem();
        datas["type"] = getType();
        check(datas);
        //report
        TopicI topic("XceMonitorStorm","DiskMonitor");
        XiaoNeiMonitorPrx monitor = XiaoNeiMonitorPrx::uncheckedCast(topic.getPublisher());
        monitor->report(datas);
}

string DiskStatus::getType(){
	return "Disk";
}

void  DiskStatus::check(map<string,string>& maps){
	FILE *fp;
	char tmp[PATH_MAX];
	while(true){
		fp=popen("df -P","r");
		if(fp==NULL){
			continue;
		}
		fgets(tmp,PATH_MAX,fp);
		int man=INT_MIN;
		string manname;
		while(fgets(tmp,PATH_MAX,fp)!=NULL){
			vector<string> line;
			line=StrUtil::split(tmp,"\t\n ");
			MountPoint point;
			point._used=lexical_cast<int>(line.at(2));
			point._free=lexical_cast<int>(line.at(3));
			string name=line.at(5);
			if(man<(point._used*100/(point._used+point._free))){
				man=point._used*100/(point._used+point._free);
				manname=name;
			}
			_mountmap[name]=point;
		}
		if(pclose(fp)==-1){
			continue;
		}
		maps["value"]=lexical_cast<string>(man);
		break;
	}

	Long freeTotal = 0;
	Long usedTotal = 0;
	for(map<string,MountPoint>::iterator it=_mountmap.begin();it!=_mountmap.end();it++){
		freeTotal += getFree(it->first);
		usedTotal += getUsed(it->first);
	}
	maps["DiskFree"] = lexical_cast<string>(freeTotal);
	maps["DiskUsed"] = lexical_cast<string>(usedTotal);
	if( (freeTotal+usedTotal) == 0 ){
		maps["DiskFreeRate"] = lexical_cast<string>(0);
	} else {
		maps["DiskFreeRate"] = lexical_cast<string>(10000*freeTotal/(freeTotal+usedTotal));
	}
}

Long DiskStatus::getFree(const string& name){
	if(_mountmap.find(name)!=_mountmap.end()){
		return _mountmap[name]._free;
	}else{
		return 0;
	}
}

Long DiskStatus::getUsed(const string& name){
	if(_mountmap.find(name)!=_mountmap.end()){
		return _mountmap[name]._used;
	}else{
		return 0;
	}
}

Long DiskStatus::getFreeRate(const string& name){
	if (_mountmap.find(name)!=_mountmap.end()){
		MountPoint status= _mountmap[name];
		return static_cast<Long>(10000*status._free/status.getTotal());
	}else{
		return 0;
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
                //CpuStatus cpu(new CpuStatus);
                DiskStatus disk;
                LoadAvgStatus load;
                LiveStatus live;
                while(true){
                        memory.report();
                        //cpu.report();
                        disk.report();
                        load.report();
                        live.report();
                        sleep(10);
                }
        }else{
                return 0;
        }
    return 0;
}

