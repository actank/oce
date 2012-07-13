
#include <iostream>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>
#include "item.h"
#include "disk.h"

#include "String.h"
#include "../MceMonitorI.h"

using namespace std;
using namespace MceMonitor;
using namespace MceMonitor::Item;

Disk_Info::Disk_Info(const string& machine_id, const Resource& item)
	:Item(machine_id, item)
{
}

void Disk_Info::run()
{
	while (true) 
	{
		//cout<<"Disk run "<<endl;
        Log m;
		m.resourcegroup = _machine_id;
		m.resource = _item.name;
		m.type = _item.type;
		m.value = getDiskFreeRate(_item.parameters.at(0));
		m.time = time(NULL);
			
		MonitorManagerI::instance().getMonitorManager()->addLog(m);

		//cout<<"Disk run !"<<endl;
		//cout << m.value << endl;
		sleep(_item.interval);
	}
}

int Disk_Info::getDiskFree(string a)
{
	_Disk ret = getStatus();
	if (ret.find(a) != ret.end())
	{
		return ret[a].free;
	}else{
		return -1;
	}
}

float Disk_Info::getDiskFreeRate(string mountpoint)
{
	_Disk ret = getStatus();
	if (ret.find(mountpoint) != ret.end())
	{
		_MountPoint status = ret[mountpoint];
		//cout << "==> Free: " << status.free << "\t Used: " << status.used << "\tTotal: " << status.get_total() << "\tRate: " << 1000*status.free/status.get_total() << endl;
		return 100.0 * status.free / status.get_total();
	} else {
		return -1;
	}
}

_Disk Disk_Info::getStatus()
{
	_Disk ret;
	for(;;)
	{
		FILE *fp;
		int status;
		char tmp[PATH_MAX];
		fp = popen( "df -P", "r" );
		if (fp == NULL)
		{
			cout << "open failed." << endl;
			usleep(1000);
			continue;
		}
		fgets(tmp, PATH_MAX, fp);
		while ( fgets(tmp, PATH_MAX, fp) != NULL )
		{
			vector<string> line;
			line = MyUtil::StrUtil::split(tmp, "\t\n ");
			_MountPoint mountpoint;
			mountpoint.used = boost::lexical_cast<int>(line.at(2));
			mountpoint.free = boost::lexical_cast<int>(line.at(3));
			string name = line.at(5);
			ret.insert(make_pair(name, mountpoint));
		};
		status = pclose(fp);
		if (status == -1) continue;else break;
	}
	return ret;
}

