#include <vector>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "MceMonitor.h"
#include "item.h"
#include "loadavg.h"
#include "../MceMonitorI.h"

using namespace std;
using namespace MceMonitor;
using namespace MceMonitor::Item;

Loadavg_Info::Loadavg_Info(const string& machine_id, const Resource& item)
		: Item(machine_id, item)
{
}

void Loadavg_Info::run()
{
	while (true) 
	{
        Log m;
		m.resourcegroup = _machine_id;
		m.resource = _item.name;
		m.type = _item.type;
		if(_item.parameters.size() != 1)
		{
			m.value = getLoadavg1();
		}
		else
		{
			if(_item.parameters.at(0)=="15")
			{
				m.value = getLoadavg15();
			}
			else if(_item.parameters.at(0)=="5")
			{
				m.value = getLoadavg5();
			}
			else 
			{
				m.value = getLoadavg1();
			}
		}
		m.time = time(NULL);
		MonitorManagerI::instance().getMonitorManager()->addLog(m);
		//cout << m.value << endl;
	
        sleep(_item.interval);
	}
}

float Loadavg_Info::getLoadavg1()
{
	return getStatus().min1;
}

float Loadavg_Info::getLoadavg5()
{
	return getStatus().min5;
}

float Loadavg_Info::getLoadavg15()
{
	return getStatus().min15;
}

_Loadavg Loadavg_Info::getStatus()
{
	_Loadavg ret;
	double o[3];
	if ( 3 == getloadavg(o, 3) )
	{
		ret.min1 = boost::lexical_cast<float>( o[0] );
		ret.min5 = boost::lexical_cast<float>( o[1] );
		ret.min15 = boost::lexical_cast<float>( o[2] );
	}
    return ret;
}
