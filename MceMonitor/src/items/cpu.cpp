
#include "MceMonitor.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <IceUtil/Mutex.h>
#include "item.h"
#include "cpu.h"
#include "sysinfo.h"

#include "String.h"
#include "../MceMonitorI.h"

using namespace std;
using namespace MceMonitor;
using namespace MceMonitor::Item;

IceUtil::Mutex CPU_Info::_proc_mutex;

CPU_Info::CPU_Info(const string& machine_id, const Resource& item)
		:Item(machine_id, item)
{
	_last_stat = getStatus();
	if ( _item.parameters.at(0) == "Idle" )
	{
		_getResult = &CPU_Info::getIdleRate;
	}
	if ( _item.parameters.at(0) == "IOWait" )
	{
		_getResult = &CPU_Info::getIOWRate;
	}
}

void CPU_Info::run()
{
	while (true) 
	{
        Log m;
		m.resourcegroup = _machine_id;
		m.resource = _item.name;
		m.type = _item.type;
		m.time = time(NULL);
		m.value = -1;
		m.value = getResult() / 10.0;
		MonitorManagerI::instance().getMonitorManager()->addLog(m);
		MCE_DEBUG("CPU_Info:" << _item.type <<" "<< m.value);
		sleep(_item.interval);
	}
}

int CPU_Info::getResult()
{
	return (this->*_getResult)();
}

int CPU_Info::getIdleRate()
{
	_CPU now_stat = getStatus();
	_CPU dcpu = now_stat - _last_stat;
	int ret = -1;
	Ice::Long cpu_total = dcpu.get_total();
	if ( cpu_total != 0L ){
		//cout<<"[GATHER][getIdleRate]"<< dcpu.cpu_idle << " | " << cpu_total << endl;
		ret = 1000 * dcpu.cpu_idle / cpu_total;
	};
	_last_stat = now_stat;
	return ret;
};

int CPU_Info::getIOWRate()
{
	_CPU now_stat = getStatus();
	_CPU dcpu = now_stat - _last_stat;
	int ret = -1;
	Ice::Long cpu_total = dcpu.get_total();
	if ( cpu_total != 0L ){
		//cout<< dcpu.cpu_idle << " | " << dcpu.cpu_total << endl;
		ret = 1000 * dcpu.cpu_iow / cpu_total;
	};
	_last_stat = now_stat;
	return ret;
}

_CPU CPU_Info::getStatus()
{
	_CPU ret;
	vector<string> field;
	int failed=5;
	string str;

	{
		IceUtil::Mutex::Lock lock(_proc_mutex);
		str = sysinfo().readfile("/proc/stat");
	}

	field = MyUtil::StrUtil::split(str, " \n");
	if ( field.size() >= 8 )
	{
	}else{
		if( --failed < 0 )
		{
			cout << "[ERROR] Field Error size:" << field.size() << endl;
			for(int xxx=0;xxx<field.size();++xxx) cout << field.at(xxx) << "\t";
			cout << endl;
			return ret;
		}
		field.clear();
	}


	ret.cpu_user = boost::lexical_cast<Ice::Long>(field[1]);
	ret.cpu_nice = boost::lexical_cast<Ice::Long>(field[2]);
	ret.cpu_sys  = boost::lexical_cast<Ice::Long>(field[3]);
	ret.cpu_idle = boost::lexical_cast<Ice::Long>(field[4]);
	ret.cpu_iow  = boost::lexical_cast<Ice::Long>(field[5]);
	ret.cpu_xxx  = boost::lexical_cast<Ice::Long>(field[6]);
	ret.cpu_yyy  = boost::lexical_cast<Ice::Long>(field[7]);
	ret.cpu_zzz  = 0;

	return ret;
}
