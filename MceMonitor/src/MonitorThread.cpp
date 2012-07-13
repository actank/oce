
#include "MonitorThread.h"
#include "MceMonitorI.h"
#include "MonitorStatus.h"

#include "Date.h"
#include "converter.h"

using namespace std;
using namespace MceMonitor;

MonitorThread::MonitorThread(const string& name, const Resource& res, MonitorStatus* status)
	: _resource(res), _status(status), _name(name),	_inCount(0), _outCount(0), _isNormal(true)
{
}

void
MonitorThread::run()
{
	while(true)
	{
		MCE_TRACE("MonitorThread: run");
		LogQueue recent = _status->getLog(_name, _resource.name, 2);
		if(recent.size() != 2)
		{
			MCE_WARN("Service just initialized. Still not functional. "<<_name<<" "<<_resource.name<<" type="<<_resource.type);
			sleep(_resource.interval);
			continue;
		}
		
		Log recentAvg;
		recentAvg.resourcegroup = _name;
		recentAvg.resource = _resource.name;
		recentAvg.type = _resource.type;
		recentAvg.value = 0;
		recentAvg.time = 0;

		for(LogQueue::iterator it = recent.begin();
			it != recent.end();
			++it)
		{
			recentAvg.value += it->value/2.0;
			recentAvg.time += it->time/2;
		};
		MCE_INFO("MonitorThread: recentAvg: " << log2str(recentAvg));

		// judege the value
		if(recentAvg.value < _resource.min 
				|| recentAvg.value > _resource.max )
		{
			MCE_DEBUG("result is out of range.");
			_outCount++;
		}
		else
		{
			MCE_DEBUG("result is in range");
			_inCount++;
		}

		// decide alert
		if(_isNormal)
		{
			if( time(NULL) - recentAvg.time > _resource.interval * _resource.warningCount )
			{
				MCE_TRACE("THIS IS A TIMEOUT.");
				UserSeq users = MonitorManagerI::instance().getUserManager()->getUsers(0);
				
				ostringstream message;
				message << "[" << _name << "]["<<_resource.name<<"][" << _resource.type << "] Timeout";

				MonitorManagerI::instance().getAlertManager()->sendAlert(users, message.str());
				_outCount = 0;
				_inCount = 0;
				_isNormal = false;
			}
			if(_outCount >= _resource.warningCount)
			{
				MCE_TRACE("THIS IS A WARNING.");
				UserSeq users = MonitorManagerI::instance().getUserManager()->getUsers(0);
				
				ostringstream message;
				message << "[" << _name << "]["<<_resource.name<<"][" << _resource.type << "]" << _resource.warningMessage << recentAvg.value;

				MonitorManagerI::instance().getAlertManager()->sendAlert(users, message.str());
				_outCount = 0;
				_inCount = 0;
				_isNormal = false;
			}
		}
		else
		{
			if(_inCount >= _resource.resumeCount)
			{
				MCE_TRACE("THIS IS A RESUME.");
				UserSeq users = MonitorManagerI::instance().getUserManager()->getUsers(0);

				ostringstream message;
				message << "[" << _name << "]["<<_resource.name<<"][" << _resource.type << "]Resumed";

				MonitorManagerI::instance().getAlertManager()->sendAlert(users, message.str());
				_outCount = 0;
				_inCount = 0;
				_isNormal = true;
			}
		}

		sleep(_resource.interval);
	}
}

