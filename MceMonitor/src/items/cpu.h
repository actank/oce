#ifndef MCEMONITOR_ITEM_CPU_API
#define MCEMONITOR_ITEM_CPU_API

#include <Ice/Ice.h>
#include <iostream>
#include <IceUtil/Mutex.h>
#include "item.h"

namespace MceMonitor
{
namespace Item
{

typedef struct _CPU{
	Ice::Long cpu_user;
	Ice::Long cpu_nice;
	Ice::Long cpu_sys;
	Ice::Long cpu_idle;
	Ice::Long cpu_iow;
	Ice::Long cpu_xxx;
	Ice::Long cpu_yyy;
	Ice::Long cpu_zzz;
	friend _CPU operator-(_CPU a, _CPU b){
		_CPU c;
		c.cpu_user = a.cpu_user - b.cpu_user;
    	c.cpu_nice = a.cpu_nice - b.cpu_nice;
	    c.cpu_sys  = a.cpu_sys  - b.cpu_sys;
	    c.cpu_idle = a.cpu_idle - b.cpu_idle;
	    c.cpu_iow  = a.cpu_iow  - b.cpu_iow;
	    c.cpu_xxx  = a.cpu_xxx  - b.cpu_xxx;
	    c.cpu_yyy  = a.cpu_yyy  - b.cpu_yyy;
	    c.cpu_zzz  = a.cpu_zzz  - b.cpu_zzz;
		return c;
	};
	Ice::Long get_total(){
		return cpu_user + cpu_nice + cpu_sys + cpu_idle + cpu_iow + cpu_xxx + cpu_yyy + cpu_zzz;
	};
};

class CPU_Info : public Item
{
public:
	CPU_Info(const std::string& machine_id, const Resource&);
	virtual void run();
	int getIdleRate();
	int getIOWRate();
	Ice::Int getResult();
private:
	int (CPU_Info::*_getResult)();
	_CPU getStatus();
	_CPU _last_stat;
	static IceUtil::Mutex _proc_mutex;
};
};
};

#endif

