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

std::string readFile(const std::string&);
std::string trim(const std::string& s);

namespace monitor {


class Status{
public:
	virtual ~Status() {}
	virtual void report() = 0;
protected:
	std::string getName();
};

class MemoryStatus:public Status{
public:
	virtual ~MemoryStatus() {}
	virtual void report();
};

class CpuStatus:public Status{
public:
	CpuStatus():flag_(false){}
	virtual ~CpuStatus() {}
  virtual void report();
private:
  bool flag_;
  long user_;
  long nice_;
  long system_;
  long idle_;
  long iowait_;
  long irq_;
  long softirq_;
};

class LiveStatus:public Status{
public:
	virtual ~LiveStatus() {}
	virtual void report();
};

class LoadAvgStatus:public Status{
public:
	virtual ~LoadAvgStatus() {}
	virtual void report();
private:
  int getNumCore();
};

class DiskStatus:public Status{
public:
	virtual ~DiskStatus() {}
	virtual void report();
};
}
#endif
