#include <vector>
#include <boost/lexical_cast.hpp>
#include <IceUtil/Mutex.h>
#include <fstream>
#include <iostream>
#include "sysinfo.h"
#include "mem.h"
#include "item.h"
#include "String.h"
#include "../MceMonitorI.h"

using namespace std;
using namespace MceMonitor;
using namespace MceMonitor::Item;

Mem_Info::Mem_Info(const string& machine_id, const Resource& item)
:Item(machine_id, item)
{
	if ( _item.parameters.at(0) == "MemUsedRate" )
	{
		_getResult = &Mem_Info::getMemUsedRate;
	}
	if ( _item.parameters.at(0) == "SwapUsedKb" )
	{
		_getResult = &Mem_Info::getSwapUsedKb;
	}
}

IceUtil::Mutex Mem_Info::_proc_mutex;

void Mem_Info::run()
{
	while (true) 
	{
		Log m;
		m.resourcegroup = _machine_id;
		m.resource = _item.name;
		m.type = _item.type;
		m.value = getResult();
		m.time = time(NULL);
		MonitorManagerI::instance().getMonitorManager()->addLog(m);
		//cout << m.value << endl;
		sleep(_item.interval);
	}
}

float Mem_Info::getMemUsedRate()
{
	_Mem stat = getStatus();
	float result = 100.0 * stat.get_used() / stat.total;
	return result;
}

float Mem_Info::getSwapUsedKb()
{
	return getStatus().get_swap_used();
}

float Mem_Info::getResult()
{
    return (this->*_getResult)();
}

_Mem Mem_Info::getStatus()
{
	_Mem ret;
    vector<string> field;
	string o;

	{
		IceUtil::Mutex::Lock lock(_proc_mutex);
		o = sysinfo().readfile("/proc/meminfo");
	}

   	vector<string> tok;
	tok = MyUtil::StrUtil::split(o, "\n\t ");
	map<string, string> lines;
	for( int i = 0; i < tok.size() / 3; i++ )
	{
		//cout << tok.at(i*3) << "=" << tok.at(i*3+1) << endl;
		lines[tok.at(i*3)] = tok.at(i*3+1);
	}

	if ( lines.find("MemTotal:") != lines.end() &&
		lines.find("MemFree:") != lines.end() &&
		lines.find("Buffers:") != lines.end() &&
		lines.find("Cached:") != lines.end() &&
		lines.find("SwapTotal:") != lines.end() && 
		lines.find("SwapFree:") != lines.end() )
	{
		ret.total = boost::lexical_cast<int>(lines["MemTotal:"]);
		ret.free = boost::lexical_cast<int>(lines["MemFree:"]);
		ret.buffer = boost::lexical_cast<int>(lines["Buffers:"]);
		ret.cached = boost::lexical_cast<int>(lines["Cached:"]);
		ret.swaptotal = boost::lexical_cast<int>(lines["SwapTotal:"]);
		ret.swapfree = boost::lexical_cast<int>(lines["SwapFree:"]);
	}
	//cout << "swaptotal: " << ret.swaptotal << " swap free: " << ret.swapfree << " used: " << ret.get_swap_used() << endl;
	//cout << "memtotal: " << ret.total << " free: " << ret.free << " buffer: " << ret.buffer << endl;
    return ret;
}

