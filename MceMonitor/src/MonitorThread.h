#ifndef MCEMONITOR_MONITORTHREAD
#define MCEMONITOR_MONITORTHREAD

#include "MceMonitor.h"
#include <IceUtil/Thread.h>
#include "MonitorStatus.h"

namespace MceMonitor
{

class MonitorThread
	: public IceUtil::Thread
{
public:
	MonitorThread(const std::string&, const Resource&, MonitorStatus*);
	virtual void run();
private:
	int _inCount;
	int _outCount;
	bool _isNormal;
	std::string _name;

	Resource _resource;
	MonitorStatus* _status;
};

typedef IceUtil::Handle<MonitorThread> MonitorThreadPtr;

};

#endif

