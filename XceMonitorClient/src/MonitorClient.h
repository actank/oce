/*
 *  MonitorClient.h
 *  MonitorClient
 *
 *  Created by Viwox on 08-12-14.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _MONITOR_CLIENT_
#define _MONITOR_CLIENT_

#include "ServiceI.h"
#include "TopicI.h"


std::string readFile(const std::string&);

class Status{
public:
	virtual ~Status();
	virtual void report() = 0;
protected:
	virtual std::string getName();
	virtual std::string getItem();
	virtual std::string getType();
	virtual void check(map<std::string,std::string>&) = 0;
};

class MemoryStatus:public Status{
public:
	virtual ~MemoryStatus();
	virtual void report();
private:
	virtual void check(map<std::string,std::string>&);
	virtual std::string getType();
};

/*class CpuStatus:public Status{
public:
	CpuStatus():_last(NULL){}
	CpuStatus(CpuStatus* last):_last(last){}
	virtual ~CpuStatus();
private:
	void check(map<std::string,std::string>&);
	std::string getType(){
		return "Cpu";
	}
	Ice::Long getTotal();
	int getIdleRate();
	int getIOWRate();
	CpuStatus& operator = (const CpuStatus&);
	friend CpuStatus operator - (const CpuStatus&,const CpuStatus&);
};*/

class LiveStatus:public Status{
public:
	virtual ~LiveStatus();
	virtual void report();
private:
	virtual void check(map<std::string,std::string>&);
	virtual std::string getType();
};

class LoadAvgStatus:public Status{
public:
	virtual ~LoadAvgStatus();
	virtual void report();
private:
	virtual void check(map<std::string,std::string>&);
	int getCpuCount();
	virtual std::string getType();
};

struct MountPoint{
	Ice::Long _used;
	Ice::Long _free;
	Ice::Long getTotal();
};

class DiskStatus:public Status{
public:
	virtual ~DiskStatus();
	virtual void report();
private:
	virtual void check(map<std::string,std::string>&);
	virtual std::string getType();
	Ice::Long getFree(const std::string&);
	Ice::Long getUsed(const std::string&);
	Ice::Long getFreeRate(const std::string&);
	map<std::string,MountPoint> _mountmap;
};

#endif
