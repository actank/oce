
#include "MceMonitorI.h"
#include "MonitorStatus.h"
#include <IceUtil/Mutex.h>
#include "converter.h"

using namespace std;
using namespace MceMonitor;

void
MonitorStatus::addLog(const Log& onelog)
{
	MCE_TRACE("MonitorStatus::addLog " << log2str(onelog));

	IceUtil::Mutex::Lock lock(_mutex);
	_status[onelog.resourcegroup][onelog.resource].push_front(onelog);
	while(_status[onelog.resourcegroup][onelog.resource].size() > 10)
	{
		MCE_TRACE("MonitorStatus::addLog pop up a log");
		_status[onelog.resourcegroup][onelog.resource].pop_back();
	}
}

LogQueue
MonitorStatus::getLog(const string& rg, const string& rs, int count)
{
	MCE_TRACE("MonitorStatus::getLog name="<< rg << " type="<< rs << " count="<< count);
	IceUtil::Mutex::Lock lock(_mutex);
	LogQueue result;
	for(LogQueue::iterator it = _status[rg][rs].begin();
			it != _status[rg][rs].end();
			++it)
	{
		result.push_back(*it);
		if(--count <= 0)
		{
			break;
		};
	};
	MCE_TRACE("get log size=" << result.size());
	return result;
}

