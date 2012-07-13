#ifndef MCEMONITOR_MONITORSTATUS
#define MCEMONITOR_MONITORSTATUS

#include "MceMonitor.h"
#include <IceUtil/Thread.h>
#include <deque>

namespace MceMonitor
{

typedef std::deque<Log> LogQueue;
class MonitorStatus
{
public:
	void addLog(const Log& onelog);
	LogQueue getLog(const std::string& name, const std::string& type, int count);
private:
	IceUtil::Mutex _mutex;
	std::map<std::string, std::map<std::string, LogQueue> > _status;
};

};

#endif

