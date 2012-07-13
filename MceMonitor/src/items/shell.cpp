
#include <iostream>
#include <vector>
#include <map>
#include <boost/lexical_cast.hpp>
#include "item.h"
#include "shell.h"
#include "String.h"
#include "../MceMonitorI.h"

using namespace std;
using namespace MceMonitor;
using namespace MceMonitor::Item;

Shell_Info::Shell_Info(const string& machine_id, const Resource& item)
	:Item(machine_id, item)
{
}

void Shell_Info::run()
{
	while (true) 
	{
        //cout << "Shell Info run"<< endl;
		Log m;
		m.resourcegroup = _machine_id;
		m.resource = _item.name;
		m.type = _item.type;
		try{
			m.value = getResult(_item.parameters.at(0), boost::lexical_cast<int>(_item.parameters.at(1)));
		}catch(...)
		{
			m.value = -1;
		}
		m.time = time(NULL);
		
		MonitorManagerI::instance().getMonitorManager()->addLog(m);

        //cout << "Shell Info run():"<< m.value <<"!"<< endl;
		sleep(_item.interval);
	}
}

int Shell_Info::getResult(const string& cmd, int atline)
{
	return getStatus(cmd, atline);
}

int Shell_Info::getStatus(const string& cmd, int atline)
{
	int ret = -1;
	FILE *fp;
	int status;
	char tmp[PATH_MAX];
	fp = popen( cmd.c_str(), "r" );
	if (fp == NULL)
	{
		cout << "open failed." << endl;
	}
	if ( fgets(tmp, PATH_MAX, fp) != NULL )
	{
		ret = boost::lexical_cast<int>(MyUtil::StrUtil::split(tmp,"\n").at(atline));
	}
	status = pclose(fp);
	//if (status == -1) continue;else break;
	return ret;
}

